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

#include <TMath.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TGraphErrors.h>
#include <TRandom3.h>
#include <TFoam.h>
#include <TFoamIntegrand.h>
#include <TLorentzVector.h>
#include <TDatabasePDG.h>

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
double mX;
double mV;
double mass_ratio; 
double q2_out;
std::string dm_type;

const double ee = TMath::Sqrt(4.*TMath::Pi()*alpha);
const double gp = 5e-4;//TMath::Sqrt(4.*TMath::Pi()*alpha_D);
const double gc11 = gp; 
const double gc8 = 18.*ee*eps; // 18*ee*eps


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
  const double FF = 1;//3. * std::sph_bessel(1, q*R)/(q*R)*TMath::Exp(-q2*s*s/2.); // Spherical Bessel function 

  using TMath::Power;
  double m2; 

  // According to Mathematica's results, these are the right matrix elements (2->3 scattering). 
  // We add an extra factor of 1/2. to account for the spin average 
  if(dm_type == "fermion"){
      m2=FF*FF*(1/2.)*(
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
    m2 = FF*FF*(-4*std::pow(gc11,4)*std::pow(gc8,2))*
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


	//set up ingoing particles
   
	double mB = 40; 
    double eA = ene;
    double mA = mX;
  
	//setup CM frame
		
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
		//weight *= 1.0 / (2.0* pbCM.M2()); //flux
		weight *= 1.0/(4.0*F);
		weight *= 1.0; //for now |ME|^2 is unity
        weight *= mel2(pb[1],pb[2],pf[1],pf[2],pf[3]);
	
		//convert GeV^-2 to mb if matrix element is in GeV^-2(natural units) - uncomment if needed
		weight *= 0.3894;

		
		//setup event weight
		eventWeight = weight;
	return( weight );

};

////////////////////////////////////////////////////////////////////////
//MAIN
////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
		
	//inegral, its error and mc weight
	Double_t MCresult,MCerror,MCwt;
    double ene;
		
    char c;

	std::cout << "Read parameters" << std::endl;

    while((c = getopt(argc, argv, "e:m:r:t:")) != -1) {
        switch(c) {
            case 'e':
                std::istringstream(optarg)>>ene;
			case 'm':
        		std::istringstream(optarg)>>mV;
			
			case 'r':
				std::istringstream(optarg)>>mass_ratio; 

            case 't':
                std::istringstream(optarg)>>dm_type;
            default:
              break;
          }
        }

	mX = mass_ratio * mV;

	std::cout << "mA, mChi : " << mV << ", " << mX << std::endl;
    std::cout << "DM TYPE : " << dm_type << std::endl;
    std::cout << "Energy: " << ene << std::endl;




	double mass[Ndp] = {0.000511,0.000511};
    double edp;
    double eX, xsec,xerr;
	int countg = 0;
	int idx_ene = 2.;


    if(ene < mX+mV || ene > 10.) {
        	std::cerr << "ene " << ene << " out of good range [" <<mX+mV<<",10]"<<std::endl;
        	return -1;
    	}

	std::cout << "Energy: " << ene <<  " (range: [" << mV + mX << ", 10])" << std::endl;

		//Setup Foam - according to ROOT Foam tutorial
		//=========================================================
		long NevTot   =     100000;   // Total MC statistics
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
		TFoam   *FoamX    = new TFoam("FoamX");   // Create Simulator
		TDensity    *rho= new TDensity(ene);   //Create density
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
	
		//cout<<" About to start MC loop: ";  cin.getline(question,20);
		Double_t *MCvect = new Double_t[kDim]; // vector generated in the MC run
	
		TGenPhaseSpace *_decay = new TGenPhaseSpace();
    
		//integrand
		double integrand = 0.0;
	
		//GENRATION LOOP
		long   loop;
		for(loop=0; loop<NevTot; loop++)
		{
			// generate MC event
			FoamX->MakeEvent();           
			FoamX->GetMCvect( MCvect);
			FoamX->GetMCwt( MCwt );
			//update integrad
			integrand += rho->getEventWeight();
        }
		
		//cross check for integal
		cout << "Integral = " << integrand / (double) loop << endl;
		cout << "====== Events generated, entering Finalize" << endl;
		Double_t eps = 0.0005;
		Double_t Effic, WtMax, AveWt, Sigma;
		Double_t IntNorm, Errel;
		FoamX->Finalize( IntNorm, Errel);     // final printout
		FoamX->GetIntegMC( MCresult, MCerror);  // get MC intnegral
		FoamX->GetWtParams(eps, AveWt, WtMax, Sigma); // get MC wt parameters
		Effic=0; if(WtMax>0) Effic=AveWt/WtMax;

		xsec=MCresult;
		xerr=MCerror;
		eX=ene;
		++countg;

    std::cout <<"XSEC VAL: " << (integrand / (double) loop)*1e9 << std::endl;
	std::cout <<"XSEC VAL: " << xsec*1e9 << std::endl;
	cout << "***** End of Demonstration Program  *****" << endl;
	return 0;
	
};
