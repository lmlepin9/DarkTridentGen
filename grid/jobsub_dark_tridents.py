#!/usr/bin/env python    

##################################################                                                                                                                                                                                                                              
# Dark Tridents grid job submitter                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
# Made to work with parameter_uboone_template_grid.dat                                                                                                                                                                                                                                 
# 2020.01 -- A.Navrer-Agasson                                                                                                                                                                                                                                                   
##################################################                                                                                                                                                                                                                              
import os, optparse, random, shutil, tarfile, sys
import subprocess, string
import time


PWD = os.getenv("PWD")

##################################################                                                                                                                                                                                                                              
# Job Defaults                                                                                                                                                                                                                                                                  
################################################## 
USER                  = os.getenv("USER") 
DO_TAR                = False
MA_DIR                = 0.05
RATIO                 = 0.6                                                                                                                                                                                                                           
NEVTS                 = 10000
N_JOBS                = 10
ALD                   = 0.1
DM_TYPE               = "fermion"
DECAY_CHAN            = "pi0_decay"
RUN_NUMBER            = 900
SEED                  = 20220727
SIGNAL                = "fermion_test_different_seed"
OUTDIR                = "/pnfs/uboone/scratch/users/{USER}/DarkTridentGen/{DECAY_CHAN}/run_{RUN_NUMBER}/{mA}/files/".format( USER = os.getenv("USER"), DECAY_CHAN = DECAY_CHAN, mA = MA_DIR, RUN_NUMBER = RUN_NUMBER)
LOGDIR                = "/pnfs/uboone/scratch/users/{USER}/DarkTridentGen/{DECAY_CHAN}/run_{RUN_NUMBER}/{mA}/log".format( USER = os.getenv("USER"), DECAY_CHAN = DECAY_CHAN, mA = MA_DIR, RUN_NUMBER = RUN_NUMBER)
TEMPLATE              = "/uboone/app/users/lmoralep/DM-MicroBooNE/DarkTridentGen/templates/template_parameter_uboone_root_pi0.dat"
FILETAG               = ""
TARFILE_NAME          = "local_install_dark_tridents.tar.gz"


def get_options():
    parser       = optparse.OptionParser(usage="usage: %prog [options]")
    grid_group   = optparse.OptionGroup(parser, "Grid Options")

    grid_group.add_option("--make_tar", default = DO_TAR, help = "Make new tarfile. Default = False")
    grid_group.add_option("--outdir", default = OUTDIR, help = "Output flux histograms location. Default = %default.")
    grid_group.add_option("--logdir", default = LOGDIR, help = "Output log file location. Default = %default.")
    grid_group.add_option("--n_jobs", default = N_JOBS, type=int, help = "Number of g4numi jobs. Default = %default.")
    grid_group.add_option("--run_number", default = RUN_NUMBER, type=int, help = "Tag on the end of outfiles. Doubles as random # seed. Default = %default.")
    
    #grid_group.add_option('--pot',default = POT, type=int, help="Number of protons on target to simulate. Default = %default.")
    grid_group.add_option('--filetag', default = FILETAG)
    
    #uboone_group   = optparse.OptionGroup(parser, "uBoone Options")
    
    bdnmc_group    = optparse.OptionGroup(parser, "BdNMC Options")
    bdnmc_group.add_option('--template', default = TEMPLATE, help='Specify template parameter file. Default = parameter_uboone_template_grid_pi0.dat')
    bdnmc_group.add_option('--mA', default = MA_DIR, type=float, help='Specify dark photon mass. Default = 0.05.')
    bdnmc_group.add_option('--ratio', default = RATIO, type=float, help='Specify dark photon mass to dark matter mass ratio. Default = 0.6.')
    bdnmc_group.add_option('--nevts', default = NEVTS, type=int, help='Specify number of events to generate. Default = 5000.')
    bdnmc_group.add_option('--alD', default = ALD, help='Specify value of alD. Default = 0.1')
    bdnmc_group.add_option('--dm_type', default = DM_TYPE, help='Specify the dark matter type, default: scalar')
    bdnmc_group.add_option('--decay_channel', default = DECAY_CHAN, help='Specify meson decay channel (pi0_decay, eta_decay, all). Default = pi0_decay.')
    bdnmc_group.add_option('--signal_channel', default = SIGNAL, help='Specify signal channel. Default = NCE_nucleon.')
    
    parser.add_option_group(grid_group)
    parser.add_option_group(bdnmc_group)
    #parser.add_option_group(old_group)
    #parser.add_option_group(uboone_group)

    options, remainder = parser.parse_args()

    print("\nPrinting config options to run with: \n")
    print('mA',                        options.mA)
    print('mX',                        options.mA * options.ratio)
    print('run',                       options.run_number)

    return options

