# Instructions for uboonecode interface

1. Activate the SL7 container

<pre>/cvmfs/oasis.opensciencegrid.org/mis/apptainer/current/bin/apptainer shell --shell=/bin/bash -B /cvmfs,/exp,/nashome,/pnfs/uboone,/opt,/run/user,/etc/hostname,/etc/hosts,/etc/krb5.conf --ipc --pid /cvmfs/singularity.opensciencegrid.org/fermilab/fnal-dev-sl7:latest</pre>   


2. The uboonecode version used for the dark trident analysis is v08_00_00_61, so invoke it through UPS

<pre>
version="v08_00_00_61"                                                                          	 
qual="e17:prof"                                                                                 	 
echo "Setting up uboonecode: ${version}"                                                        	 
source /cvmfs/uboone.opensciencegrid.org/products/setup_uboone_mcc9.sh                          	 
setup uboonecode $version -q $qual  
</pre>                                                            	 
                               	 

3. Create your own development area: 

<pre>
mrb newDev -v $version -q $qual 
source localProducts_larsoft_v08_00_00_61_e17_prof/setup
cd src
mrb g -t v08_00_00_61 uboonecode 
mrb g -t 08_02_00_14 larsim 
mrbsetenv 
mrb i -j 4
mrbslp
</pre>   

4. Copy the fcl files from this directory to your /src/larsim/larsim directory.


5. Recompile your development area
<pre>
mrbsetenv
mrb i -j 4
mrbslp
</pre> 

6. Run the fcl file chain. Note, you can use the example input.txt (only 17 dark trident events, so your N_EVENTS is 17) to test this framework.

<pre>
lar -c proddm_wirecell_g4_uboone.fcl -n N_EVENTS -o dark_trident_g4.root
lar -c wirecell_detsim_numi_uboone.fcl -s dark_trident_g4.root -o dark_trident_g4_detsim.root -n -1
lar -c reco_uboone_mcc9_8_driver_stage1.fcl -s dark_trident_g4_detsim.root -o dark_trident_g4_detsim_reco1.root -n -1
lar -c reco_uboone_mcc9_8_driver_stage2_numi_fullMC.fcl -s dark_trident_g4_detsim_reco1.root -o dark_trident_g4_detsim_reco1_reco2.root -n -1
lar -c run_SinglePhoton.fcl -s dark_trident_g4_detsim_reco1_reco2.root -n -1
</pre> 

7. Overlays: To DO