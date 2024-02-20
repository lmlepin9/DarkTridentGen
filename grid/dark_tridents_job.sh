#!/bin/bash

echo
echo "======== cd to CONDOR_DIR_INPUT ========"
cd $CONDOR_DIR_INPUT

echo
echo "======== ls ========"
ls

echo
echo "======== UNTARRING... ========"
tar xvfz local_install_dark_tridents.tar.gz -C ./ > /dev/null

echo
echo "======== Done untarring. ls ========"
ls

echo "======== SETUP G4, ROOT, ETC ========"
source setup_evgen_grid.sh

echo
echo "======== UPDATE MACRO WITH RUN NUMBER ========"
SEED=$((RUN+PROCESS))
OUTFILE="events_${MA}_${DM_TYPE}_${PROCESS}.dat"
OUTROOT="events_${MA}_${DM_TYPE}_${PROCESS}.root"
SUMFILE="summary_${MA}_${DM_TYPE}_${PROCESS}.dat"
sed -i 's/\${seed}/'$SEED'/g' parameter_uboone_grid.dat
sed -i 's/\${decay_type}/'$DM_TYPE'/g' parameter_uboone_grid.dat
sed -i 's/\${outFile}/'$OUTFILE'/g' parameter_uboone_grid.dat
sed -i 's/\${sumFile}/'$SUMFILE'/g' parameter_uboone_grid.dat
sed -i 's/\${outroot}/'$OUTROOT'/g' parameter_uboone_grid.dat 

echo "PROCESS=$PROCESS"
echo "SEED=$SEED"
echo "OUTFILE=$OUTFILE"
echo "SUMFILE=$SUMFILE"
echo "ROOTFILE=$OUTROOT"

echo
echo "======== PARAMETER FILE CONTENT ========"
cat parameter_uboone_grid.dat

echo
echo "======== EXECUTING BdNMC ========"
echo "./BdNMC/bin/BDNMC parameter_uboone_grid.dat"
./BdNMC/bin/BDNMC parameter_uboone_grid.dat

echo "======= Listing BdNMC output files ========"

cd ./BdNMC

ls

cd ../

echo
echo "======== EXECUTING GenExLight ========"
echo "./evgen.exe -i ./BdNMC/events_${MA}_${DM_TYPE}_${PROCESS}.root -x ./xsec/cross_section_${MA}_ratio_${RATIO}_${DM_TYPE}_fix.root -f root -t ${DM_TYPE} -o hepevt_${MA}_${DM_TYPE}_${PROCESS}"
./evgen.exe -i ./BdNMC/events_${MA}_${DM_TYPE}_${PROCESS}.root -x ./xsec/cross_section_${MA}_ratio_${RATIO}_${DM_TYPE}_fix.root -f root -t ${DM_TYPE} -o hepevt_${MA}_${DM_TYPE}_${PROCESS} -m ${MA} -r ${RATIO} -s ${SEED}

echo
echo "Moving output to CONDOR_DIR_DARKTRIDENT"

#mv BdNMC/Events/*.dat $CONDOR_DIR_DARKTRIDENT
mv BdNMC/events_${MA}_${DM_TYPE}_${PROCESS}.root $CONDOR_DIR_DARKTRIDENT

mv hepevt_${MA}_${DM_TYPE}_${PROCESS}.root $CONDOR_DIR_DARKTRIDENT
mv hepevt_${MA}_${DM_TYPE}_${PROCESS}.txt $CONDOR_DIR_DARKTRIDENT

