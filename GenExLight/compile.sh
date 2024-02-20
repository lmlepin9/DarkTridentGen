#Script to compile GenExLight 

g++ evgen.cxx -I$(root-config --incdir) --std=c++17 -I ./TDecayTFoam $(root-config --libs) -lFoam -lEG ./TDecayTFoam/TDecay.o -o evgen.exe


#g++ evgen_cross_check.cxx -I$(root-config --incdir) --std=c++17 -I ./TDecayTFoam $(root-config --libs) -lFoam -lEG ./TDecayTFoam/TDecay.o -o evgen_cross_check.exe

