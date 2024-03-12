import os, optparse, random, shutil, tarfile, sys
import subprocess, string
import time

def make_tarfile(output_filename):
    os.listdir(".")
    tar = tarfile.open(output_filename, "w:gz")
    tar.add("./mesons/pi0s.dat", arcname="pi0s.dat")
    tar.add("./mesons/etas.dat", arcname="etas.dat")
    tar.add("./mesons/rhc_pi0s.dat", arcname="rhc_pi0s.dat")
    tar.add("./mesons/rhc_etas.dat", arcname="rhc_etas.dat")
    tar.add("./BdNMC/bin/BDNMC")
    for i in os.listdir("./BdNMC/build"):
      tar.add("BdNMC/build/"+i)
    for i in os.listdir("./BdNMC/src"):
      tar.add("BdNMC/src/"+i)
    for i in os.listdir("./xsec"):
        tar.add("./xsec/"+i)
    tar.add("./GenExLight/evgen.exe", arcname="evgen.exe")
    tar.close()


make_tarfile("dark_trident_generator_4_grid_V2.tar.gz")