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


#include"TEventMaker2toN.h"

////////////////////////////////////////////////////////////////////////



//Constants:
const double TEventMaker2toN::PI =  M_PI;



////////////////////////////////////////////////////////////////////////
TEventMaker2toN::TEventMaker2toN(double tecm, double p_min, double p_max, double y_min, double y_max, int nop, double mass_min, double mass_max, double* mass, int* idIn, int* idOut, TLorentzVector* pb, TLorentzVector* pf) 
{
 
	PDGDatabese = TDatabasePDG::Instance();
	
	this->tecm     = tecm;
	this->p_min    = p_min;
	this->p_max    = p_max;
	this->y_min    = y_min;
	this->y_max    = y_max;
	this->nop      = nop;
	this->mass_min = mass_min;
	this->mass_max = mass_max;
	this->pb = pb;
	this->pf = pf;
	
	//allocate tables
	this->mass  = new double [nop+1];
	this->idOut = new int[nop+1];
	//pf = new TLorentzVector [nop+1];
	
	//set up outgoing masses
	for( int i = 1; i < nop+1; i++ )
	{
		this->mass[i]  = mass[i];
		this->idOut[i] = idOut[i];
	}
	
	//set up ingoing particles
	this->idA = idIn[1];
	this->idB = idIn[2];
   
	Decay = new TDecay();
   
   	mA = PDGDatabese->GetParticle(idA)->Mass();
	mB = PDGDatabese->GetParticle(idB)->Mass(); 
  
	//setup CM frame
	EA = (pow(mA,2) - pow(mB,2) + pow(tecm,2))/(2.0 * tecm);
	EB = (-pow(mA,2) + pow(mB,2) + pow(tecm,2))/(2.0 *tecm);
		
	double pzA = sqrt( EA*EA - mA*mA );
	double pzB = -sqrt( EB*EB - mB*mB );
		
	pb[1].SetPxPyPzE( 0.0, 0.0, pzA, EA );
	pb[2].SetPxPyPzE( 0.0, 0.0, pzB, EB );
	
	//calculate mass^2 range of CM generation
	mass2_min=pow(mass_min,2);
	mass2_max=pow(mass_max,2);
	
  
};

////////////////////////////////////////////////////////////////////////
TEventMaker2toN::~TEventMaker2toN(void)
{
	
	delete [] mass;
	delete [] idOut;
   
	delete Decay;
   
};


////////////////////////////////////////////////////////////////////////
int TEventMaker2toN::CalculateKinematics(double p1t, double p2t, double dphi1, double dphi2, double y, double vmass2, int isol)
{
  TLorentzVector p1,p2,p3;

    //   four-momenta of the initial nucleons;
    //   ------------------------------------;
    double s=tecm*tecm;

    TVector2 pp1;
    pp1.SetMagPhi(p1t,dphi1);
    TVector2 pp2;
    pp2.SetMagPhi(p2t,dphi2);
    TVector2 pp3t = (pp1 + pp2);
    TVector2 pp3=pp3t.Rotate(PI);

    double am3t = sqrt( vmass2 + pp3.Mod2() );

    double p3_0 = am3t*cosh(y);
    double p3_z = am3t*sinh(y);
    if(p3_0>tecm) return 1;
    //set four momentum of intermediate state (or central particle)
    p3.SetXYZT(pp3.X(), pp3.Y(), p3_z, p3_0);


   // solving set of equations for p_{1z} and p_{2z};
   // a p1z^2 + b p1z + c = 0

    double Em = p3.E();
    double pmz = p3.Z();

	double m1 = mass[1];
	double m2 = mass[2];

    //Coefficients of the equation for p1z = x   in  ax^2+bx+c = 0:
    double a =-4*(pow(Em,2) - pow(pmz,2) - 2*Em*tecm + pow(tecm,2));

    double b = -4*pmz*(pow(Em,2) + pow(m1,2) - pow(m2,2) + pow(p1t,2) - pow(p2t,2) - pow(pmz,2) - 2*Em*tecm + pow(tecm,2));

    double c = pow(Em,4) + pow(m1,4) + pow(m2,4) - 2*pow(m2,2)*pow(p1t,2) + pow(p1t,4) + 2*pow(m2,2)*pow(p2t,2) - 2*pow(p1t,2)*pow(p2t,2) + pow(p2t,4) + 2*pow(m2,2)*pow(pmz,2) - 2*pow(p1t,2)*pow(pmz,2) + 2*pow(p2t,2)*pow(pmz,2) + pow(pmz,4) - 4*pow(Em,3)*tecm - 2*pow(m2,2)*pow(tecm,2) - 2*pow(p1t,2)*pow(tecm,2) - 2*pow(p2t,2)*pow(tecm,2) - 
		2*pow(pmz,2)*pow(tecm,2) + pow(tecm,4) - 2*pow(Em,2)*(pow(m1,2) + pow(m2,2) + pow(p1t,2) + pow(p2t,2) + pow(pmz,2) - 3*pow(tecm,2)) + 4*Em*tecm*(pow(m1,2) + pow(m2,2) + pow(p1t,2) + pow(p2t,2) + pow(pmz,2) - pow(tecm,2)) - 2*pow(m1,2)*(pow(m2,2) - pow(p1t,2) + pow(p2t,2) + pow(pmz,2) + pow(tecm,2));

    //Delta of the quadratic equation 
    double Delta = b*b-4.0*a*c;

	//fail of generation
    if( Delta < 0)
    {
    	return 2;
    };

    //two solutions
    double p1za = (-b + sqrt(Delta))/(2.0*a);
    double p1zb = (-b - sqrt(Delta))/(2.0*a);

    double pp1z[2],pp2z[2];
    double pp10[2],pp20[2];

    //construct full solution
    pp1z[0] = p1za;
    pp2z[0] = -(p3.Z()+p1za);

    pp1z[1] = p1zb;
    pp2z[1] = -(p3.Z()+p1zb);

    //transverse mass
    double am1t2 = sq(mass[1]) + sq(p1t);
    double am2t2 = sq(mass[2]) + sq(p2t);

    //construct energies
    pp10[0] = sqrt(am1t2+sq(pp1z[0]));
    pp10[1] = sqrt(am1t2+sq(pp1z[1]));

    pp20[0] = sqrt(am2t2+sq(pp2z[0]));
    pp20[1] = sqrt(am2t2+sq(pp2z[1]));

    assert( isol <2 && isol >=0 );

    double p1testz = pp1z[isol];		
    double p2testz = pp2z[isol];		

    double energy1 = sqrt(am1t2+sq(p1testz));
    double energy2 = sqrt(am2t2+sq(p2testz));

    double esum = energy1+energy2+p3_0;

    if(fabs(esum-tecm)>0.001)
		return 3;
    
    p1.SetXYZT(pp1.X(), pp1.Y(), pp1z[isol], pp10[isol]); 
    p2.SetXYZT(pp2.X(), pp2.Y(), pp2z[isol], pp20[isol]);


    TLorentzVector p13,p23;
    p13 = p1+p3;
    p23 = p2+p3;

    if( p13.M2() > s ) 
		return 4;
    
    if( p23.M2() > s ) 
		return 6;

    TLorentzVector p0a=pb[1] - p1 - p3;
    TLorentzVector p0b=pb[2] - p2 - p3;
    
    
    //decay third particle and boost  
    PM = p3;
        
    Decay->SetDecay(p3, nop-2, mass+3);
	
	wtdecay = Decay->Generate( rndQueue );
	
	pf[1]=p1;
	pf[2]=p2;
    
    for( int i = 3; i < nop+1; i++ )
    {
		pf[i] = *(Decay->GetDecay( i-3 ));
	}
 
   
   return 0;

};

