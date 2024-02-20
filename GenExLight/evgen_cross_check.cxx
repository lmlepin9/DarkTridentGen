/*


Script to cross check the DM scattering with Ar. 


*/

// Import std dependencies 
#include <iostream>
#include <assert.h>
#include <cmath>
#include <complex>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <queue> 
#include <unistd.h>
#include <sstream>
#include <stdlib.h> 

// Import root dependencies 
#include <TMath.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TChain.h>
#include <TRandom3.h>
#include <TFoam.h>
#include <TFoamIntegrand.h>
#include <TLorentzVector.h>
#include <TDatabasePDG.h>
#include <TGraph.h>
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TDecay.h"
#include "TGenPhaseSpace.h"

using namespace std;

TDatabasePDG * PDGDatabese = TDatabasePDG::Instance();


////////////////////////////////////////////////////////////////////////
//GLOBAL VARIABLES
////////////////////////////////////////////////////////////////////////
//incoming particles
const int Nip = 2; 
//outgoing particles
const int Nop = 2;


//global 4vectors
//in/beam particles 
TLorentzVector pb[Nip+1];
//out particles
TLorentzVector pf[Nop+1];


const double eps = 1e-3;
const double alpha_D = 0.1;
const double alpha = 1./137.;
const double ee = TMath::Sqrt(4.*TMath::Pi()*alpha);
const double gp = TMath::Sqrt(4.*TMath::Pi()*alpha_D);
const double gc11 = gp; 
const double gc8 = 18.*ee*eps; // 18*ee*eps

double mX;
double mV;
double m_e = 0.5e-3;
string dm_type;
double q2_out;
double n_e = 18.;



// Matrix element for DM-Ar scattering 
double mel2(const TLorentzVector& pX, const TLorentzVector& pN, const TLorentzVector& pX2, const TLorentzVector& pN2) {
  const double k2p1 = pN2.Dot(pX);
  const double p2p1 = pN.Dot(pX);
  const double k2p2 = pN2.Dot(pN);
  const double p1p1 = pX.Dot(pX);
  const double p2k1 = pN.Dot(pX2);
  const double k2k1 = pN2.Dot(pX2);
  const double p1k2 = pX.Dot(pN2);
  const double k1k1 = pX2.Dot(pX2);
  const double p1k1 = pX.Dot(pX2);
  const double k1p1 = pX2.Dot(pX);
  const double T = (pX2 - pX)*(pX2 - pX);
  const double m_ar = m_e;

  const double q2 = -((pN2-pN).Mag2());
  q2_out = -((pN2-pN).Mag2());

  // Helm FF 
  const double q = TMath::Sqrt(q2);
  const double invGeV_per_fm = 5.068;
  const double A = 40.;
  const double c = (1.23*TMath::Power(A,1./3.)-0.6) /* fm */ *invGeV_per_fm;
  const double a = 0.52 /* fm */ * invGeV_per_fm;
  const double s = 0.9 /* fm */ * invGeV_per_fm;
  const double R = TMath::Sqrt(c*c+7*TMath::Pi()*TMath::Pi()*a*a/3.-5.*s*s);
  const double FF = 3. * std::sph_bessel(1, q*R)/(q*R)*TMath::Exp(-q2*s*s/2.); // Spherical Bessel function 

  using TMath::Power;
  double m2; 

// Matrix element for 2-2 scattering of a scalar DM with a fermion target (Taken from Daedalus paper)
    m2=(32.*Power(eps,2)*Power(TMath::Pi(),2)*alpha*alpha_D)*(k2p1*p2p1 + k2p1*p2p1 - k2p2*p1p1 + k2p1*p2k1 + k2k1*p2p1
    - k2p2*p1k1 + k2k1*p2k1 + k2k1*p2k1 - k2p2*k1k1 + k2k1*p2p1
    + k2p1*p2k1 - k2p2*k1p1 + Power(m_ar,2)*(Power(mX,2) + Power(mX,2) + 2*p1k1))/Power(T - Power(mV,2),2);

  return m2;
}





////////////////////////////////////////////////////////////////////////
//TDensity DEFINITION
////////////////////////////////////////////////////////////////////////

///Class density for Foam
class TDensity: public TFoamIntegrand 
{
  private:	

    ///decay object
    TDecay * _decay;