def make_parfile(options):
  template_filename = options.template
  template_string   = open(template_filename, 'r').read()
  template          = string.Template(template_string)


  dm_mass_string = "{:.3f}".format(options.mA*options.ratio)
  dp_mass_string = "{:.3f}".format(options.mA)

  macro_string = template.safe_substitute(
      {
        'run':                str(options.run_number),
        'nevents':            str(options.nevts),
        'dp_mass':            dp_mass_string,#str(options.mA),
        'dm_mass':            dm_mass_string,#str(options.mA * options.ratio),
        'alD':                str(options.alD),
        'decay_type':         options.dm_type
      }
    )

  macro_name = "parameter_uboone_grid.dat"
  macro = open(macro_name, "w") 
  macro.write(macro_string)
  macro.close()

  return macro_name

def make_tarfile(output_filename):

    dmodes = ["fermion","scalar"]
    ratios = [0.6, 2.0]


    os.listdir(".")
    tar = tarfile.open(output_filename, "w:gz")
    #tar.add("parameter_uboone_grid.dat")
    tar.add("./mesons/pi0s.dat", arcname="pi0s.dat")
    tar.add("./mesons/etas.dat", arcname="etas.dat")
    tar.add("./BdNMC/bin/BDNMC")
    for i in os.listdir("./BdNMC/build"):
      tar.add("BdNMC/build/"+i)
    for i in os.listdir("./BdNMC/src"):
      tar.add("BdNMC/src/"+i)

    for d in dmodes:
      for ratio in ratios:

        if(ratio == 0.6):
            masses = [0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.1, 0.2, 0.3, 0.4]
        else: 
            masses = [0.010, 0.020, 0.030, 0.040, 0.050, 0.060, 0.065, 0.070, 0.075, 0.080, 0.085,
                      0.090, 0.095, 0.100, 0.105, 0.110, 0.115, 0.120, 0.125]

        for m in masses:
          if(ratio==0.6):
            tar.add("xsec/cross_section_{:.2f}_ratio_{:.2f}_{}_fix.root".format(m,ratio,d))
          else:
            tar.add("xsec/cross_section_{:.3f}_ratio_{:.2f}_{}_fix.root".format(m,ratio,d))
    tar.add("./GenExLight/evgen.exe", arcname="evgen.exe")
    tar.add("./grid/setup_evgen_grid.sh", arcname="setup_evgen_grid.sh")
    tar.close()



