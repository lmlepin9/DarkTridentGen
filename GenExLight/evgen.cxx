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
// SCRIPT TO USE WITH GenExLight. The output is a hepevt file which can be processed with LArSoft 

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
#include <stdlib.h> 

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


//global variables


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
const double ee = TMath::Sqrt(4.*TMath::Pi()*alpha);
const double gp = TMath::Sqrt(4.*TMath::Pi()*alpha_D);
const double gc11 = gp; 
const double gc8 = 18.*ee*eps; // 18*ee*eps

double mX;
double mV;
string dm_type;
double q2_out;


double mel2(const TLorentzVector& pX, const TLorentzVector& pN, const TLorentzVector& pX2, const TLorentzVector& pN2, const TLorentzVector& pV) {



  const double MAp2 = pV.Mag2();
  const double MAp4 = std::pow(MAp2,2);
  const double MAp6 = std::pow(MAp2,3);
  const double MAp8 = std::pow(MAp2,4);
  const double Mchi2 = pX.Mag2();
  const double MSChi2 = Mchi2;
  const double MNucAr2 = pN.Mag2();

  const double spVX2 = pV.Dot(pX2);
  const double spVX = pV.Dot(pX);
  const double spXX2 = pX.Dot(pX2);
  const double spSX2 = pN.Dot(pX2);
  const double spSV = pN.Dot(pV);
  const double spSX = pN.Dot(pX);

  const double spSX2_2 = std::pow(spSX2,2);
  const double spSV_2 = std::pow(spSV,2);
  const double spVX2_2 = std::pow(spVX2,2);
  const double spVX_2 = std::pow(spVX,2);
  const double spSX_2 = std::pow(spSX,2);

  const double spSX_spSX2_2 = std::pow(spSX + spSX2,2);
  const double spSV_spSX_spSX2_2 = std::pow(spSV + spSX + spSX2,2);
  const double mspSV_spSX_spSX2_2 = std::pow(-spSV + spSX + spSX2,2);

  const double q2 = -((pN2-pN).Mag2());
  q2_out = -((pN2-pN).Mag2());
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

  // According to Mathematica's results, these are the right matrix elements (2->3 scattering). 
  // A factor 1/2 is added in the fermion case to acount for the spin average 
  if(dm_type == "fermion"){
      m2=FF*FF*(1./2.)*(
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

  }

  else if(dm_type == "scalar"){
    m2 = FF*FF*
  					(-4*std::pow(gc11,4)*std::pow(gc8,2))*
					(MAp8*MNucAr2
           				- MAp6*(spSV_2 + 2*spSV*(-spSX + spSX2) + 2*spSX_spSX2_2 + 4*MNucAr2*(spVX - spVX2))
           				+ 4*MSChi2*(spSV_spSX_spSX2_2*spVX_2 + mspSV_spSX_spSX2_2*spVX2_2)
           				+ 2*MAp4*(MSChi2*(spSV_2 + spSX_spSX2_2) + 2*spVX*((spSX + spSX2)*(spSX + 2*spSX2) + MNucAr2*spVX) - 2*((spSX + spSX2)*(2*spSX + spSX2) + 4*MNucAr2*spVX)*spVX2 + 2*MNucAr2*spVX2_2 - spSV*(spSX\
						*spVX - 5*spSX2*spVX - 5*spSX*spVX2 + spSX2*spVX2) + spSV_2*(spVX - spVX2 - spXX2) + spSX_spSX2_2*spXX2)
           				+ 8*spVX*spVX2*(-2*spSX2*(spSV + spSX + spSX2)*spVX + 2*(spSX*(-spSV + spSX + spSX2) + MNucAr2*spVX)*spVX2 + (spSV - spSX - spSX2)*(spSV + spSX + spSX2)*spXX2)
           				- MAp2*(spSX_2*spVX_2 + 10*spSX*spSX2*spVX_2 + 9*spSX2_2*spVX_2 - 14*spSX_2*spVX*spVX2
               			- 28*spSX*spSX2*spVX*spVX2 - 14*spSX2_2*spVX*spVX2 - 16*MNucAr2*spVX_2*spVX2 + 9*spSX_2*spVX2_2 + 10*spSX*spSX2*spVX2_2
               			+ spSX2_2*spVX2_2 + 16*MNucAr2*spVX*spVX2_2 + 4*MSChi2*(spSV_spSX_spSX2_2*spVX - mspSV_spSX_spSX2_2*spVX2)
               			+ 2*spSV*((spSX + 5*spSX2)*spVX_2 + 8*(spSX - spSX2)*spVX*spVX2 - (5*spSX + spSX2)*spVX2_2) + spSV_2*(spVX - spVX2)*(spVX - spVX2 - 4*spXX2)
               			+ 4*spSX_spSX2_2*(spVX - spVX2)*spXX2)
					)/(Power(MAp2 - 2*spVX,2)*Power(MAp2 + 2*spVX2,2)*Power(Mchi2 - spVX + spVX2 - spXX2,2));

  }

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

  double mB = 40.; 

  double eA = ene;
  double mA = mX;
  //double mV = 0.05;

  //setup CM frame
  //std::cout << "ene: " << ene << std::endl;

  pb[1].SetPxPyPzE( 0.0, 0.0, TMath::Sqrt(eA*eA-mA*mA), eA );
  pb[2].SetPxPyPzE( 0.0, 0.0, 0., mB );

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
  double F = TMath::Sqrt(TMath::Power(pb[1].Dot(pb[2]),2) - mA*mA*mB*mB);
	weight *= 1.0/(4.0*F); // Flux factor fixed! 
  weight *= 1.0; //for now |ME|^2 is unity

  weight *= mel2(pb[1],pb[2],pf[1],pf[2],pf[3]);

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


TGenPhaseSpace *_vdecay = 0;

void generate_interaction(const double ene, const TVector3& mom,
    TLorentzVector& pV, TLorentzVector& pEp, TLorentzVector& pEm, const string& cachedir) {
    TFoam *foam = get_foam(ene, cachedir);

    foam->MakeEvent();           

    double mass[Ndp] = {0.000511,0.000511};
    _vdecay->SetDecay(pf[3], Ndp, mass);
    _vdecay->Generate(  );
    //get out particles
    for( int i = 1; i < Ndp+1; i++ )
    {
      pd[i] = *(_vdecay->GetDecay( i-1 ));
    }

    // rotate outputs to input mom
    TVector3 dir = mom.Unit();

    // This is all new stuff (4-momentum of x and Ar after scattering (?))
    pf[1].RotateUz(dir);
    pf[2].RotateUz(dir);

    pf[3].RotateUz(dir);
    pd[1].RotateUz(dir);
    pd[2].RotateUz(dir);

    pV = pf[3];
    pEp = pd[1];
    pEm = pd[2];

}


TGraph* get_xsec(const string& file) {
  TFile *f  = TFile::Open(file.c_str());
  TGraph *g = (TGraph *)f->Get("gxsec");
  return g;
}



void DumpHepevt(TTree *out_tree, string &file){
  // Here we dump all the information in to a hepevt file, including POT per event. 
  // TLorentzVector pointer should point to something, don't know why 
  const double cm = 100.;
  TLorentzVector *vtx = 0;
  TLorentzVector *xmom = 0;
  TLorentzVector *dgam = 0;
  TLorentzVector *eneg = 0;
  TLorentzVector *epos = 0;
  TLorentzVector *orig = 0 ;
  TLorentzVector *inX_pr = 0;
  TLorentzVector *vV_pr = 0;
  TLorentzVector *outE_pr = 0;
  TLorentzVector *outP_pr = 0;
  TLorentzVector *intpos_pr = 0;
  Double_t out_q2, w, out_pot;
  Int_t origin_id; 
  std::string *name = 0;

  out_tree->SetBranchAddress("evt_weight",&w);
  out_tree->SetBranchAddress("vtx",&vtx);
  out_tree->SetBranchAddress("inX",&xmom);
  out_tree->SetBranchAddress("origin",&orig);
  out_tree->SetBranchAddress("origin_id",&origin_id);
  out_tree->SetBranchAddress("vV",&dgam);

  // These two are swapped, for some reason...
  out_tree->SetBranchAddress("outE",&epos);
  out_tree->SetBranchAddress("outP",&eneg);

  out_tree->SetBranchAddress("vtx_pr",&intpos_pr);
  out_tree->SetBranchAddress("inX_pr",&inX_pr);
  out_tree->SetBranchAddress("vV_pr",&vV_pr);
  out_tree->SetBranchAddress("outE_pr",&outP_pr);
  out_tree->SetBranchAddress("outP_pr",&outE_pr);
  out_tree->SetBranchAddress("q2",&out_q2);
  out_tree->SetBranchAddress("origin_name",&name);
  out_tree->SetBranchAddress("total_pot",&out_pot);

  ofstream outputFile;
  cout << "Writing hepevt file: " << file + ".txt" << endl; 
  outputFile.open((file+".txt").c_str());
  Int_t n_entries = (Int_t) out_tree->GetEntries();
  Double_t tree_weight = (Double_t) out_tree ->GetWeight(); 
  cout << "Number of entries: " << n_entries << endl; 

  for(Int_t ievt = 0; ievt < n_entries; ievt++){
      out_tree->GetEntry(ievt);
      outputFile << ievt << " " << 4 << " " << *name << " " << origin_id << " " << (out_pot/n_entries)/tree_weight << " " << out_q2 << endl; 
       //    status      pdg         mother1     mother2     daugher1  daughter2
      outputFile << 2 << " " << 41 << " " << 0 << " " << 0 << " " << 2 <<" "<< 2 << " "
		 << xmom->X() <<" " << xmom->Y() << " " << xmom->Z() <<" " << xmom->T() <<" "<< xmom->M() <<" "
		 << orig->X() * cm << " " << orig->Y() * cm << " " << orig->Z() * cm << " " << orig->T() <<endl;
      outputFile << 2 << " " << 80 << " " << 1 << " " << 1 << " " << 3 <<" "<< 4 << " "
		 << dgam->X() <<" " << dgam->Y() << " " << dgam->Z() <<" " <<dgam->E() <<" "<< dgam->M()  <<" "
		 << vtx->X() * cm << " " << vtx->Y() * cm << " " << vtx->Z() * cm << " " << vtx->T() <<endl;
      outputFile << 1 << " " << -11 << " " << 2 << " " << 2 << " " << 0 <<" "<< 0 << " "
		 << epos->X() <<" " << epos->Y() << " " << epos->Z() <<" " <<epos->E() <<" "<< epos->M()  <<" "
		 << vtx->X() * cm << " " << vtx->Y() * cm << " " << vtx->Z() * cm << " " << vtx->T() <<endl;
      outputFile << 1 << " " << 11 << " " << 2 << " " << 2 << " " << 0 <<" "<< 0 << " "
		 << eneg->X() <<" " << eneg->Y() << " " << eneg->Z() <<" " <<eneg->E() <<" "<< eneg->M()  <<" "
		 << vtx->X() * cm << " " << vtx->Y() * cm << " " << vtx->Z() * cm << " " << vtx->T() <<endl;
  }
  outputFile.close();

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
  while((c = getopt(argc, argv, "i:x:c:o:s:f:t:m:r:")) != -1) {
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
      case 'f':
        inputmode = optarg; // input mode (root or text)
        break;
      case 't':
        dm = optarg; 
      case 'm':
        mass = optarg; // Need to provide dark photon mass
      case 'r':
        ratio = optarg; // Ratio of dark sector particles 
      default:
        break;
    }
  }
  
  if(infn.empty()||xsecdir.empty()) {
    cerr << "Need to supply input file " << endl;
    return -1;
  }

  mX = stod(ratio) * stod(mass);
  mV = stod(mass);
  dm_type = dm;
  cout << "Dark matter type: " << dm_type << endl; 
  cout << "Dark photon mass: " << mass << endl;
  cout << "Dark sector mass ratio: " << ratio << endl; 

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
    cout << "Partial POT" << tot_pot << endl;
    pot_final += tot_pot; 

  }

  const double invGeV2_to_mb = 0.3894; // convert xsec in GeV^-2 to mb
  cout << "Max w represents " << maxW*invGeV2_to_mb << " mb*m" << endl;
  const double density = 1.396; // g/cm3
  const double molar_mass = 40.; // g/mol
  const double avogadro = 6.022e23;
  const double mbm_to_cm3 = 1e-27 * 100.; // convert mb*m to cm3;
  const double interaction_weight = (maxW*invGeV2_to_mb*mbm_to_cm3)*density*avogadro/molar_mass;
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
  
  // hepevt output moved to external function 

  TRandom3 *r = new TRandom3(seed);
  TRandom3 *r_timing = new TRandom3(seed+1);


  bool root_option = true;


  // Declarations for output ROOT TTree
  TFile *of;
  TTree *ot;
  TLorentzVector inX, vV, outE, outP, intpos, origin;
  TLorentzVector inX_pr, vV_pr, outE_pr, outP_pr, intpos_pr, out_Ar;
  Int_t org_id;
  Double_t out_q2, weight, out_pot;
  std::string *name_out = 0;


  if(root_option) {
    of = new TFile((outf+".root").c_str(), "RECREATE");
    ot = new TTree("event_tree","Tree with events after evgen");
    ot->SetWeight(interaction_weight);
    ot->Branch("evt_weight",&weight);
    ot->Branch("origin_id",&org_id);
    ot->Branch("origin",&origin);
    ot->Branch("vtx",&intpos);
    ot->Branch("inX",&inX);
    ot->Branch("vV",&vV);
    ot->Branch("outE",&outP);
    ot->Branch("outP",&outE);
    ot->Branch("outAr",&out_Ar);
    ot->Branch("vtx_pr",&intpos_pr);
    ot->Branch("inX_pr",&inX_pr);
    ot->Branch("vV_pr",&vV_pr);
    ot->Branch("outE_pr",&outP_pr);
    ot->Branch("outP_pr",&outE_pr);
    ot->Branch("q2",&out_q2);
    ot->Branch("origin_name", &name_out);
    ot->Branch("total_pot",&out_pot);
  }

  cout << "output file name: " << outf << endl; 
  for(int i = 0; i < intree->GetEntries(); ++i) {
    intree->GetEntry(i);
    if(ene <= mX+mV) continue;
    double thisW = impwt * xsec->Eval(ene) * len;
    if(r->Uniform() < thisW/maxW) {
      TVector3 xmom(px,py,pz);
      cout << "Len and L1: " << len << " " << L1 << endl;
      cout << "Origin: " << vx << " " << vy << " " << vz << endl;
      cout << "Momentum before scattering: " << px << " " << py << " " << pz << " " << ene << endl;
      cout << "Total POT: " << pot_final << endl; 
      cout << "\n" << endl;
      double lpos = r->Uniform() * len + L1;
      TVector3 orig(vx,vy,vz);
      const TVector3& dir = xmom.Unit();
      TVector3 vtx = orig + lpos * dir;
      TLorentzVector dgam, epos, eneg;
      TLorentzVector dgam_pr, epos_pr, eneg_pr, vX_pr, vertex_pr;
      generate_interaction(ene, xmom, dgam, epos, eneg, cachedir);

      cout <<"Transferred momentum to Ar q2: " << q2_out << endl;

      vt *= 1e9; // time is in s, needs to be in ns

      if(root_option) {
        TLorentzVector vX(xmom.X(),xmom.Y(),xmom.Z(),ene);
        TLorentzVector vertex(vtx.X(),vtx.Y(),vtx.Z(),vt);
        vertex_pr = vertex;

        // Stuff for outgoing 4-momentum scattering
        out_Ar = pf[2];
        inX_pr = pf[1];

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

      // out
      out_Ar *= det_rot;
      inX_pr *= det_rot;
      
      cout << ievt << " " << 4 << " " << *name << " " << origin_id << endl;
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

     cout << "\n" << endl;

     ievt++;

    if(root_option) {
        TLorentzVector vX(xmom.X(),xmom.Y(),xmom.Z(),ene);
        TLorentzVector O(orig.X(),orig.Y(),orig.Z(), vt);
        TLorentzVector vertex(vtx.X(),vtx.Y(),vtx.Z(),ivt);
        origin = O; 
        weight = thisW; 
        intpos = vertex;
        inX = vX;
        vV = dgam;
        outE = eneg;
        outP = epos;
        intpos_pr = vertex_pr;
        //inX_pr = vX_pr;
        vV_pr = dgam_pr;
        outE_pr = eneg_pr;
        outP_pr = epos_pr;
        name_out = name; 
        out_q2 = q2_out;
        org_id = origin_id;
        out_pot = pot_final; 
        ot->Fill();
      }
    }
  }


   if(root_option) {
    of->cd();
    cout << "Final number of events: " << ievt << endl;
    DumpHepevt(ot, outf);
    ot->Write();
    of->Close();
  }
  


}