    ///queue for random numbers
    queue<double> rndQueue;

    ///weight of the event
    Double_t eventWeight;
    Double_t ene;


  public:

    /// Constructor
    TDensity(double e){ _decay = new TDecay(); ene=e;};

    /// Destructor
    virtual ~TDensity(){ delete _decay; };

    /// @returns weight of the process
    /// @param nDim  dimension of integration
    /// @param Xarg vector of probablilistic variables from [0;1] of dim nDim
    /// @warning it is required by Foam integrator
    Double_t Density(int nDim, Double_t *Xarg);
    ///returns weight of current event
    Double_t getEventWeight( void ){ return eventWeight; };

};

////////////////////////////////////////////////////////////////////////
//TDensity IMPLEMENTATION
////////////////////////////////////////////////////////////////////////

Double_t TDensity::Density(int nDim, Double_t *Xarg)
{	

  //set up ingoing particles
  double mB = m_e; 
  double eA = ene;

  // This is the mass of the DM, do not confuse it with the dark photon! 
  double mA = mX;

  //setup CM frame
  pb[1].SetPxPyPzE( 0.0, 0.0, TMath::Sqrt(eA*eA-mA*mA), eA );
  pb[2].SetPxPyPzE( 0.0, 0.0, 0., mB );

  //central mass 4-momentum
  TLorentzVector pbCM;
  pbCM = pb[1]+pb[2];

  //make decay 
  //clear queue - in case there was error in previous run
  while (!rndQueue.empty())
  {
    rndQueue.pop();
  }

  //put rnd numbers into queue
  for( int i = 0; i < nDim; i++)
  {
    rndQueue.push( Xarg[i] );
  }  

  //masses of products
  double mass[Nop] = {mA,mB};

  //for(int i=0; i < Nop; i++)
  //{
  //	mass[i] = (i==1)?mA:(i==2?mB:mV);
  //}

  //set decay configuration
  _decay->SetDecay(pbCM, Nop, mass);

  //make decay
  double wtdecay =  2 * M_PI * _decay->Generate( rndQueue );

  //get out particles
  for( int i = 1; i < Nop+1; i++ )
  {
    pf[i] = *(_decay->GetDecay( i-1 ));
  }
  //|Matrix Element|^2
  double weight = wtdecay;
  double F = TMath::Sqrt(TMath::Power(pb[1].Dot(pb[2]),2) - mA*mA*mB*mB);
	weight *= 1.0/(4.0*F);
  //weight *= 1.0 / (2.0* pbCM.M2()); //flux why we divide by 2 here???? 
  weight *= 1.0; //for now |ME|^2 is unity
  weight *= mel2(pb[1],pb[2],pf[1],pf[2]);

  //convert GeV^-2 to mb if matrix element is in GeV^-2(natural units) - uncomment if needed
  //weight *= 0.3894;
  //setup event weight
  eventWeight = weight;
  return( weight );

};


TFoam* init_foam(const double ene) {

  Int_t  kDim   =     3*Nop-4;   // total dimension
  Int_t  nCells   =     10000;   // Number of Cells
  Int_t  nSampl   =     10000;   // Number of MC events per cell in build-up
  Int_t  nBin     =       8;   // Number of bins in build-up
  Int_t  OptRej   =       1;   // Wted events for OptRej=0; wt=1 for OptRej=1 (default)
  Int_t  OptDrive =       2;   // (D=2) Option, type of Drive =0,1,2 for TrueVol,Sigma,WtMax
  Int_t  EvPerBin =      25;   // Maximum events (equiv.) per bin in buid-up
  Int_t  Chat     =       1;   // Chat level
  //=========================================================
  TRandom *PseRan   = new TRandom3();  // Create random number generator
  TFoam *foam    = new TFoam("FoamX");   // Create Simulator
  TDensity    *rho= new TDensity(ene);   //Create density
  PseRan->SetSeed(4357);
  //=========================================================
  //cout<<"*****   Demonstration Program for Foam version "<<FoamX->GetVersion()<<"    *****"<<endl;
  foam->SetkDim(        kDim);      // Mandatory!!!
  foam->SetnCells(      nCells);    // optional
  foam->SetnSampl(      nSampl);    // optional
  foam->SetnBin(        nBin);      // optional
  foam->SetOptRej(      OptRej);    // optional
  foam->SetOptDrive(    OptDrive);  // optional
  foam->SetEvPerBin(    EvPerBin);  // optional
  foam->SetChat(        Chat);      // optional
  //===============================
  foam->SetRho(rho);
  foam->SetPseRan(PseRan);

  // Initialize simulator
  foam->Initialize(); 


  //rho->ene=ene;

  //long nCalls=FoamX->GetnCalls();

  return foam;
}

