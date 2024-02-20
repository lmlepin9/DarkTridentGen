/***********************************************************************




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



TDecay *_decay = 0;
TGenPhaseSpace *_vdecay = 0;

void generate_interaction(const double ene, const TVector3& mom,
TLorentzVector& pV, TLorentzVector& pEp, TLorentzVector& pEm, const string& cachedir) {

    ///queue for random numbers
    queue<double> rndQueue;

    ///weight of the event
    Double_t eventWeight;
    Int_t  nDim   =  3*Nop-4;   // total dimension

    //set up ingoing particles
    double mB = 40.; 
    double eA = ene;
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

    TRandom3 *r_new = new TRandom3();
    //put rnd numbers into queue
    for( int i = 0; i < nDim; i++)
    {
        rndQueue.push(r_new->Uniform());
    }  

    //masses of products
    double mass_scatter[Nop] = {mA,mB,mV};

    //set decay configuration
    _decay->SetDecay(pbCM, Nop, mass_scatter);
    _decay->Generate(rndQueue );

    //get out particles
    for( int i = 1; i < Nop+1; i++ )
    {
        pf[i] = *(_decay->GetDecay( i-1 ));
    }


    q2_out = -((pf[1]-pb[1]).Mag2());

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
  // Initialize TDecay 
  _decay = new TDecay;

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
