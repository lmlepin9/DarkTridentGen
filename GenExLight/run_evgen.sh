#!/bin/bash 
dm_type="fermion"
RATIO=0.6
MASS=0.05
XSEC_MASS=0.05
XSEC_RATIO=0.60
./evgen.exe -i /uboone/data/users/lmoralep/GeneratorTest/bdnmc_pi0_fermion_4k_new_test.root -x /uboone/app/users/lmoralep/DM-MicroBooNE/DarkTridentGen/xsec/cross_section_${XSEC_MASS}_ratio_${XSEC_RATIO}_fermion_fix.root -f root -t ${dm_type} -o /uboone/data/users/lmoralep/GeneratorTest/hepevt_${dm_type}_${decay}_${MASS}_ratio_${RATIO}_03-10-2023 -m ${MASS} -r ${RATIO} -s 900
#./evgen_cross_check.exe -i /uboone/data/users/lmoralep/GeneratorTest/bdnmc_pi0_scalar_1k_testing_b.root -x /uboone/app/users/lmoralep/DM-MicroBooNE/DarkTridentGen/xsec/cross_section_ma_0.02_mx_0.01_fermion_electron.root -s 900 
#rm ./cache/* 
