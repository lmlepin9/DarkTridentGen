/***********************************************************************

Descritpion:
 
	Toy MC generator that using modified TGenPhaseSpace

Warning:
 	
	It makes spherical decay and is unsiutable for strong cuts, e.g., 
	on p_{t} of some particles.

	It is only proof of concept - not suitable for production. Use FOAM version instead.

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


using namespace std;


TDatabasePDG * PDGDatabese = TDatabasePDG::Instance();



////////////////////////////////////////////////////////////////////////
//GLOBAL VARIABLES
////////////////////////////////////////////////////////////////////////


//global variables
const double tecm = 200.0;


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
	
	///decay object
	TDecay * _decay;
	
	///queue for random numbers
    queue<double> rndQueue;
    
    ///weight of the event
    Double_t eventWeight;
	
public:
	
	/// Constructor
	TDensity(){ _decay = new TDecay(); };
	
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
	
	pb[1].SetXYZT( 0.0, 0.0, 0.0, PDGDatabese->GetParticle( idIn[1] )->Mass() );

	//set up ingoing particles
	int idA = idIn[1];
	int idB = idIn[2];
   
   	double mA = PDGDatabese->GetParticle(idA)->Mass();
	double mB = PDGDatabese->GetParticle(idB)->Mass(); 
  
	//setup CM frame
	double EA = (pow(mA,2) - pow(mB,2) + pow(tecm,2))/(2.0 * tecm);
	double EB = (-pow(mA,2) + pow(mB,2) + pow(tecm,2))/(2.0 *tecm);
		
	double pzA = sqrt( EA*EA - mA*mA );
	double pzB = -sqrt( EB*EB - mB*mB );
		
	pb[1].SetPxPyPzE( 0.0, 0.0, pzA, EA );
	pb[2].SetPxPyPzE( 0.0, 0.0, pzB, EB );

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
			double mass[Nop];
	
			for(int i=0; i < Nop; i++)
				mass[i] = PDGDatabese->GetParticle( idOut[i+1] )->Mass();
 
			//set decay configuration
			_decay->SetDecay(pbCM, Nop, mass);
		
			//make decay
			double wtdecay =  2 * M_PI * _decay->Generate( rndQueue );
		
			//get out particles
			for( int i = 1; i < Nop+1; i++ )
			{
				pf[i] = *(_decay->GetDecay( i-1 ));
			}
		
	
		
		//|Matrix Element}^2
			double weight = wtdecay;
			weight *= 1.0 / (2.0* pbCM.M2()); //flux
			weight *= 1.0; //for now |ME|^2 is unity
	
		//convert GeV^-2 to mb if matrix element is in GeV^-2(natural units) - uncomment if needed
			//weight *= 0.3894;
	
		//CUT
			const double ptcut = 15.0;
		
			if( ( pf[1].Pt() > ptcut ) || ( pf[2].Pt() > ptcut ) )
				weight = 0.0;
		
			
			TLorentzVector pfCM = pf[3]+pf[4];
			
			const double ycut = 10.0;
			if( (abs( pfCM.Rapidity() ) > ycut) )
				weight = 0.0;
		
			const double Mmin = 0.280;
			const double Mmax = 100.0;
			
			if( ( pfCM.M() > Mmax ) || ( pfCM.M() < Mmin ) )
				weight = 0.0;
		/*
			if( ( pf[1].Pz() * pb[1].Pz() < 0.0 ) && ( pf[2].Pz() * pb[2].Pz() < 0.0 ) )
				weight =0.0;
		*/
		
		//setup event weight
		eventWeight = weight;

	
	return( weight );

};



////////////////////////////////////////////////////////////////////////
//MAIN
////////////////////////////////////////////////////////////////////////


