# DarkTridentGen: Generator of dark trident interactions in LArTPCs

## Authors:

Pawel Guzowski, Anyssa Navrer-Agasson, Luis Mora


## Dependencies:

This package relies on two existing tools.

BdNMC: Simulates the decay of neutral mesons into DM particles
by using NuMI flux files.

GenExLight: Simulates the scattering of DM particles intersecting
the detector volume. 


## How to run:

Obtain the [g4NuMI neutral meson flux](https://cernbox.cern.ch/s/U4uy7tRbj4gYYmo) files 
and locate them in a new directory in the top directory

mkdir -p neutral_mesons 

Compile and run BdNMC using a template parameter file:







