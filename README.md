# DarkTridentGen: Generator of dark trident interactions in LArTPCs

## Authors:

Pawel Guzowski, Anyssa Navrer-Agasson and Luis Mora


## Dependencies:

This package relies on two existing tools. We do not claim authorship 
of BdNMC or GenExLight. 

[BdNMC](https://github.com/pgdeniverville/BdNMC): Simulates the decay of neutral mesons into DM particles
by using NuMI flux files. The custom version used in this generator has some features specially tailored for 
the dark trident search:

* Neutral meson decay to fermion dark matter 
* Use of NuMI neutral meson flux files including weights
* Output to ROOT files 

[GenExLight](https://github.com/rkycia/GenExLight): Simulates the scattering of DM particles intersecting
the detector volume. Custom features that we included in this version:

* Matrix element of dark trident scattering, both fermion and scalar cases
* Fix of the flux factor in the cross section calculation 


## How to run:

Setup the generator environment variables and download flux files:

`source setup.sh`

Compile and run BdNMC using a parameter file. The path to the meson flux directory
and to the output directory have to be declared before running BdNMC. 

`./BdNMC/bin/BDNMC BdNMC/parameter_fermion_test_pi0.dat` (from within folder DarkTridentGen/)

## This Branch
* Has dynamic paths
* To Do - electron dark matter scattering






