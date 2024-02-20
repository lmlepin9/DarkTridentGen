/***********************************************************************
                TEventMaker2toN                                 
                                                                           
    Generate event and weight of phase space for process 2 -> 2 + M, M->3+4+...+N                                            

Authors:
	
	Radoslaw Kycia, Jacek Trunau

Warning:
	It is modified version which selects two solutions randomly.

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

#ifndef TEventMaker2toN_H
#define TEventMaker2toN_H


/*!
  @class TEventMaker2toN

  @brief Generate event and weight of phase space for process 2 -> 2 + M, M->3+4+...+N                                            
		


 */

#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <assert.h>
#include <complex>
#include <queue>          

#include <TSystem.h>
#include <TROOT.h>
#include <TRandom.h>
#include <TDatabasePDG.h>
#include <TApplication.h>
#include <TSystem.h>
#include <TFile.h>
#include <TObjArray.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TF1.h>
#include <TAxis.h>
#include <TLine.h>
#include <TMath.h>
#include <TLorentzVector.h>
#include <TPostScript.h>

#include "TParticlePDG.h"
#include "TDatabasePDG.h"


#include "TDecay.h"


using namespace std;

////////////////////////////////////////////////////////////////////////
//#define DEBUG
#undef DEBUG
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class TEventMaker2toN
{

private:
    
    /// Pi = 3.14...
    static const double PI;

	/// Particle Database PDG
	TDatabasePDG * PDGDatabese;

    /// CalculateKinematics solves energy-momentum conservation equations
	int CalculateKinematics(double p1t, double p2t, double dphi1, double dphi2, double y, double vmass2, int isol);
	
	///calculate 2-particle momentum in their cma frame
	double kcms(double s, double m1, double m2);
   
	///Check if generation failed
    bool generationFailed;
    
    ///Determines type of reaction
    int weightStrategy;
   
    /// number of all particles
    int nopf;  
    
    /// final particles four-vectors
    TLorentzVector* pf;
    
    ///central mass four-vector
    TLorentzVector PM;
    
    /// beam 1 && 2 fourvectors 
    TLorentzVector* pb;
    
    /// number of outgoing particles (2 protons + ( nop-2 ) pions)   	       
    int nop;
    /// store masses of particles corresponding to pcm properties;
    double * mass;
    
    ///PDG id of outgouing particles
    int * idOut;   
    
	///PDG id of particle A
	int idA;
	///PDG id of particle B
	int idB;
	///Energy of particle A in LAB frame
	double EA;
	///Energy of particle B in LAB frame
	double EB;
	///Total energy in CM
	double tecm;
	///Mass of A
	double mA;
	///Mass of B
	double mB;
	///4-momentum of A
	TLorentzVector pA;
	///4-momentum of B
	TLorentzVector pB;	
    
    ///min value of particle 1,2 transverse momenta
    double p_min;
    ///max value of particle 1,2 transverse momenta
    double p_max;
    ///min value of CM rapidity
    double y_min;
    ///max value of CM rapidity
    double y_max;
    
	///CM mass min and max range 
	double mass_min,mass2_min,mass_max, mass2_max;
	
	///weight for decay
	double wtdecay;
	
	///weight
	double weight;
	
    inline double sq(double a){double temp=pow(a,2);return temp;}
    
    ///queue for random numbers
    queue<double> rndQueue;
    
    ///decay central mass
    TDecay * Decay;
    
public:
 
    ///Constructor
    TEventMaker2toN(double tecm, double p_min, double p_max, double y_min, double y_max, int nop, double mass_min, double mass_max, double* mass, int* idIn, int* idOut, TLorentzVector* pb, TLorentzVector* pf);                    
    
    ///Destructor
    virtual ~TEventMaker2toN(void);           
  
	/// Generate one event in Cylindrical Phase Space starting from 
	/// @param Xarg vector 
	/// @param nDim dimension of @ref Xarg
	double SetEvent(int nDim, double *Xarg);

	/// @returns weight of the last event
	double getWeight( void );
	
	/// @returns weight of the decay
	double getDecayWeight( void );
	
	/// flag for generation fail
	bool isGenerationFailed( void );
	
};
#endif