int main()
{
	

	TRandom *PseRan   = new TRandom3();  // Create random number generator

	//weight function
	TDensity * density = new TDensity();

	long NevTot   =  10000;   // Total MC statistics
	int nDim = 3*Nop-4;
	Double_t Xarg[ nDim ];
	
	//weight of the event
	double weight = 0.0;

	
	//set up ingoing particles
	int idA = idIn[1];
	int idB = idIn[2];
   
   	double mA = PDGDatabese->GetParticle(idA)->Mass();
	double mB = PDGDatabese->GetParticle(idB)->Mass(); 
  
	//setup CM frame
	double EA = (pow(mA,2) - pow(mB,2) + pow(tecm,2))/(2.0 * tecm);
	double EB = (-pow(mA,2) + pow(mB,2) + pow(tecm,2))/(2.0 *tecm);
		
	double pzA = sqrt( EA*EA - mA*mA );
	double pzB = -sqrt( EB*EB - mB*mB );
		
	pb[1].SetPxPyPzE( 0.0, 0.0, pzA, EA );
	pb[2].SetPxPyPzE( 0.0, 0.0, pzB, EB );

	//central mass 4-momentum
	TLorentzVector pbCM;
	
	pbCM = pb[1]+pb[2];

	//PROLOG USER SECTION (definitions etc.):
	
	//Histograms
		TH1D * h_rapidity = new TH1D("h_rapidity", "h_rapidity; y; events",100,-10.0,10.0);
		h_rapidity->SetBit(TH1::kCanRebin);
		h_rapidity->Sumw2();
	
		TH1D * h_CM = new TH1D("h_CM", "h_CM; CM[GeV]; events",100,0.0,200.0);
		h_CM->SetBit(TH1::kCanRebin);
		h_CM->Sumw2();
	
		TH2D * h_etaphi = new TH2D("h_etaphi", "h_etaphi; y; #phi",100,-10.0,10.0,100,-M_PI,M_PI);
		//h_etaphi->SetBit(TH1::kCanRebin);
		h_etaphi->Sumw2();
	
	
	//Prepare tree for events
		TTree * tree = new TTree("events","Event tree");
			
		//integrand value
		tree->Branch( "eventIntegrandValue", & weight, "eventIntegrandValue/D" );
	
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
	

	//accumulation value of integral/mean
	double integral = 0.0;
	
	//accumulation value of mans square
	double sqMena = 0.0;

	//EXPLORATION PROCEDURE


	//exploration maximal weight
	double maxWt = 0.0;
	
	//exploration samples
	long Nsamples = 100000000;

	//EXPLORATION LOOP
	for( int i = 0; i < Nsamples; i++ )
	{
		for( int i = 0; i < nDim; i++ )
		{
			Xarg[ i ] = PseRan->Rndm();
		}


		//make decay 
		double weight = density->Density( nDim, Xarg );
		
		
		//update max value
		if( weight > maxWt )
			maxWt = weight;
	
		
		if( i % 1000000 == 0 )
			cout << "exploration loop = " << i << endl;
		
		
	}


	cout << "maxWt = " << maxWt << endl;




	//long Nacc = 0;
	long Ntot = 0;


	
	//GENRATION LOOP
	long   loop;
	for(loop=0; loop<NevTot; loop++)
	{

	////DO REJECTION SAMPLING:
	
		long Nrej = 0;
	
		while( true )
		{
			//generate event:
				
			for( int i = 0; i < nDim; i++ )
			{
				Xarg[ i ] = PseRan->Rndm();
			}


			//make decay 
			double weight = density->Density( nDim, Xarg );


			
			Ntot ++;
			
			
			
			sqMena += pow( ( weight / maxWt ), 2 );
			
			//DO REJECTION:
			
			double randRej = PseRan->Rndm();
			
			if( weight < randRej * maxWt )
			{
				Nrej ++;
				continue;
			}
			
			
			if( weight < maxWt )
			{
					weight = 1.0;
					//cout << "wt = 1 event generated" << endl;
					//cout << "Nrej= " << Nrej << endl;
					break;
			}	
			
		}
		
		integral += weight;
		sqMena += weight * weight;

			
		//filling histograms
			h_rapidity->Fill( pf[1].Rapidity(), weight);
			h_rapidity->Fill( pf[2].Rapidity(), weight);
			
			TLorentzVector pCM;
			
			for( int i = 3; i < Nop+1; i++)
			{
				pCM += pf[i];
			}
		
			h_CM->Fill( pCM.M(), weight);
			
			for( int i = 1; i < Nop+1; i++)
			{
				h_etaphi->Fill( pf[i].Rapidity(), pf[i].Phi(), weight);
			}
			
			
		//save events in tree
			tree->Fill();
		
		if( loop % 10000 == 0)
			cout << "loop = " << loop << endl;
		
	}
	
	//END GENRATION LOOP	


	//inegral, its error and mc weight
	Double_t MCresult,MCerror;

	MCresult = maxWt * double( NevTot ) / double( Ntot );
	MCerror  = sqrt( sqMena * double( NevTot ) / double( Ntot ) - pow( maxWt * double( NevTot ) / double( Ntot ), 2 ) ) / double( Ntot );

	cout << "================================================================" << endl;
	cout << " MCresult= " << MCresult << " +- " << MCerror << " RelErr= "<< MCerror/MCresult << endl;
	cout << "================================================================" << endl;

	cout << "Ntot = " << Ntot << "; NevTot = " << NevTot << "; rejection effectivenes = " << double( NevTot ) / double( Ntot ) << endl;

	cout << "====== Events generated, entering Finalize" << endl;

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
	
		//tree->Write("",TObject::kOverwrite); //uncomment if want to write events with overwrite
		tree->Write();  //write events with replace

		file.Close();

	//END EPILOG SECTION	
	
	
	
	
	//CLEANING
		delete h_rapidity;
		delete h_CM;
		delete h_etaphi;
		delete PseRan;
		delete tree;
		delete density;
	
	
	cout << "***** End of Demonstration Program  *****" << endl;


	return 0;
	
};
