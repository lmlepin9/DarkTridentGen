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
* Has Fermionic dark matter - electron scattering

## Running On Noether With CVM-FS
Noether now has CVM-FS, so we can use the ROOT versions from this.
```
$ qrsh    # jump to work node
$ source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.12.06/x86_64-fedora26-gcc72-opt/root/bin/thisroot.sh
$ cd DarkTridentGen   # Go to your DarkTridentGen folder
$ echo $PATH | tr ":" "\n" # should see the following returned
/cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.12.06/x86_64-fedora26-gcc72-opt/root/bin
...
$ ./BdNMC/bin/BDNMC BdNMC/parameter_fermion_test_pi0.dat
```

## Running On Noether Without CVM-FS
This requires a specific version of a precompiled binary of ROOT. We reccomend https://root.cern/releases/release-61206/ fedora27.
```
$ qrsh
$ wget https://root.cern/download/root_v6.12.06.Linux-fedora27-x86_64-gcc7.2.tar.gz
$ tar -xzvf root_v6.12.06.Linux-fedora27-x86_64-gcc7.2.tar.gz
$ source root/bin/thisroot.sh
$ echo $PATH | tr ":" "\n"
/gluster/home/USER/....../root/bin
$ cd DarkTridentsGen    # go to wherever your DarkTridentGen is cloned to
$ ./BdNMC/bin/BDNMC BdNMC/parameter_fermion_test_pi0.dat
```







