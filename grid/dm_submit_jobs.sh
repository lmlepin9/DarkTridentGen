#!/bin/bash
echo "Bash version ${BASH_VERSION}..."


python ./grid/jobsub_dark_tridents.py --make_tar true --run_number 900 --nevts 2000 --n_jobs 20 --mA 0.05 --decay_channel eta_decay --signal_channel 03_03_2023-2k --template ./templates/template_parameter_uboone_root_eta.dat --dm_type fermion


