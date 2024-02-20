/***********************************************************************

Description:
  
	Toy MC generator that using modified TGenPhaseSpace and N-particle phase space
	with two leading particles

Warning:
 	
	It is modified version with extended phase space dimension - select randomly two solutions bouncing and unbouncing.

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

#include <iostream>
#include <assert.h>
#include <cmath>
#include <complex>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <queue> 

#include <TMath.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TRandom3.h>
#include <TFoam.h>
#include <TFoamIntegrand.h>
#include <TLorentzVector.h>
#include <TDatabasePDG.h>

#include "TDecay.h"
#include "TEventMaker2toN.h"



using namespace std;


TDatabasePDG * PDGDatabese = TDatabasePDG::Instance();



////////////////////////////////////////////////////////////////////////
//GLOBAL VARIABLES
////////////////////////////////////////////////////////////////////////

//energy of collision at the center
const double tecm = 200.0;  //GeV
//pt of leading particles 1 and 2:
const double p_min = 0.0;   //GeV
const double p_max = 1.0;   //GeV
//rpaidity of central blob:
const double y_min = -10.0;
const double y_max = 10.0;
// invariant mas of central blob:
const double mass_min = 0.280; //GeV
const double mass_max = 100.0;  //GeV


//incoming particles
const int Nip = 2; 
//outgoing particles
const int Nop = 4;

//pdg codes of particles starting from index 1
int idIn[Nip+1] = {0, 2212,2212};		// p+p
int idOut[Nop+1] = {0, 2212,2212,211,-211}; //p+p+pi^{+}+pi^{-}


//global 4vectors
//in/beam particles 
TLorentzVector pb[Nip+1];
//out particles
TLorentzVector pf[Nop+1];


////////////////////////////////////////////////////////////////////////
//TDensity DEFINITION
////////////////////////////////////////////////////////////////////////

///Class density for Foam
class TDensity: public TFoamIntegrand 
{
private:	
	
	///Decay
	TEventMaker2toN * _decay;
	
	///particles
	TLorentzVector* pb;
	TLorentzVector* pf;
	
	
public:
	
	/// Constructor - sets up generation range
	TDensity(double tecm, double p_min, double p_max, double y_min, double y_max, int nop, double mass_min, double mass_max, double* mass, int* idIn, int* idOut, TLorentzVector* pb, TLorentzVector* pf);
	
	/// Destructor
	virtual ~TDensity(){ delete _decay; };
	
	/// @returns weight of the process
	/// @param nDim  dimension of integration
	/// @param Xarg vector of probablilistic variables from [0;1] of dim nDim
	/// @warning it is required by Foam integrator
	Double_t Density(int nDim, Double_t *Xarg);

	
};

////////////////////////////////////////////////////////////////////////
//TDensity IMPLEMENTATION
////////////////////////////////////////////////////////////////////////

TDensity::TDensity(double tecm, double p_min, double p_max, double y_min, double y_max, int nop, double mass_min, double mass_max, double* mass, int* idIn, int* idOut, TLorentzVector* pb, TLorentzVector* pf)
{ 
	
	_decay = new TEventMaker2toN( tecm, p_min, p_max, y_min, y_max, nop, mass_min, mass_max, mass, idIn, idOut, pb, pf);
	
	this->pb = pb;
	this->pf = pf;
	
};

////////////////////////////////////////////////////////////////////////
Double_t TDensity::Density(int nDim, Double_t *Xarg)
{	
	
		double  wtdecay = _decay->SetEvent( nDim, Xarg );
	
		//nullify failed generation
		if( _decay->isGenerationFailed() )
		{
				wtdecay = 0.0;
		}
		
		
		//CUT
			const double ptcut = 1.0;
		
			if( ( pf[1].Pt() > ptcut ) || ( pf[2].Pt() > ptcut ) )
				wtdecay = 0.0;
		
			
			TLorentzVector pfCM = pf[3]+pf[4];
			
			const double ycut = 10.0;
			if( (abs( pfCM.Rapidity() ) > ycut) )
				wtdecay = 0.0;
		
			const double Mmin = 0.280;
			const double Mmax = 100.0;
			
			if( ( pfCM.M() > Mmax ) || ( pfCM.M() < Mmin ) )
				wtdecay = 0.0;

		/*
			if( ( pf[1].Pz() * pb[1].Pz() < 0.0 ) && ( pf[2].Pz() * pb[2].Pz() < 0.0 ) )
				weight =0.0;
		*/

		
		
		//|Matrix Element|^2
			double weight = wtdecay;
			
			
			weight *= 1.0; //for now |ME|^2 is unity
	
		//convert GeV^-2 to mb if matrix element is in GeV^-2(natural units)
			//weight *= 0.3894;

	
	return( weight );
	

};



