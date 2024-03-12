#!/bin/bash

dm_type="scalar"
#g++ test_xsec_point.cpp -I$(root-config --incdir) --std=c++17 -I ../GenExLight/TDecayTFoam $(root-config --libs) -lFoam -lEG ../GenExLight/TDecayTFoam/TDecay.o -o test_xsec_point.exe


g++ ${dm_type}_xsec.cxx -I$(root-config --incdir) --std=c++17 -I ../GenExLight/TDecayTFoam $(root-config --libs) -lFoam -lEG ../GenExLight/TDecayTFoam/TDecay.o -o ${dm_type}_xsec.exe

#g++ scalar_xsec_2to2.cxx -I$(root-config --incdir) --std=c++17 -I ../GenExLight/TDecayTFoam $(root-config --libs) -lFoam -lEG ../GenExLight/TDecayTFoam/TDecay.o -o scalar_xsec_2to2.exe