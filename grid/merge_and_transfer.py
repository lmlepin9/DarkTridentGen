import glob
import os



dm_type = "fermion"
base_dir = "/pnfs/uboone/scratch/users/lmoralep/DarkTridentGen/run_80/16-03-2023/"
output_dir = "/pnfs/uboone/persistent/users/lmoralep/DarkTridentGen/fermion_ratio_2.0/"

masses_both = [0.01, 0.02, 0.03, 0.04,  0.05, 0.06,  0.07,  0.08,  0.09]
masses_eta = [0.1,  0.11, 0.12,  0.13, 0.14, 0.15]


for m in masses_both:
    
    temp_pi0_files_bdnmc = "{:.2f}/pi0_decay/{}/files/events_{:.2f}_{}_*.root".format(m,dm_type,m,dm_type)
    temp_pi0_files_hepevt = "{:.2f}/pi0_decay/{}/files/hepevt_{:.2f}_{}_*.root".format(m,dm_type,m,dm_type)
    temp_bdnmc_pi0 = "bdnmc_events_pi0_decay_ma_" + str(m) + ".root"
    temp_hepevt_pi0 = "hepevt_events_pi0_decay_ma_" + str(m) + ".root"
    os.system("hadd -f " + output_dir + temp_bdnmc_pi0 + " " + base_dir + temp_pi0_files_bdnmc)
    os.system("hadd -f " + output_dir + temp_hepevt_pi0 + " " + base_dir + temp_pi0_files_hepevt)

    temp_eta_files_bdnmc = "{:.2f}/eta_decay/{}/files/events_{:.2f}_{}_*.root".format(m,dm_type,m,dm_type)
    temp_eta_files_hepevt = "{:.2f}/eta_decay/{}/files/hepevt_{:.2f}_{}_*.root".format(m,dm_type,m,dm_type)
    temp_bdnmc_eta = "bdnmc_events_eta_decay_ma_" + str(m) + ".root"
    temp_hepevt_eta = "hepevt_events_eta_decay_ma_" + str(m) + ".root"
    os.system("hadd -f " + output_dir + temp_bdnmc_eta + " " + base_dir + temp_eta_files_bdnmc)
    os.system("hadd -f " + output_dir + temp_hepevt_eta + " " + base_dir + temp_eta_files_hepevt)




for m in masses_eta:

    if(m == 0.1):
        temp_eta_files_bdnmc = "{:.1f}/eta_decay/{}/files/events_{:.1f}_{}_*.root".format(m,dm_type,m,dm_type)
        temp_eta_files_hepevt = "{:.1f}/eta_decay/{}/files/hepevt_{:.1f}_{}_*.root".format(m,dm_type,m,dm_type)

    else: 
        temp_eta_files_bdnmc = "{:.2f}/eta_decay/{}/files/events_{:.2f}_{}_*.root".format(m,dm_type,m,dm_type)
        temp_eta_files_hepevt = "{:.2f}/eta_decay/{}/files/hepevt_{:.2f}_{}_*.root".format(m,dm_type,m,dm_type)

    temp_bdnmc_eta = "bdnmc_events_eta_decay_ma_" + str(m) + ".root"
    temp_hepevt_eta = "hepevt_events_eta_decay_ma_" + str(m) + ".root"
    os.system("hadd -f " + output_dir + temp_bdnmc_eta + " " + base_dir + temp_eta_files_bdnmc)
    os.system("hadd -f " + output_dir + temp_hepevt_eta + " " + base_dir + temp_eta_files_hepevt)
