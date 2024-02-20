/***********************************************************************

 Adopted from TGenPhaseSpace Root package with the following modifications
 
1. Generate() takes random numbers from the queue, not from gRandom.
2. Weight is equal to Lorentz invariant phase space factor corresponding to 
\f$<p'|p>=(2\pi)^3*2E*\delta(p-p')\f$ state normalization
H.Pilkhun, The Interactions of Hadrons North-Holland 1967,p.16
3. Result is multiplied by additional 1/PI factor.

***********************************************************************/   

#ifndef ROOT_TDecay
#define ROOT_TDecay

/*!
  @class TDecay

  @brief  Adopted from TGenPhaseSpace Root package with the following modifications
 
	1. Generate() takes random numbers from the queue, not from gRandom.
	2. Weight is equal to Lorentz invariant phase space factor corresponding to \f$<p'|p>=(2\pi)^3*2E*\delta(p-p')\f$ state normalization
		H.Pilkhun, The Interactions of Hadrons North-Holland 1967,p.16
	3. Result is multiplied by additional 1/PI factor.

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
   
	///Check if array of dim N is ordered
	/// @param array - array to be checked
	/// @param N - number of elements in array
	/// @returns boolean value that determine if table is ordered
	bool isOrdered(const double array[], int N);
   
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
   
   
   ///flag failed
   bool ordered;

};

#endif

