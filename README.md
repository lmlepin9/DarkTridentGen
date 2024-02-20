# DarkTridentGen: Generator of dark trident interactions in LArTPCs

## Authors:

Pawel Guzowski, Anyssa Navrer-Agasson and Luis Mora


## Dependencies:

This package relies on two existing tools. We do not claim authorship 
of BdNMC or GenExLight. 

[BdNMC](https://github.com/pgdeniverville/BdNMC): Simulates the decay of neutral mesons into DM particles
by using NuMI flux files.

[GenExLight](https://github.com/rkycia/GenExLight): Simulates the scattering of DM particles intersecting
the detector volume. 


## How to run:

Setup the generator environment variables and download flux files:

source setup.sh

Compile and run BdNMC using a parameter file

./BdNMC/bin/BdNMC parameter_fermion_test_pi0.dat

Output files will stored in a specific directory created by setup.sh 









