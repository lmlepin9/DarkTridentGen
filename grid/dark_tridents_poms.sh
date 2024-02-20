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
echo "======== UPDATE MACRO WITH CORRECT PARAMETERS ========"
SEED=$((RUN+PROCESS))
sed -i 's/\${seed}/'$SEED'/g' parameter_uboone_grid.dat
OUTFILE="dark_tridents_bdnmc_alD=${ALD}_mA=${MA}_${DM_TYPE}_${PROCESS}.root"
sed -i 's/\${outroot}/'$OUTFILE'/g' parameter_uboone_grid.dat

echo "PROCESS=$PROCESS"
echo "SEED=$SEED"
echo "OUTFILE=$OUTFILE"

echo
echo "======== PARAMETER FILE CONTENT ========"
cat parameter_uboone_grid.dat

echo
echo "======== EXECUTING BdNMC ========"
echo "./BdNMC/bin/BDNMC parameter_uboone_grid.dat"
./BdNMC/bin/BDNMC parameter_uboone_grid.dat

echo
echo "======== EXECUTING INTEGRAL ========"
echo "./integral_mass.exe -E ene -m mA"
./integral_range.exe -m ${MA}

echo
echo "======== EXECUTING EVGEN ========"
echo "./evgen_rootinput.exe -i $OUTFILE -x events -o events_uboone_0.05_test.root -h hepevt_uboone_0.05_test.txt"
./evgen_rootinput.exe -i BdNMC/$OUTFILE -x cross_section.root -s $SEED -m ${MA} -r ${RATIO} -o dark_tridents_evgen_alD=${ALD}_mA=${MA}_ratio_${RATIO}_${DM_TYPE}_${PROCESS}.root -h hepevt_alD=${ALD}_mA=${MA}_ratio_${RATIO}_${DM_TYPE}_${PROCESS}.txt

echo
echo "======== COUNT EVENTS IN HEPEVT ========"

lines=${wc -l < hepevt_alD=${ALD}_mA=${MA}_ratio_${RATIO}_${DM_TYPE}_${PROCESS}.txt}
if(( $lines/5 < 50 )); then
    echo "WARNING: less than 50 evts in hepevt file"
    mv hepevt_alD=${ALD}_mA=${MA}_ratio_${RATIO}_${DM_TYPE}_${PROCESS}.txt hepevt_alD=${ALD}_mA=${MA}_ratio_${RATIO}_${DM_TYPE}_${PROCESS}_under50.txt
fi

echo
echo "Moving output to CONDOR_DIR_DARKTRIDENT"

rm cross_section.root

mv BdNMC/dark_tridents_bdnmc_* $CONDOR_DIR_DARKTRIDENT

mv *.root $CONDOR_DIR_DARKTRIDENT
mv *.txt $CONDOR_DIR_DARKTRIDENT

