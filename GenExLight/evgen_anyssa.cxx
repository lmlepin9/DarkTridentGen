/***********************************************************************
Description:

Toy MC generator that using modified TGenPhaseSpace

Warning:

It makes spherical decay and is unsiutable for strong cuts, e.g., 
on p_{t} of some particles.

Authors:

Radoslaw Kycia, Jacek Trunau

License:

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>



 ***********************************************************************/
//#define RStringView_H
#include <iostream>
#include <assert.h>
#include <cmath>
#include <complex>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <queue> 
//#include <cstdlib>
#include <unistd.h>
#include <sstream>

#include<TMath.h>
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

#include "TDecay.h"
#include "TGenPhaseSpace.h"

using namespace std;

TDatabasePDG * PDGDatabese = TDatabasePDG::Instance();


////////////////////////////////////////////////////////////////////////
//GLOBAL VARIABLES
////////////////////////////////////////////////////////////////////////


//global variables

//center of mass energy
//const double tecm = 200.0;


//incoming particles
const int Nip = 2; 
//outgoing particles
const int Nop = 3;
//number of decay products of V
const int Ndp = 2;

//pdg codes of particles starting from index 1
//int idIn[Nip+1] = {0, 2212,2212};		//PDGID incoming paricles
//int idOut[Nop+1] = {0, 2212,2212,211,-211}; //PDGID outgoing paricles


//global 4vectors
//in/beam particles 
TLorentzVector pb[Nip+1];
//out particles
TLorentzVector pf[Nop+1];
TLorentzVector pd[Nop+1];

const double eps = 1e-3;
const double alpha_D = 0.1;
const double alpha = 1./137.;
double mX;
double mV;

const double ee = TMath::Sqrt(4.*TMath::Pi()*alpha);
const double gp = TMath::Sqrt(4.*TMath::Pi()*alpha_D);

const double gc11 = gp; 
const double gc8 = 18.*ee*eps; // 18*ee*eps


