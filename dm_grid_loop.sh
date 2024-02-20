#!/bin/bash
echo "Bash version ${BASH_VERSION}..."
echo "Submitting dark trident generator jobs..." 


dm_type="scalar"
m_ratio=0.60



#declare -a mass_points_pi0=(0.010 0.020 0.030)
#declare -a mass_points_eta=(0.010 0.020 0.030 0.040 0.050 0.060 0.065 0.070 0.075 0.080 0.085 0.090 0.095 0.100 0.105 0.110 0.115 0.120 0.125)
#declare -a mass_points_eta=(0.110 0.115)
declare -a mass_points_pi0=(0.01 0.02 0.03 0.04 0.05 0.06 0.07 0.08 0.09 0.10 0.20 0.30 0.40)
declare -a mass_points_eta=(0.01 0.02 0.03 0.04 0.05 0.06 0.07 0.08 0.09 0.10 0.20 0.30 0.40)


#for mA in $(seq 0.01 0.01 0.09)
#  do 
#    python ./grid/jobsub_dark_tridents.py --make_tar true --run_number 80 --nevts 12000 --n_jobs 1 --mA $mA --ratio $m_ratio --decay_channel pi0_decay --signal_channel 26-04-2023 --template ./templates/template_parameter_uboone_root_pi0.dat --dm_type $dm_type
#    python ./grid/jobsub_dark_tridents.py --make_tar true --run_number 80 --nevts 2500 --n_jobs 1 --mA $mA --ratio $m_ratio --decay_channel eta_decay --signal_channel 26-04-2023 --template ./templates/template_parameter_uboone_root_eta.dat --dm_type $dm_type
#  done


#for mA in $(seq 0.10 0.1 0.40)
#  do 
#    python ./grid/jobsub_dark_tridents.py --make_tar true --run_number 80 --nevts 2500 --n_jobs 1 --mA $mA --ratio $m_ratio --decay_channel eta_decay --signal_channel 26-04-2023 --template ./templates/template_parameter_uboone_root_eta.dat --dm_type $dm_type
#  done



for mA in "${mass_points_pi0[@]}"
  do
    python ./grid/jobsub_dark_tridents.py --run_number 400 --nevts 12000 --n_jobs 5 --mA $mA --ratio $m_ratio --decay_channel pi0_decay --signal_channel 06-10-2023 --template ./templates/template_parameter_uboone_root_pi0.dat --dm_type $dm_type
  done



for mA in "${mass_points_eta[@]}"
  do
    python ./grid/jobsub_dark_tridents.py --run_number 400 --nevts 2500 --n_jobs 5 --mA $mA --ratio $m_ratio --decay_channel eta_decay --signal_channel 06-10-2023 --template ./templates/template_parameter_uboone_root_eta.dat --dm_type $dm_type
  done 