////////////////////////////////////////////////////////////////////////
//MAIN
////////////////////////////////////////////////////////////////////////

int main()
{
	
	
	//set up masses of outgoing particles
	double mass[ Nop+1 ];
	
	//initialize masses from PDG table
	for(int i=1; i < Nop; i++)
			mass[i] = PDGDatabese->GetParticle( idOut[i] )->Mass();
	
		
	//inegral, its error and mc weight
	Double_t MCresult,MCerror,MCwt;
		
	
	//Setup Foam - according to ROOT Foam tutorial
	//=========================================================
	long NevTot   =     100000;   // Total MC statistics
	Int_t  kDim   =     3*Nop - 4 + 1;   // total dimension (+1) - for selecting solution of kinematics randomly
	Int_t  nCells   =     10000;   // Number of Cells
	Int_t  nSampl   =     10000;   // Number of MC events per cell in build-up
	Int_t  nBin     =       8;   // Number of bins in build-up
	Int_t  OptRej   =       1;   // Wted events for OptRej=0; wt=1 for OptRej=1 (default)
	Int_t  OptDrive =       2;   // (D=2) Option, type of Drive =0,1,2 for TrueVol,Sigma,WtMax
	Int_t  EvPerBin =      25;   // Maximum events (equiv.) per bin in buid-up
	Int_t  Chat     =       1;   // Chat level
	//=========================================================
	TRandom *PseRan   = new TRandom3();  // Create random number generator
	TFoam   *FoamX    = new TFoam("FoamX");   // Create Simulator
	
	TDensity    *rho= new TDensity(tecm, p_min, p_max, y_min, y_max, Nop, mass_min, mass_max, mass, idIn, idOut, pb, pf);   //Create density
	
	PseRan->SetSeed(4357);
	//=========================================================
	cout<<"*****   Demonstration Program for Foam version "<<FoamX->GetVersion()<<"    *****"<<endl;
	FoamX->SetkDim(        kDim);      // Mandatory!!!
	FoamX->SetnCells(      nCells);    // optional
	FoamX->SetnSampl(      nSampl);    // optional
	FoamX->SetnBin(        nBin);      // optional
	FoamX->SetOptRej(      OptRej);    // optional
	FoamX->SetOptDrive(    OptDrive);  // optional
	FoamX->SetEvPerBin(    EvPerBin);  // optional
	FoamX->SetChat(        Chat);      // optional
	//===============================
	FoamX->SetRho(rho);
	FoamX->SetPseRan(PseRan);
	
	// Initialize simulator
		FoamX->Initialize(); 
	
	long nCalls=FoamX->GetnCalls();
	cout << "====== Initialization done, entering MC loop" << endl;
	
	Double_t *MCvect = new Double_t[kDim]; // vector generated in the MC run

	//PROLOG USER SECTION (definitions etc.):	

	//Histograms
		TH1D * h_rapidity = new TH1D("h_rapidity", "h_rapidity; y; events",100,0.0,0.0);
		h_rapidity->SetBit(TH1::kCanRebin);
		h_rapidity->Sumw2();
	
		TH1D * h_CM = new TH1D("h_CM", "h_CM; CM[GeV]; events",100,0.0,tecm);
		h_CM->SetBit(TH1::kCanRebin);
		h_CM->Sumw2();
	
		TH2D * h_etaphi = new TH2D("h_etaphi", "h_etaphi; y; #phi",1000,-10.0,10.0,1000,-M_PI,M_PI);
		h_etaphi->Sumw2();
	
	
	//Prepare tree for events
		TTree * tree = new TTree("events","Event tree");
			
		//integrand value
		tree->Branch( "eventIntegrandValue", & MCwt, "eventIntegrandValue/D" );
	
		string basename("");
		string number(""); 
	
		//in particles
		basename = "In particle ";
	
		for( int i = 1; i < Nip + 1; i++ )
		{
			number = "";		
			number = static_cast<ostringstream*>( &(ostringstream() << i) )->str();
		
			tree->Branch( ( basename + number ).c_str() ,"TLorentzVector", & pb[i] );
		}
	
		//out particles
		basename = "Out particle ";
	
		for( int i = 1; i < Nop + 1; i++ )
		{
			number = "";		
			number = static_cast<ostringstream*>( &(ostringstream() << i) )->str();
		
			tree->Branch( ( basename + number ).c_str() ,"TLorentzVector", &(pf[i]) );
		}
	
	
	//END OF PROLOG USER SECTION
	
	
	//GENRATION LOOP
	long   loop;
	for(loop=0; loop<NevTot; loop++)
	{
		// generate MC event
			FoamX->MakeEvent();           
			FoamX->GetMCvect( MCvect);
			FoamX->GetMCwt( MCwt );
    
			
		//filling histograms
			h_rapidity->Fill( pf[1].Rapidity(), MCwt);
			h_rapidity->Fill( pf[2].Rapidity(), MCwt);
			
			TLorentzVector pCM;
			
			for( int i = 3; i < Nop+1; i++)
			{
				pCM += pf[i];
			}
		
			h_CM->Fill( pCM.M(), MCwt);
			
			for( int i = 1; i < Nop+1; i++)
			{
				h_etaphi->Fill( pf[i].Rapidity(), pf[i].Phi(), MCwt);
			}
		
		//save events in tree
			tree->Fill();
		
		
		//progress info
		if( loop % 1000 == 0)
			cout << "loop = " << loop << endl;
		
		
	}
	
	//END GENRATION LOOP

	cout << "====== Events generated, entering Finalize" << endl;

	Double_t eps = 0.0005;
	Double_t Effic, WtMax, AveWt, Sigma;
	Double_t IntNorm, Errel;
	FoamX->Finalize(   IntNorm, Errel);     // final printout
	FoamX->GetIntegMC( MCresult, MCerror);  // get MC intnegral
	FoamX->GetWtParams(eps, AveWt, WtMax, Sigma); // get MC wt parameters
	Effic=0; if(WtMax>0) Effic=AveWt/WtMax;
	cout << "================================================================" << endl;
	cout << " MCresult= " << MCresult << " +- " << MCerror << " RelErr= "<< MCerror/MCresult << endl;
	cout << " Dispersion/<wt>= " << Sigma/AveWt << endl;
	cout << "      <wt>/WtMax= " << Effic <<",    for epsilon = "<<eps << endl;
	cout << " nCalls (initialization only) =   " << nCalls << endl;
	cout << "================================================================" << endl;

	delete [] MCvect;
	
	
	//EPILOG USER SECTION (writing files etc.)
	
	//Saving histograms - root file
		TFile RootFile("histograms.root","RECREATE","histograms");
		RootFile.ls();
		h_rapidity->Write();
		h_CM->Write();
		h_etaphi->Write();
		RootFile.Close();
	
	
	//Saving histograms - pdf file
	
		
	//helper function/closure that set up and save TH1D histograms
	struct {
        void operator() ( TH1D * hist, string filename ) const 
        {
			//setup histograms
				
				/*
				TH1D * h_Hist = hist;
				h_Hist->SetStats(0);
				h_Hist->SetTitle(0);
				h_Hist->GetYaxis()->CenterTitle();
				h_Hist->GetYaxis()->SetTitleSize(0.05);
				h_Hist->GetYaxis()->SetTitle("#sigma_{B}^{conv}(E,#delta)/#sigma_{B}(E) #left( #sigma_{(c)}^{conv}(E,#delta)/#sigma_{(c)}^{conv}(E,0) #right)^{-1}");
				h_Hist->GetYaxis()->SetLabelSize(0.05);
				h_Hist->GetXaxis()->CenterTitle();
				h_Hist->GetXaxis()->SetTitleSize(0.05);
				h_Hist->GetXaxis()->SetTitle("#delta [MeV]");
				h_Hist->GetXaxis()->SetLabelSize(0.05);
			
				h_Hist->GetYaxis()->SetRangeUser(0., 2.);
				*/
				
			//smooth histograms
				//hist->Smooth(5);
		
				
			//save
				TCanvas* canv1 = new TCanvas("canv","plot");
				//canv1->SetGrid(bGrid);
	
				canv1->cd(1);
				hist->Draw("h");
				//hist->Draw("L");
				canv1->Update();
	
				canv1->Print( filename.c_str(), "" );

				delete canv1;
			
        }
    } saveTH1DHistograms;
	
	
	//helper function/closure that set up and save TH2D histograms
	struct {
        void operator() ( TH2D * hist, string filename ) const 
        {
			//setup histograms
				
				/*
				TH2D * h_Hist = hist;
				h_Hist->SetStats(0);
				h_Hist->SetTitle(0);
				h_Hist->GetYaxis()->CenterTitle();
				h_Hist->GetYaxis()->SetTitleSize(0.05);
				h_Hist->GetYaxis()->SetTitle("#sigma_{B}^{conv}(E,#delta)/#sigma_{B}(E) #left( #sigma_{(c)}^{conv}(E,#delta)/#sigma_{(c)}^{conv}(E,0) #right)^{-1}");
				h_Hist->GetYaxis()->SetLabelSize(0.05);
				h_Hist->GetXaxis()->CenterTitle();
				h_Hist->GetXaxis()->SetTitleSize(0.05);
				h_Hist->GetXaxis()->SetTitle("#delta [MeV]");
				h_Hist->GetXaxis()->SetLabelSize(0.05);
			
				h_Hist->GetYaxis()->SetRangeUser(0., 2.);
				*/
				
			//smooth histograms
				//hist->Smooth(5);
		
				
			//save
				TCanvas* canv1 = new TCanvas("canv","plot");
				//canv1->SetGrid(bGrid);
	
				canv1->cd(1);
				hist->Draw("h");
				//hist->Draw("L");
				canv1->Update();
	
				canv1->Print( filename.c_str(), "" );

				delete canv1;
			
        }
    } saveTH2DHistograms;
	
	
	//save histograms
		saveTH1DHistograms( h_rapidity, string("rapidity.eps") );
		saveTH1DHistograms( h_CM, string("CM.eps") );
		saveTH2DHistograms( h_etaphi, string("etaPhi.eps") );
	
	//save tree
		string rootFilename("events.root");
		
		TFile file( rootFilename.c_str(), "Update" ); 
		file.SetCompressionLevel(1); 
		file.cd();
	
		//tree->Write("",TObject::kOverwrite);  //uncomment if want to write events with overwrite
		tree->Write();  //write events with replace

		file.Close();
	
	//END EPILOG SECTION
	
	//CLEANING
		delete h_rapidity;
		delete h_CM;
		delete h_etaphi;
		delete FoamX;
		delete rho;
		delete tree;
	
	
	cout << "***** End of Demonstration Program  *****" << endl;


	return 0;
	
};
