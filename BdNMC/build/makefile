CXXFLAGS = $(shell root-config --cflags) $(shell root-config --libs) -Wall

VPATH = ../src:header

CXX = g++

DEPDIR = .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILE.cpp = $(CXX) $(DEPFLAGS) $(CXXFLAGS)
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d
OUTPUT_OPTION = -o

OBJ = main.o Position_Distributions.o DMscattering.o Random.o Kinematics.o sanfordwang.o Particle.o Parameter.o detector.o decay.o decay_gen_kinetic_mixing.o decay_gen_baryonic.o Integrator.o record.o record_root.o branchingratios.o DMNscattering.o Electron_Scatter.o Nucleon_Scatter.o Particle_List.o partonsample.o parton_V_gen.o DMNscattering_Baryonic.o BurmanSmith.o BMPT_dist.o minimization.o Proton_Brem_Distribution.o V_decay_gen.o Pion_Inelastic.o Inelastic_Nucleon_Scatter.o piminus_capture_gen.o SignalDecay.o Axion_Dark_Photon.o

.PHONY: all
all: main


# Now this works for Ubuntu too... 

main : $(OBJ)
	$(CXX) $(OUTPUT_OPTION) $@ $^ $(DEPFLAGS) $(CXXFLAGS)
	
%.o : %.cpp $(DEPDIR)/%.d
	$(CXX) $(OUTPUT_OPTION) $@ $< $(DEPFLAGS) $(CXXFLAGS) -c 
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(OBJ)))


.PHONY: clean
clean:
	rm *.o
	rm -r .d