double mel2(const TLorentzVector& pX, const TLorentzVector& pN, const TLorentzVector& pX2, const TLorentzVector& pN2, const TLorentzVector& pV) {



  const double MAp2 = pV.Mag2();
  const double Mchi2 = pX.Mag2();
  const double MNucAr2 = pN.Mag2();

  const double spVX2 = pV.Dot(pX2);
  const double spVX = pV.Dot(pX);
  const double spXX2 = pX.Dot(pX2);
  const double spSX2 = pN.Dot(pX2);
  const double spSV = pN.Dot(pV);
  const double spSX = pN.Dot(pX);

  const double q2 = -((pN2-pN).Mag2());
  const double q = TMath::Sqrt(q2);
  const double invGeV_per_fm = 5.068;
  const double A = 40.;
  const double c = (1.23*TMath::Power(A,1./3.)-0.6) /* fm */ *invGeV_per_fm;
  const double a = 0.52 /* fm */ * invGeV_per_fm;
  const double s = 0.9 /* fm */ * invGeV_per_fm;
  const double R = TMath::Sqrt(c*c+7*TMath::Pi()*TMath::Pi()*a*a/3.-5.*s*s);
  const double FF = 3.*TMath::BesselJ1(q*R)/(q*R)*TMath::Exp(-q2*s*s/2.);

  using TMath::Power;

  const double m2=FF*FF*(
      -32*Power(gc11,4)*Power(gc8,2)*(
        Power(MAp2,3)*(Mchi2*MNucAr2 + spSX*spSX2)
        + MAp2*(
          - 2*spVX*(Power(Mchi2,2)*MNucAr2 + Power(spSX2,2)*spVX - Mchi2*(Power(spSV,2) - 2*spSV*spSX - 2*spSX*spSX2 + 2*MNucAr2*spVX))
          + 2*(Power(Mchi2,2)*MNucAr2 + spVX*(2*spSV*(spSX - spSX2) + Power(spSX + spSX2,2) - MNucAr2*spVX) - Mchi2*(Power(spSV,2) + 2*spSV*spSX2 - 2*spSX*spSX2 + 2*MNucAr2*spVX))*spVX2
          + 2*(-Power(spSX,2) + MNucAr2*(2*Mchi2 + spVX))*Power(spVX2,2)
          + (-2*spSV*(spSX - spSX2)*(spVX - spVX2) + 4*spSX*spSX2*(-spVX + spVX2) + MNucAr2*(Power(spVX,2) - 6*spVX*spVX2 + Power(spVX2,2)))*spXX2
          + 2*MNucAr2*(spVX - spVX2)*Power(spXX2,2)
          )
        + Power(MAp2,2)*(Power(Mchi2,2)*MNucAr2
          - spSX*spSX2*spVX + Power(spSX2,2)*spVX
          - Mchi2*(Power(spSV,2) - 2*spSX*spSX2 + spSV*(-spSX + spSX2) + 3*MNucAr2*(spVX - spVX2))
          - Power(spSX,2)*spVX2 + spSX*spSX2*spVX2 + MNucAr2*spVX*spVX2 + 2*spSX*spSX2*spXX2
          - MNucAr2*spVX*spXX2 + MNucAr2*spVX2*spXX2 - MNucAr2*Power(spXX2,2)
          - spSV*(spSX*(spVX2 - spXX2) + spSX2*(spVX + spXX2)))
        + 2*(
          Power(Mchi2,2)*MNucAr2*(Power(spVX,2) + Power(spVX2,2))
          + Mchi2*(
            2*Power(spSV,2)*spVX*spVX2 + 2*spSV*(spSX*Power(spVX,2) - spSX2*Power(spVX2,2))
            + (Power(spVX,2) + Power(spVX2,2))*(2*spSX*spSX2 + MNucAr2*(-spVX + spVX2))
            - MNucAr2*Power(spVX + spVX2,2)*spXX2
            )
          + spVX*spVX2*(
            -2*Power(spSX2,2)*spVX + MNucAr2*Power(spVX,2) + 2*Power(spSX,2)*spVX2
            + MNucAr2*Power(spVX2,2) + 2*MNucAr2*(spVX - spVX2)*spXX2
            + 2*MNucAr2*Power(spXX2,2) - 2*spSX*spSX2*(spVX - spVX2 + 2*spXX2)
            - 2*spSV*(spSX2*(spVX2 - spXX2) + spSX*(spVX + spXX2))
            )
          )
        )
        )/(Power(MAp2 - 2*spVX,2)*Power(MAp2 + 2*spVX2,2)*Power(Mchi2 - spVX + spVX2 - spXX2,2));

  //std::cout << "m2 = "<<m2<<" "<<FF<<" "<<q2;
  //for(int i = 1; i <= 5; ++i) for(int j = i+1; j<=5; ++j) std::cout << "s["<<i<<","<<j<<"]="<<sij[i][j]<< " ";
  //std::cout<<std::endl;
  //pX.Print();
  //pN.Print();
  //pX2.Print();
  //pN2.Print();
  //pV.Print();
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
  //setup decaying particle

  //set up ingoing particles
  // A: DM particle, B: Ar40, V: dark photon
  double mB = 40.; 

  double eA = ene;
  double mA = mX;
  //double mV = 0.05;

  //setup CM frame
  //std::cout << "ene: " << ene << std::endl;

  pb[1].SetPxPyPzE( 0.0, 0.0, TMath::Sqrt(eA*eA-mA*mA), eA ); 
  pb[2].SetPxPyPzE( 0.0, 0.0, 0., mB ); //Ar40

  //central mass 4-momentum
  TLorentzVector pbCM;

  pbCM = pb[1]+pb[2];

  //std::cout << "pbCM mass: " <<  pbCM.M2() << std::endl;
  //throw std::exception();


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
  double mass[Nop] = {mA,mB,mV};

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
  weight *= 1.0 / (2.0* pbCM.M2()); //flux
  weight *= 1.0; //for now |ME|^2 is unity

  weight *= mel2(pb[1],pb[2],pf[1],pf[2],pf[3]);

  //convert GeV^-2 to mb if matrix element is in GeV^-2(natural units) - uncomment if needed
  weight *= 0.3894;


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
TTree *newcachet = 0;
TFile *newcachef = 0;
map<double,map<double,map<int,int>>> cachemap;

TFoam *get_foam(const double ene, const string& cachedir) {
  int mev = (int)(ene*1000.+1e-4);
  /*if(foams.find(mev) != foams.end()) {
    return foams[mev];
  }*/
  TFoam *foam;
  /*if(!cachet) {
    cachet = new TChain("foam");
    cachet->Add(Form("%s/*root",cachedir.c_str()));
    cachet->SetBranchStatus("*",0);
    cachet->SetBranchStatus("ene",1);
    cachet->SetBranchStatus("mV",1);
    cachet->SetBranchStatus("mX",1);
    cachet->SetBranchStatus("decay",1);
    double cacheene, mx, mv, decay;
    cachet->SetBranchAddress("ene",&cacheene);
    cachet->SetBranchAddress("mX",&mx);
    cachet->SetBranchAddress("mV",&mv);
    cachet->SetBranchAddress("decay",&decay);
    for(int i = 0; i < cachet->GetEntries(); ++i) {
      cachet->GetEntry(i);
      int mev = (int)(cacheene*1000.+1e-4);
      cachemap[mx][mv][mev]=i;
    }
    //cachet->SetBranchStatus("foam",1);
    delete cachet;
    cachet = new TChain("foam");
    cachet->Add(Form("%s/*root",cachedir.c_str()));
  }
  if(cachemap[mX][mV].find(mev) != cachemap[mX][mV].end()) {
    int entry = cachemap[mX][mV][mev];
    if(entry < 0 || entry >= cachet->GetEntries()) {
      cerr << "We should not be here!" << endl;
      return 0;
    }
    cachefoam = 0;
    cachet->SetBranchAddress("foam",&cachefoam);
    cachet->GetEntry(entry);
    foam = new TFoam;
    *foam = *cachefoam;
    TDensity    *rho= new TDensity((0.001*mev>mX+mV)?0.001*(mev+0.5):0.5*(0.001*(mev+1)+mX+mV));
    foam->SetRho(rho);
    foams[mev] = foam;
    return foam;
  }
  if(!newcachet) {
    UChar_t uuid[16];
    TUUID u;
    u.GetUUID(uuid);
    string fname = Form("%s/cache_%u.root",cachedir.c_str(),*((unsigned int*)(uuid+8)));
    newcachef = new TFile(fname.c_str(),"recreate");
    newcachet = new TTree("foam","foam");
    newcachet->Branch("foam",&cachefoam);
    newcachet->Branch("ene",&cacheene);
    newcachet->Branch("decay",&cachedecay);
    newcachet->Branch("mX",&cachemx);
    newcachet->Branch("mV",&cachemv);
  }*/

  foam = init_foam((0.001*mev>mX+mV)?0.001*(mev+0.5):0.5*(0.001*(mev+1)+mX+mV));
  //cachefoam = foam;
  //cacheene = ene;
  //cachemx = mX;
  //cachemv = mV;
  //newcachet->Fill();
  //newcachet->AutoSave();
  //cachemap[mX][mV][mev] = -1-newcachet->GetEntries();

  foams[mev] = foam;
  return foam;
}

TGenPhaseSpace *_vdecay = 0;

void generate_interaction(const double ene, const TVector3& mom,
    TLorentzVector& pV, TLorentzVector& pEp, TLorentzVector& pEm, const string& cachedir) {
  TFoam *foam = get_foam(ene, cachedir);

  foam->MakeEvent();           

  double mass[Ndp] = {0.000511,0.000511};

  _vdecay->SetDecay(pf[3], // dark photon Lorentz vector
                    Ndp,   // number of decay products
                    mass); // array of decay products masses

  _vdecay->Generate(  ); // generate a random final state

  //get out particles
  for( int i = 1; i < Ndp+1; i++ )
  {
    pd[i] = *(_vdecay->GetDecay( i-1 ));
  }

  // rotate outputs to input mom
  TVector3 dir = mom.Unit();
  pf[3].RotateUz(dir);
  pd[1].RotateUz(dir);
  pd[2].RotateUz(dir);

  pV = pf[3];
  pEp = pd[1];
  pEm = pd[2];

}

TGraph* get_xsec(const string& dir) {
  TFile *f  = TFile::Open("cross_section.root");
  TGraph *g = (TGraph *)f->Get("ggg");
  return g;
}

int main(int argc, char** argv)
{
  """
  Main function, calling all other functions. 

  """

  //inegral, its error and mc weight

  //Initialise strings for folder and file names
  string infn     = "";
  string xsecdir  = "";
  string cachedir = "cache";
  string outf     = "";
  string outhep   = "";
  string mass      = "";

  int seed = -1;

  //Get user inputs
  char c;
  while((c = getopt(argc, argv, "i:x:c:o:s:h:m:")) != -1) {
    switch(c) {
      case 'i':
        infn = optarg;
        break;
      case 'x':
        xsecdir = optarg;
        break;
      case 'c':
        cachedir = optarg;
        break;
      case 'o':
        outf = optarg;
        break;
      case 's':
        std::istringstream(optarg) >> seed;
        break;
      case 'h':
        outhep = optarg;
        break;
      case 'm':
        mass = optarg;
      default:
        break;
    }
  }

  if(infn.empty()||xsecdir.empty()) {
    cerr << "Need to supply input file " << endl;
    return -1;
  }

  //Set mX = 0.6mA
  mX = 0.6 * stod(mass);
  mV = stod(mass);

  //Read BdNMC input file into a TTree
  TTree *intree = new TTree;
  intree->ReadFile(infn.c_str(),"pi0/C:L/D:pname/C:w/D:E:px:py:pz:m:vx:vy:vz:vt:ex:ey:ez:et:L1:L2:id/L");
  double ene, len, L1, px, py, pz, vx, vy, vz, vt, impwt;
  char decay_type[100];
  Long64_t origin_id;
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
  intree->SetBranchAddress("pi0",decay_type);
  intree->SetBranchAddress("id",&origin_id);
  double maxE = intree->GetMaximum("E");
  //const double maxL = inttree->GetMaximum("L");

  cout<< "Tree contains " << intree->GetEntries() << " entries" << endl;
  
  cout << "Generating xsec graph" << endl;
  
  TGraph *xsec = get_xsec(xsecdir);
  if(!xsec) {
    cerr << "Failed to make xsec" << endl;
    return -1;
  }
  //Check that the cross section spline goes to sufficently high energies
  double maxXE, dummy;
  xsec->ComputeRange(dummy,dummy,maxXE,dummy);
  if(maxXE < maxE) {
    cout << "cross section spline not wide enough" << endl;
    return -1;
  }
  //double maxX = xsec->Eval(maxE,"S");
  //double maxW = maxX * maxL;

  cout << "Compute maximum weight" << endl;

  double maxW = 0.;

  for(int i = 0; i < intree->GetEntries(); ++i) {
    intree->GetEntry(i);
    if(ene <= mX+mV) continue; //
    double thisW = impwt * xsec->Eval(ene) * len;
    if(maxW < thisW) {
      maxW = thisW;
    }
  }

  const double invGeV2_to_mb = 0.3894; // convert xsec in GeV^-2 to mb
  cout << "Max w represents " << maxW*invGeV2_to_mb << " mb*m" << endl;
  const double density = 1.396; // g/cm3
  const double molar_mass = 40.; // g/mol
  const double avogadro = 6.022e23;
  const double mbm_to_cm3 = 1e-27 * 100.; // convert mb*m to cm3;
  const double interaction_weight = (maxW*invGeV2_to_mb*mbm_to_cm3) * density * avogadro/molar_mass;
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


  _vdecay = new TGenPhaseSpace;

  int ievt = 0;

  TRandom3 *r = new TRandom3(seed);
  TRandom3 *r_timing = new TRandom3(0);

  

  TTree *ot;
  TFile *of = 0;
  ofstream oh;
  TLorentzVector *inX = new TLorentzVector();
  TLorentzVector *vV = new TLorentzVector();
  TLorentzVector *outE = new TLorentzVector();
  TLorentzVector *outP = new TLorentzVector();
  TLorentzVector *intpos = new TLorentzVector();
  TLorentzVector *inX_pr = new TLorentzVector();
  TLorentzVector *vV_pr = new TLorentzVector();
  TLorentzVector *outE_pr = new TLorentzVector();
  TLorentzVector *outP_pr = new TLorentzVector();
  TLorentzVector *intpos_pr = new TLorentzVector();

  double thisW;
  


  if(!outf.empty()) {
    
    of = new TFile(outf.c_str(), "recreate");
    ot = new TTree("events","events");
    ot->SetWeight(interaction_weight);
    ot->Branch("vtx", &intpos);
    

    ot->Branch("inX",&inX);
    ot->Branch("vV",&vV);
    ot->Branch("outE",&outP);
    ot->Branch("outP",&outE);
    
    ot->Branch("vtx_pr",&intpos_pr);
    ot->Branch("inX_pr",&inX_pr);
    ot->Branch("vV_pr",&vV_pr);
    ot->Branch("outE_pr",&outP_pr);
    ot->Branch("outP_pr",&outE_pr);

    ot->Branch("weight",&thisW);
    ot->Branch("max_weight",&maxW);
    cout << outf << endl;
  }

  

  if(!outhep.empty()){
    oh.open(outhep.c_str());
  }

  for(int i = 0; i < intree->GetEntries(); ++i) {
    intree->GetEntry(i);
    cout << i << endl;
    cout << "i: " << i << " ene: " << ene << " mX+mV: " << mX+mV << endl;
    if(ene <= mX+mV) {
      cout << "kinematically forbidden" << endl;
      continue;
    }

    thisW = impwt * xsec->Eval(ene) * len;
    cout << "Weight: " << thisW << " max weight: " << maxW << " ratio: " << thisW/maxW << endl; 
    if(r->Uniform() < thisW/maxW){
      TVector3 xmom(px,py,pz);
      double lpos = r->Uniform() * len + L1;
      TVector3 orig(vx,vy,vz);
      const TVector3& dir = xmom.Unit();
      TVector3 vtx = orig + lpos * dir;
      TLorentzVector dgam, epos, eneg;
      TLorentzVector dgam_pr, epos_pr, eneg_pr, vX_pr, vertex_pr;
      generate_interaction(ene, xmom, dgam, epos, eneg, cachedir);

      vt *= 1e9; // time is in s, needs to be in ns
      
      if(of) {
        TLorentzVector vX(xmom.X(),xmom.Y(),xmom.Z(),ene);
        TLorentzVector vertex(vtx.X(),vtx.Y(),vtx.Z(),vt);
        vertex_pr = vertex;
        vX_pr = vX;
        dgam_pr = dgam;
        epos_pr = epos;
        eneg_pr = eneg;
      }
      // transform all vectors into detector coordinate system
      vtx -= det_centre;
      orig -= det_centre;
      vtx *= det_rot;
      orig *= det_rot;
      const double cm = 100.; // cm per m
      const double c = 0.299792; // m per ns
      const TVector3 Vbeam(0,0,-4.); 
      const double t0 = (det_centre-Vbeam).Mag() / c; 
      double ivt = vt + lpos / ( xmom.Mag()/ene * c) - t0;

      const double beam_time = 4687.5 + r_timing->Uniform() * 9600.;
      ivt += beam_time;
      
      xmom *= det_rot;
      dgam *= det_rot;
      epos *= det_rot;
      eneg *= det_rot;

      oh << ievt << " 4 " << decay_type << " " << origin_id << "\n";
      oh << 2 << " " << 41 << " " << 0 << " " << 0 << " " << 2 <<" "<< 2 << " "
        << xmom.X() <<" " << xmom.Y() << " " << xmom.Z() <<" " <<ene <<" "<< mX <<" "
        << orig.X() * cm << " " << orig.Y() * cm << " " << orig.Z() * cm << " " << vt << "\n";
      oh << 2 << " " << 80 << " " << 1 << " " << 1 << " " << 3 <<" "<< 4 << " "
        << dgam.X() <<" " << dgam.Y() << " " << dgam.Z() <<" " <<dgam.E() <<" "<< dgam.M()  <<" "
        << vtx.X() * cm << " " << vtx.Y() * cm << " " << vtx.Z() * cm << " " << ivt << "\n";
      oh << 1 << " " << 11 << " " << 2 << " " << 2 << " " << 0 <<" "<< 0 << " "
        << epos.X() <<" " << epos.Y() << " " << epos.Z() <<" " <<epos.E() <<" "<< epos.M()  <<" "
        << vtx.X() * cm << " " << vtx.Y() * cm << " " << vtx.Z() * cm << " " << ivt << "\n";
      oh << 1 << " " << -11 << " " << 2 << " " << 2 << " " << 0 <<" "<< 0 << " "
        << eneg.X() <<" " << eneg.Y() << " " << eneg.Z() <<" " <<eneg.E() <<" "<< eneg.M()  <<" "
        << vtx.X() * cm << " " << vtx.Y() * cm << " " << vtx.Z() * cm << " " << ivt << "\n";
      

      cout << ievt << " " << 4 << " " << decay_type << " " << origin_id << endl;
      //    status      pdg         mother1     mother2     daugher1  daughter2
      cout << 2 << " " << 41 << " " << 0 << " " << 0 << " " << 2 <<" "<< 2 << " "
        << xmom.X() <<" " << xmom.Y() << " " << xmom.Z() <<" " <<ene <<" "<< mX <<" "
        << orig.X() * cm << " " << orig.Y() * cm << " " << orig.Z() * cm << " " << vt <<endl;
      cout << 2 << " " << 80 << " " << 1 << " " << 1 << " " << 3 <<" "<< 4 << " "
        << dgam.X() <<" " << dgam.Y() << " " << dgam.Z() <<" " <<dgam.E() <<" "<< dgam.M()  <<" "
        << vtx.X() * cm << " " << vtx.Y() * cm << " " << vtx.Z() * cm << " " << ivt <<endl;
      cout << 1 << " " << -11 << " " << 2 << " " << 2 << " " << 0 <<" "<< 0 << " "
        << epos.X() <<" " << epos.Y() << " " << epos.Z() <<" " <<epos.E() <<" "<< epos.M()  <<" "
        << vtx.X() * cm << " " << vtx.Y() * cm << " " << vtx.Z() * cm << " " << ivt <<endl;
      cout << 1 << " " << 11 << " " << 2 << " " << 2 << " " << 0 <<" "<< 0 << " "
        << eneg.X() <<" " << eneg.Y() << " " << eneg.Z() <<" " <<eneg.E() <<" "<< eneg.M()  <<" "
        << vtx.X() * cm << " " << vtx.Y() * cm << " " << vtx.Z() * cm << " " << ivt <<endl;
      ievt++;

      if(of) {
        TLorentzVector vX(xmom.X(),xmom.Y(),xmom.Z(),ene);
        TLorentzVector vertex(vtx.X(),vtx.Y(),vtx.Z(),ivt);
        intpos = &vertex;
        inX = &vX;
        vV = &dgam;
        outE = &eneg;
        outP = &epos;
        
        intpos_pr = &vertex_pr;
        inX_pr = &vX_pr;
        vV_pr = &dgam_pr;
        outE_pr = &eneg_pr;
        outP_pr = &epos_pr;
        
        ot->Fill();
      }
    }
    else cout << "Statistically ignored" << endl;
  }
  if(of) {
    of->cd();
    ot->Write();
    of->Close();
  }

  if(oh){
    oh.close();
  }

  /*if(newcachef && newcachet) {
    newcachef->cd();
    //newcachet->Write();
    newcachef->Close();
    delete newcachef;
  }*/
}
