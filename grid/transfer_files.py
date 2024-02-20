import glob
import os



dm_type = "fermion"
base_dir = "/pnfs/uboone/scratch/users/lmoralep/DarkTridentGen/run_90/16-03-2023/"
output_dir = "/pnfs/uboone/persistent/users/lmoralep/DarkTridentGen/fermion_ratio_0.6/"

masses_both = [0.01, 0.02, 0.03, 0.04,  0.05, 0.06,  0.07,  0.08,  0.09]
masses_eta = [0.1,  0.2, 0.3,  0.4]


for m in masses_both:
    temp_pi0_files_bdnmc = "{:.2f}/pi0_decay/{}/files/events_{:.2f}_{}_*.root".format(m,dm_type,m,dm_type)
    temp_pi0_files_hepevt = "{:.2f}/pi0_decay/{}/files/hepevt_{:.2f}_{}_*.root".format(m,dm_type,m,dm_type)

    out_pi0_dir = output_dir + "{:.2f}/{}".format(m,"pi0_decay")
    
    os.system("mkdir -p " + out_pi0_dir) 
    os.system("cp {} {}".format(base_dir + temp_pi0_files_bdnmc, out_pi0_dir))
    os.system("cp {} {}".format(base_dir + temp_pi0_files_hepevt, out_pi0_dir))

    temp_eta_files_bdnmc = "{:.2f}/eta_decay/{}/files/events_{:.2f}_{}_*.root".format(m,dm_type,m,dm_type)
    temp_eta_files_hepevt = "{:.2f}/eta_decay/{}/files/hepevt_{:.2f}_{}_*.root".format(m,dm_type,m,dm_type)

    out_eta_dir = output_dir + "{:.2f}/{}".format(m,"eta_decay")
    
    os.system("mkdir -p " + out_eta_dir) 
    os.system("cp {} {}".format(base_dir + temp_eta_files_bdnmc, out_eta_dir))
    os.system("cp {} {}".format(base_dir + temp_eta_files_hepevt, out_eta_dir))




for m in masses_eta:

    temp_eta_files_bdnmc = "{:.1f}/eta_decay/{}/files/events_{:.1f}_{}_*.root".format(m,dm_type,m,dm_type)
    temp_eta_files_hepevt = "{:.1f}/eta_decay/{}/files/hepevt_{:.1f}_{}_*.root".format(m,dm_type,m,dm_type)



    out_eta_dir = output_dir + "{:.1f}/{}".format(m,"eta_decay")
    os.system("mkdir -p " + out_eta_dir) 
    os.system("cp {} {}".format(base_dir + temp_eta_files_bdnmc, out_eta_dir))
    os.system("cp {} {}".format(base_dir + temp_eta_files_hepevt, out_eta_dir))