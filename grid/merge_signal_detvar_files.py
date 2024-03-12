import glob
import os
import sys 

          
det_vars = ["CV", "ly_atten", "ly_down", "ly_rayleigh", "recomb", "sce", "wiremod_anglexz", "wiremod_angleyz", "wiremod_dEdx", "wiremod_x", "wiremod_yz"]
 

def merge_sp(run,mass):
    base_dir = "/pnfs/uboone/scratch/users/lmoralep/dark_tridents/v08_00_00_61/"
    output_dir = "/pnfs/uboone/persistent/users/lmoralep/single_photon_files/{}_signal_dt_vars/".format(run)
    prefix_name = "{}_0.6_0.6_ma_{}".format(run,mass) 
    prefix_output = "{}_ratio_0.6_ma_{}_{}_sp.root"
    for var in det_vars:
        full_path = base_dir + prefix_name + "_" + var + "/ana_sp/*/vertexed*.root"
        output_file = output_dir + prefix_output.format(run,mass,var)
        os.system("hadd -f " + output_file + " " + full_path)
        #print("hadd -f " + output_file + " " + full_path)



def merge_larcv(run,mass):
    base_dir = "/pnfs/uboone/scratch/users/lmoralep/dark_tridents/v08_00_00_40a_dl/"
    output_dir = "/pnfs/uboone/persistent/users/lmoralep/larcv_files/{}_signal_det_variations/".format(run)
    prefix_name = "{}_0.6_0.6_ma_{}".format(run,mass) 
    prefix_output = "{}_ratio_0.6_ma_{}_{}_larcv.root"
    for var in det_vars:
        full_path = base_dir + prefix_name + "_" + var + "/larcv/*/out_larcv_test.root"
        output_file = output_dir + prefix_output.format(run,mass,var)
        os.system("hadd -f " + output_file + " " + full_path)
        #print("hadd -f " + output_file + " " + full_path)




if __name__ == '__main__':
    print("Your input parameters: ", sys.argv[1:])
    merge_sp(sys.argv[1:][0],sys.argv[1:][1])