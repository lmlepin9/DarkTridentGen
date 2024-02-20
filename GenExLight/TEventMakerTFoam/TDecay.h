/***********************************************************************

 Adopted from TGenPhaseSpace Root package with the following modifications
 
1. Generate() takes random numbers from the queue, not from gRandom.
2. Weight is equal to Lorentz invariant phase space factor corresponding to 
\f$<p'|p>=(2\pi)^3*2E*\delta(p-p')\f$ state normalization
H.Pilkhun, The Interactions of Hadrons North-Holland 1967,p.16
3. Result is multiplied by additional 1/(2*PI) factor.

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

#ifndef ROOT_TDecay
#define ROOT_TDecay

/*!
  @class TDecay

  @brief  Adopted from TGenPhaseSpace Root package with the following modifications
 
	1. Generate() takes random numbers from the queue, not from gRandom.
	2. Weight is equal to Lorentz invariant phase space factor corresponding to \f$<p'|p>=(2\pi)^3*2E*\delta(p-p')\f$ state normalization
		H.Pilkhun, The Interactions of Hadrons North-Holland 1967,p.16
	3. Result is multiplied by additional 1/(2*PI) factor.

*/


#include "TLorentzVector.h"


#include <iostream>       // std::cin, std::cout
#include <queue>          // std::queue
#include <assert.h>
using namespace std;


class TDecay : public TObject {
private:  
   Int_t        fNt;             // number of decay particles
   Double_t     fMass[18];       // masses of particles
   Double_t     fBeta[3];        // betas of decaying particle
   Double_t     fTeCmTm;         // total energy in the C.M. minus the total mass
   Double_t     fWtMax;          // maximum weigth 
   TLorentzVector  fDecPro[18];  //kinematics of the generated particles 

   Double_t PDK(Double_t a, Double_t b, Double_t c);
   
   ///factorial function
   int factorial(int n);  

public:
   TDecay(): fNt(0), fMass(), fBeta(), fTeCmTm(0.), fWtMax(0.) {}
   TDecay(const TDecay &gen);
   virtual ~TDecay() {}
   TDecay& operator=(const TDecay &gen);

   Bool_t          SetDecay(TLorentzVector &P, Int_t nt, const Double_t *mass);
   Double_t        Generate(std::queue<double> &rnd);
   TLorentzVector *GetDecay(Int_t n); 

   Int_t    GetNt()      const { return fNt;}
   Double_t GetWtMax()   const { return fWtMax;}

};

#endif