////////////////////////////////////////////////////////////////////////
double TEventMaker2toN::kcms(double s, double m1, double m2)
{
  // momentum in cms of systme of particles with masses m1, m2
  double m12=m1*m1;
  double m22=m2*m2;
  double temp= sqrt(pow((s-m12-m22),2)-4.0*m12*m22)/2.0/sqrt(s);
  return temp;

};

////////////////////////////////////////////////////////////////////////
double TEventMaker2toN::SetEvent( int nDim, double *Xarg )
{
		
	//GENERATE MOMENTA AND RAPIDITIES starting from nDim random numbers
	// nDim=3*(number of particles)-4

	//nomber of true degrees of freedom
	assert( nDim == 3 * nop - 4 + 1 );
	

	//set up  new generation
	generationFailed = false;

    double p1t = p_min + abs(p_max-p_min)*Xarg[0];
    double p2t = p_min + abs(p_max-p_min)*Xarg[1];
    double dphi1 = 2.0*PI*Xarg[2];
    double dphi2 = 2.0*PI*Xarg[3];
    double y = y_min+(y_max-y_min)*Xarg[4];
   //intermediate mass
    double mass2 = mass2_min + (mass2_max-mass2_min)*Xarg[5];
	
	int isol=1;
	
	if( Xarg[6] <= 0.5 )
	{
		isol = 1;
	}
	else
	{
		isol = 0;
	}
	
	////clear queue - in case there was error in previous run
	while (!rndQueue.empty())
	{
		rndQueue.pop();
	}

	//put rnd numbers into queue
	for( int i = 7; i < nDim; i++)
	{
		rndQueue.push( Xarg[i] );
	}  
 
    
    int r=0; 
     
    r = CalculateKinematics(p1t, p2t, dphi1, dphi2, y, mass2, isol);
    if(!r==0)generationFailed=true;
	
	//generate weight 
	double norm3=pow( 2.0 * PI, -3*3 + 4 )*pow(2,-3);
  
	double wt = 0.0;
	
	double jacob_rndm_3 = 0.0;
	
	if ( false == generationFailed )
	{
         double jacob_rndm_3 = abs(p_max-p_min) * abs(p_max-p_min) * 2 *PI* 2*PI*abs(y_max-y_min)*(mass2_max-mass2_min);//PhaseSpace-->dXarg for a+b->1+2+M
         
         double jacobinv_phs = fabs( pf[1].Z()/pf[1].E() - pf[2].Z()/pf[2].E());
         
         double jacob3 = jacob_rndm_3/jacobinv_phs;

		 double flux = 2.0*tecm*tecm;
		 
		 wt = 1.0/flux;
		 
		 wt *= 2.0; //correction for random choose of solution
				
	     double wt3 = wt * norm3 * jacob3 * pf[1].Pt() * pf[2].Pt()/pf[1].E()/pf[2].E();
	     
		 wt = wt3;
		 
		 //multiply by the decay weight
		 wt *= wtdecay;
	 
	}
	else //generation failed
	{
		wt   = 0.0;
	}
	
	weight = wt/jacob_rndm_3;
	
	return( wt );
};

////////////////////////////////////////////////////////////////////////
bool TEventMaker2toN::isGenerationFailed( void )
{
	
	return( generationFailed );
};

////////////////////////////////////////////////////////////////////////
double TEventMaker2toN::getWeight( void )
{
	return( weight );
};
	
////////////////////////////////////////////////////////////////////////
double TEventMaker2toN::getDecayWeight( void )
{
	return( wtdecay );
};
	