map<int,TFoam*> foams;

TFoam *cachefoam = 0;
double cacheene, cachemv, cachemx, cachedecay;
TChain *cachet = 0;
//TTree *newcachet = 0;
//TFile *newcachef = 0;
map<double,map<double,map<int,int>>> cachemap;

TFoam *get_foam(const double ene, const string& cachedir) {
  int mev = (int)(ene*1000.+1e-4);
  TFoam *foam;
  foam = init_foam((0.001*mev>mX+mV)?0.001*(mev+0.5):0.5*(0.001*(mev+1)+mX+mV));
  foams[mev] = foam;
  return foam;
}


void generate_interaction(const double ene, const TVector3& mom, const string& cachedir) {
  TFoam *foam = get_foam(ene, cachedir);
  foam->MakeEvent();           
  // rotate outputs to input mom
  TVector3 dir = mom.Unit();
  // This is all new stuff (4-momentum of x and Ar after scattering (?))
  pf[1].RotateUz(dir);
  pf[2].RotateUz(dir);
  pf[3].RotateUz(dir);
}


TGraph* get_xsec(const string& file) {
  TFile *f  = TFile::Open(file.c_str());
  TGraph *g = (TGraph *)f->Get("gxsec");
  return g;
}


int main(int argc, char** argv)
{

  //inegral, its error and mc weight
  string infn = "";
  string xsecdir = "";
  string cachedir = "cache";
  string outf = "";
  string outn = "";
  string inputmode = "txt"; // Default input mode: txt 
  string dm = "scalar"; // 
  string mass = "";
  string ratio = "";


  int seed = -1;

  char c;
  while((c = getopt(argc, argv, "i:x:c:o:s:")) != -1) {
    switch(c) {
      case 'i':
        infn = optarg; // input files
        break;
      case 'x':
        xsecdir = optarg; // xsec directory
        break;
      case 'c':
        cachedir = optarg; // cache directory 
        break;
      case 'o':
        outf = optarg;  // output file 
        break;
      case 's':
        std::istringstream(optarg) >> seed; // seed value 
        break;
      default:
        break;
    }
  }
  
  if(infn.empty()||xsecdir.empty()) {
    cerr << "Need to supply input file " << endl;
    return -1;
  }


  double target_pot = 2e20;
  mX = 0.012;
  mV = 0.02;
  inputmode="root";
  dm_type = "scalar";
  cout << "Dark matter type: " << dm_type << endl; 
  cout << "Dark matter mass: " << mX << endl;
  cout << "Dark photon mass: " << mV << endl;

  TTree *intree = new TTree;
  double ene, len, L1, px, py, pz, vx, vy, vz, vt, impwt, maxE;
  std::string* name=0;
  Int_t origin_id;
  TTree *pot_tree = new TTree;
  double tot_pot, pot_per_event;
  double pot_final; 

  if(inputmode=="txt"){
    intree->ReadFile(infn.c_str(),"pi0/C:L/D:pname/C:w/D:E:px:py:pz:m:vx:vy:vz:vt:ex:ey:ez:et:L1:L2:id/L");
    intree->SetBranchAddress("E",&ene);
    intree->SetBranchAddress("L",&len);
    intree->SetBranchAddress("L1",&L1);
    intree->SetBranchAddress("w",&impwt);
    intree->SetBranchAddress("px",&px);
    intree->SetBranchAddress("py",&py);
    intree->SetBranchAddress("pz",&pz);
    intree->SetBranchAddress("vx",&vx);
    intree->SetBranchAddress("vy",&vy);
    intree->SetBranchAddress("vz",&vz);
    intree->SetBranchAddress("vt",&vt);
    intree->SetBranchAddress("pi0",&name);
    intree->SetBranchAddress("id",&origin_id);
    maxE = intree->GetMaximum("E");
  }

  else if(inputmode=="root"){
    TFile *fin = TFile::Open(infn.c_str());
    intree = (TTree *) fin->Get("event_tree");
    intree->SetBranchAddress("dm_energy",&ene);
    intree->SetBranchAddress("dm_L",&len);
    intree->SetBranchAddress("dm_L1",&L1);
    intree->SetBranchAddress("dm_weight",&impwt);
    intree->SetBranchAddress("dm_px",&px);
    intree->SetBranchAddress("dm_py",&py);
    intree->SetBranchAddress("dm_pz",&pz);
    intree->SetBranchAddress("dm_origin_x",&vx);
    intree->SetBranchAddress("dm_origin_y",&vy);
    intree->SetBranchAddress("dm_origin_z",&vz);
    intree->SetBranchAddress("dm_origin_t0",&vt);
    intree->SetBranchAddress("channel_name", &name);
    intree->SetBranchAddress("id",&origin_id);
    maxE = intree->GetMaximum("dm1_energy");
    pot_tree = (TTree *)fin->Get("pot_tree");
    pot_tree->SetBranchAddress("tot_pot",&tot_pot);
    pot_tree->GetEntry(0);
    //delete fin; 
  }

  TGraph *xsec = get_xsec(xsecdir);
  if(!xsec) {
    cerr << "Failed to make xsec" << endl;
    return -1;
  }


  double maxXE, dummy;
  xsec->ComputeRange(dummy,dummy,maxXE,dummy);
  if(maxXE < maxE) {
    cout << "cross section spline not wide enough" << endl;
    return -1;
  }


  double maxW = 0.;
  for(int i = 0; i < intree->GetEntries(); ++i) {
    intree->GetEntry(i);
    if(ene <= mX+mV) continue;
    double thisW = impwt * xsec->Eval(ene) * len;
    if(maxW < thisW) {
      maxW = thisW;
    }
  }

  for(int i = 0; i < pot_tree->GetEntries(); i++){
    pot_tree->GetEntry(i);
    cout << "Partial POT: " << tot_pot << endl;
    pot_final += tot_pot; 

  }

  const double invGeV2_to_mb = 0.3894; // convert xsec in GeV^-2 to mb
  cout << "Max w represents " << maxW*invGeV2_to_mb << " mb*m" << endl;
  const double density = 1.396; // g/cm3
  const double molar_mass = 40.; // g/mol
  const double avogadro = 6.022e23;
  const double mbm_to_cm3 = 1e-27 * 100.; // convert mb*m to cm3;
  const double interaction_weight = n_e*(maxW*invGeV2_to_mb*mbm_to_cm3)*density*avogadro/molar_mass;
  cout << "1 tree entry represents " << interaction_weight << " interactions" << endl;
  // translation and rotation from beamline coordinate system
  // to detector coordinate system
  const TVector3 det_centre(55.02, 72.59,  672.70);
  const TRotation det_rot = ([=]() -> TRotation {
    TRotation R;
    const TVector3 newX(0.92103853804025682, 0.0000462540012621546684, -0.38947144863934974);
    const TVector3 newY(0.0227135048039241207, 0.99829162468141475, 0.0538324139386641073);
    const TVector3 newZ(0.38880857519374290, -0.0584279894529063024, 0.91946400794392302);
    R.RotateAxes(newX,newY,newZ);
    return R;
  })();


  int ievt = 0;
  TRandom3 *r = new TRandom3(seed);
  TRandom3 *r_timing = new TRandom3(seed+1);


  std::cout << "Iterating over DM flux..." << std::endl;
  for(int i = 0; i < intree->GetEntries(); ++i) {
    intree->GetEntry(i);
    if(ene <= mX+mV) continue;
    double thisW = impwt * xsec->Eval(ene) * len;
    if(r->Uniform() < thisW/maxW) {
      std::cout<<"Event: " << ievt << std::endl;
      TVector3 xmom(px,py,pz);
      double lpos = r->Uniform() * len + L1;
      TVector3 orig(vx,vy,vz);
      const TVector3& dir = xmom.Unit();
      TVector3 vtx = orig + lpos * dir;
      generate_interaction(ene, xmom, cachedir);
      //cout <<"Transferred momentum to Ar q2: " << q2_out << endl;
     ievt++;
    }
  }
  std::cout << "Number of simulated interactions for 2e20 POT: " << interaction_weight*ievt*(target_pot)/(pot_final)<< std::endl;

}
