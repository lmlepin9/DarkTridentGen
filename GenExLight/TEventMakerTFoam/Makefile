#----------------------------------------------------------------------
# NOTES: 
#-ROOTSYS variable should be properly set into directory of ROOT
#-ROOTINC variable shows include directory form ROOT instalation, usually ($ROOTSYS)/include
#----------------------------------------------------------------------
#       make Demo-run
#----------------------------------------------------------------------
.SUFFIXES:  .o .c .cxx .f .l  .s  .sh  .h  .a
#======================================================================
# Parallel compilation
#MAKEFLAGS += -j 4  #fixed number of cores
MAKEFLAGS+="-j -l $(shell grep -c ^processor /proc/cpuinfo) "


# ROOT:
# -flags
 ROOTCFLAGS      := $(shell root-config --cflags)                                
 ROOTGLIBS       := $(shell root-config --glibs)                                 
 ROOT             = $(ROOTCFLAGS) $(ROOTGLIBS) -lFoam -lEG
 
# -includes 
 ROOTINC          = $(ROOTSYS)/include

#GSL:

 GSLLDFLAGS      := $(shell gsl-config --libs)

#valgrin suppresion
VALGRINDSUPP = $(ROOTSYS)/etc/valgrind-root.supp

#======================================================================
#####  GNU c++ compiler
CXXFLAGS    = -g  -ansi -Wall  -I. -I$(ROOTINC) -std=c++11
LDFLAGS     = $(GSLLDFLAGS) 
CXX         =  g++
CLD         =  g++
#-----------------------------
# Compile all c++ files
%.o : %.cxx
	$(CXX) $(CXXFLAGS) -c $<
#======================================================================


#------------------------------------------------------------------------------
#HHEADERS - contain classes headers for which root dictionaries will be created
#HEADEREXT := h
#HEADERPATH := .
#HEADERS := $(shell find $(HEADERPATH) -type f -name *.$(HEADEREXT))
HEADERS = TDecay.h TEventMaker2toN.h


#OBJECTS - classes to compile (all nonabstract/non-interface classes)
OBJECTS = $(HEADERS:.h=.o)


#------------------------------------------------------------------------------
#Compile program
main.exe: main.o $(OBJECTS) 
	$(CLD) -o main.exe $^ $(ROOT) $(LDFLAGS)
#------------------------------------------------------------------------------
#Compile and run program
run: main.exe
	(time ./main.exe)
#------------------------------------------------------------------------------
#Generatr documentation
Generate-doc:	
	-doxygen -g 
	-echo 'PROJECT_NAME = "Z decay MC"' >> Doxyfile
	-doxygen Doxyfile
	-firefox ./html/index.html

#-------------------------------------------------------------------------------
#Technical checks using valgrind:
#-------------------------------------------------------------------------------
check-memleak: clean main.exe
	 valgrind --leak-check=yes --show-reachable=yes --suppressions=$(VALGRINDSUPP) --log-file=valgrin.log ./main.exe
#-------------------------------------------------------------------------------
check-memleakfull: clean main.exe
	 valgrind --leak-check=full --show-reachable=yes --suppressions=$(VALGRINDSUPP) --log-file=valgrin.log ./main.exe
#-------------------------------------------------------------------------------
check-prof: clean main.exe 
	 valgrind --tool=cachegrind --suppressions=$(VALGRINDSUPP) --log-file=valgrin.log ./main.exe
	 cg_annotate cachegrind.out.*
#-------------------------------------------------------------------------------
check-call: clean main.exe
	valgrind --tool=callgrind --suppressions=$(VALGRINDSUPP) --log-file=valgrin.log ./main.exe
	callgrind_annotate callgrind.out.*
#-------------------------------------------------------------------------------
check-alloc: clean main.exe
	valgrind  --tool=exp-dhat --suppressions=$(VALGRINDSUPP) --log-file=valgrin.log ./main.exe
#-------------------------------------------------------------------------------
check-heap: clean main.exe
	valgrind  --tool=massif --suppressions=$(VALGRINDSUPP)--log-file=valgrin.log ./main.exe
	ms_print massif.out.*
#-------------------------------------------------------------------------------
line_count:
	(cat *.cxx *.h | wc -l )
#==============================================================================

#==============================================================================
clean:
	(rm -f *.o *.a *~ *.exe core *.gz lmap *.out *.dvi *.aux *.log FoamDict* output-Demo *.out.* Dict.cxx Dict.h)
#==============================================================================
cleanest: clean
	(rm -f *.eps *.root)


#------------------------------------------------------------------------------
