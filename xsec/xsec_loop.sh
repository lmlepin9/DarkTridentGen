#!/bin/bash

RATIO=0.60
DM_TYPE="scalar"


#for mA in $(seq 0.01 0.01 0.09)
#    do
#        echo "Processing ${DM_TYPE} xsec for dark photon mass: ${mA} GeV and mass ratio ${RATIO}"
#        ./${DM_TYPE}_xsec.exe -m $mA -r $RATIO
#    done 




#declare -a mass_points=(0.010, 0.020, 0.030, 0.040, 0.050, 0.060, 0.065, 0.070, 0.075, 0.080, 0.085, 0.090, 0.095, 0.100, 0.105, 0.110, 0.115, 0.120, 0.125)

#declare -a mass_points=(0.110, 0.115)


declare -a mass_points=(0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.1, 0.2, 0.3, 0.4)

for mA in "${mass_points[@]}"
    do
        echo "Processing ${DM_TYPE} xsec for dark photon mass: ${mA} GeV and mass ratio ${RATIO}"
        ./${DM_TYPE}_xsec.exe -m $mA -r $RATIO
    done 
