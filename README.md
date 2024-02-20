# DarkTridentGen: Generator of dark trident interactions in LArTPCs

## Authors:

Pawel Guzowski, Anyssa Navrer-Agasson and Luis Mora


## Dependencies:

This package relies on two existing tools and we do not claim authorship 
of any of the following of neither BdNMC or GenExLight. 

BdNMC: Simulates the decay of neutral mesons into DM particles
by using NuMI flux files.

GenExLight: Simulates the scattering of DM particles intersecting
the detector volume. 


## How to run:

Setup the generator environment variables and download flux files:

source setup.sh

Compile and run BdNMC using a parameter file

./BdNMC/bin/BdNMC parameter_fermion_test_pi0.dat

Output files will stored in a specific directory created by setup.sh 









