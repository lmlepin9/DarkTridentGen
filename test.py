import matplotlib.pyplot as plt
import numpy as np

mx = np.linspace(0.01, 0.1, 100)
s = 1
mpi0 = 0.135
alphaD = 0.1
kappa = 0.1
theta = 0.1
pi = np.pi
mv = 0.1
def GammaV(mv, mx, kappa, alphaD):
    return alphaD*(mv*mv-4*mx*mx)*np.sqrt(mv*mv/4-mx*mx)


def equation1(mx, s, mpi0, alphaD, kappa, theta, pi, mv):
    return np.sqrt(1-4*mx*mx/s)*np.power(np.power(mpi0,2)-s,3)*(s-4*mx*mx)*alphaD*np.power(kappa,2)*np.power(np.sin(theta),3)/(8*np.power(mpi0,6)*pi*(np.power(s-mv*mv,2)+np.power(mv*GammaV(mv,mx,kappa,alphaD),2)))

def equation2(mx, s, mpi0, alphaD, kappa, theta, pi, mv):
    
    return 4*np.sqrt(1-4*mx*mx/s)*np.power(np.power(mpi0,2)-s,3)*(s+2*mx*mx)*alphaD*np.power(kappa,2)*np.power(np.sin(theta),3)/(8*np.power(mpi0,6)*pi*(np.power(s-mv*mv,2)+np.power(mv*GammaV(mv,mx,kappa,alphaD),2)))


plot1 = [equation1(i, s, mpi0, alphaD, kappa, theta, pi, mv) for i in mx]
plot2 = [equation2(i, s, mpi0, alphaD, kappa, theta, pi, mv) for i in mx]
plt.plot(mx, plot1, label='equation1')
plt.plot(mx, plot2, label='equation2')
plt.show()



    """double denominator {-2.0 * me*me * ((Ep1*Ep1)-(mx*mx))};
    double prefactor {alphaD * alphaEM * epsilon * epsilon * pi};
    double t1 {(-2.0 * Ek2 * me)};
    double t2 {(ma*ma*ma*ma)+ (8.*Ep1*Ep1*me*me) + (2.*ma*ma*((2.*Ep1*me) + (me*me) + (mx*mx))) / ((ma*ma) + (2. * (Ek2-me) * me))};
    double t3 {2.* ((ma*ma) + (2.*Ep1*me) + (me*me) + (mx*mx)) * log((ma*ma) + (2.*(Ek2-me)*me))};

    return (prefactor * (t1 + t2 + t3)) / denominator;"""
    
    