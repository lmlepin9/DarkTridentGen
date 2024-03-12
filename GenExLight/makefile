# ROOT:
# -flags
 ROOTCFLAGS      := $(shell root-config --cflags)                                
 ROOTLIBS       := $(shell root-config --libs) -lFoam -lEG ./TDecayTFoam/TDecay.o                                  
 ROOT             = $(ROOTCFLAGS) $(ROOTGLIBS) 
 
# -includes 
 ROOTINC          = $(shell root-config --incdir)


#======================================================================
#####  GNU c++ compiler
CXXFLAGS    = -g --std=c++17 -I ./TDecayTFoam -I$(ROOTINC) $(ROOTLIBS)
#LDFLAGS     = $(GSLLDFLAGS) 
CXX         =  g++


#-----------------------------
# Compile main C++ file 
evgen.exe : evgen.cxx
	$(CXX) $< $(CXXFLAGS)  -o evgen.exe
#======================================================================