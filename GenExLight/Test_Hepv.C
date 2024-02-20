#include <TMath.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TChain.h>
#include <TRandom3.h>
#include "TSystem.h"
#include <TFoam.h>
#include <TFoamIntegrand.h>
#include <TLorentzVector.h>
#include <TDatabasePDG.h>
#include <TGraph.h>
#include "TTreeReader.h"
#include "TTreeReaderValue.h"


void DumpHepevt(TTree *out_tree){
  // Here we dump all the information in to a hepevt file, including POT per event. 
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
  //TLorentzVector *xmom, *dgam, *eneg, *epos, *vtx, *orig;
  //TLorentzVector *inX_pr, *vV_pr, *outE_pr, *outP_pr, *intpos_pr;
  Double_t out_q2, w;
  Int_t origin_id; 
  std::string *name = 0;
  std::string file ="hepevt_dump";

  //ot.SetWeight(interaction_weight);
  out_tree->SetBranchAddress("evt_weight",&w);
  out_tree->SetBranchAddress("vtx",&vtx);
  out_tree->SetBranchAddress("inX",&xmom);
  out_tree->SetBranchAddress("origin",&orig);
  out_tree->SetBranchAddress("origin_id",&origin_id);
  out_tree->SetBranchAddress("vV",&dgam);
  out_tree->SetBranchAddress("outE",&eneg);
  out_tree->SetBranchAddress("outP",&epos);
  out_tree->SetBranchAddress("vtx_pr",&intpos_pr);
  out_tree->SetBranchAddress("inX_pr",&inX_pr);
  out_tree->SetBranchAddress("vV_pr",&vV_pr);
  out_tree->SetBranchAddress("outE_pr",&outP_pr);
  out_tree->SetBranchAddress("outP_pr",&outE_pr);
  out_tree->SetBranchAddress("q2",&out_q2);
  out_tree->SetBranchAddress("origin_name",&name);
  //ot.Branch("pot",&out_pot);

  ofstream outputFile;
  cout << "Writing hepevt file: " << file + ".txt" << endl; 
  outputFile.open((file+".txt").c_str());
  Int_t n_entries = (Int_t) out_tree->GetEntries();
  cout << "Number of entries: " << n_entries << endl; 

  for(Int_t ievt = 0; ievt < n_entries; ievt++){
      out_tree->GetEntry(ievt);
      outputFile << ievt << " " << 4 << " " << *name << " " << origin_id << endl;
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