def main():

    print("Job submission")

    options       = get_options()
    parfile       = make_parfile(options)
    MA_DIR        = options.mA
    SIGNAL        = options.signal_channel
    decay_channel = options.decay_channel
    template      = options.template
    make_tar      = options.make_tar
    DM_TYPE       = options.dm_type

    # Create a run number directory                                                                                                                                                                                                                                               
    RUNDIR = "/pnfs/uboone/scratch/users/{USER}/DarkTridentGen/run_{RUN_NUMBER}/{sig}/{mA}/{DECAY_CHAN}/".format( USER = os.getenv("USER"), DECAY_CHAN = decay_channel, mA = MA_DIR, RUN_NUMBER = options.run_number, sig = SIGNAL)
    print(RUNDIR)

    if os.path.isdir(RUNDIR) == False:
        print(RUNDIR, " directory doen't exist, so creating...\n")
        os.makedirs(RUNDIR)

    # Create a output file directory                                                                                                                                                                                                                                              
    OUTDIR = "/pnfs/uboone/scratch/users/{USER}/DarkTridentGen/run_{RUN_NUMBER}/{sig}/{mA}/{DECAY_CHAN}/{dm}/files/".format( USER = os.getenv("USER"), DECAY_CHAN = decay_channel, mA = MA_DIR, RUN_NUMBER = options.run_number, sig = SIGNAL, dm=DM_TYPE)

    if os.path.isdir(OUTDIR) == False:
        print(OUTDIR, " directory doen't exist, so creating...\n")
        os.makedirs(OUTDIR)

    # Create a log file directory                                                                                                                                                                                                                                                 
    LOGDIR = "/pnfs/uboone/scratch/users/{USER}/DarkTridentGen/run_{RUN_NUMBER}/{sig}/{mA}/{DECAY_CHAN}/{dm}/log/".format( USER = os.getenv("USER"), DECAY_CHAN = decay_channel, mA = MA_DIR, RUN_NUMBER = options.run_number, sig = SIGNAL, dm=DM_TYPE)

    if os.path.isdir(LOGDIR) == False:
        print(LOGDIR, " directory doen't exist, so creating...\n")
        os.makedirs(LOGDIR)
    
    # Create a cache file directory  
    CACHE_PNFS_AREA = "/pnfs/uboone/scratch/users/{USER}/DarkTridentGen/run_{RUN_NUMBER}/{sig}/{mA}/{DECAY_CHAN}/{dm}/CACHE/".format(USER = os.getenv("USER"), DECAY_CHAN = decay_channel, mA = MA_DIR, RUN_NUMBER = options.run_number, sig = SIGNAL, dm=DM_TYPE)

    if os.path.isdir(CACHE_PNFS_AREA) == False:
      print(CACHE_PNFS_AREA, " directory doen't exist, so creating...\n")
      os.makedirs(CACHE_PNFS_AREA)

    # scratch /pnfs area from which to send tarfile to grid                                                                                                                                                                                                                       
    cache_folder = CACHE_PNFS_AREA + str(random.randint(10000,99999)) + "/"
    os.makedirs(cache_folder)

    if(make_tar):
      print("\nTarring up local area...")
      make_tarfile(TARFILE_NAME, options.mA, options.dm_type, options.ratio)  

    #always copy jobfile and tarball to cache area
    shutil.copy("./parameter_uboone_grid.dat",    cache_folder)
    shutil.copy(TARFILE_NAME,    cache_folder)
    shutil.copy("./grid/dark_tridents_job.sh", cache_folder)

    print("\nTarball of local area:", cache_folder + TARFILE_NAME)

    logfile = LOGDIR + "/dark_trident_{mA}_{RUN}_{TIME}_\$PROCESS.log".format(mA  = MA_DIR, RUN = options.run_number, TIME = time.strftime("%Y%m%d-%H%M%S"))

    print("\nOutput logfile(s):",logfile)

    submit_command = ("jobsub_submit {GRID} {MEMORY} {DISK} -N {NJOBS} -d DARKTRIDENT {OUTDIR} "
      "-G uboone "
      "-e RUN={RUN} "
      "-e MA={MASS:.2f} "
      "-e RATIO={RATIO:.2f} "
      "-e ALD={ALD} "
      "-e DM_TYPE={DMTYPE} "
      "-e NEVTS={NEVTS} "
      "-f {TARFILE} "
      "-f {PARFILE} "
      "-L {LOGFILE} "
      "file://{CACHE}/dark_tridents_job.sh".format(
      GRID       = ("--OS=SL7 "
                    "--resource-provides=usage_model=DEDICATED,OPPORTUNISTIC,OFFSITE "
                    "--role=Analysis "
                    "--expected-lifetime=36h "),
      DISK       = "--disk 4GB",
      MEMORY     = "--memory 4GB ",
      NJOBS      = options.n_jobs,
      OUTDIR     = OUTDIR,
      RUN        = options.run_number,
      MASS       = options.mA,
      RATIO      = options.ratio,
      ALD        = options.alD,
      DMTYPE     = options.dm_type,
      NEVTS      = options.nevts,
      TARFILE    = cache_folder + TARFILE_NAME,
      LOGFILE    = logfile,
      PARFILE    = cache_folder + "parameter_uboone_grid.dat",
      CACHE      = cache_folder)
  )

  #Ship it                                                                                                                                                                                                                                                                      
    print("\nSubmitting to grid:\n"+submit_command+"\n")
    status = subprocess.call(submit_command, shell=True)

if __name__ == "__main__":
    sys.exit(main())
    #ptions = get_options()
    #parfile = make_parfile(options)