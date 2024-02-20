#!/bin/bash

echo "Setting DarkTridentGen variables" 

cd "$(dirname -- "$0")"
export DTG_TOP_DIR="$(pwd)"
DTG_TOP_DIR_VAR=echo $DTG_TOP_DIR
#echo "Top directory: $DTG_TOP_DIR"

if [ -d "$DTG_TOP_DIR/neutral_meson_flux" ]; then 
    echo "Neutral meson flux files already exist, skipping this part..."
else 
    echo "Creating flux directory and downloading files..." 
    mkdir -p neutral_meson_flux
    wget https://cernbox.cern.ch/remote.php/dav/public-files/U4uy7tRbj4gYYmo/etas.dat -P $DTG_TOP_DIR/neutral_meson_flux/
    wget https://cernbox.cern.ch/remote.php/dav/public-files/U4uy7tRbj4gYYmo/pi0s.dat -P $DTG_TOP_DIR/neutral_meson_flux/
    wget https://cernbox.cern.ch/remote.php/dav/public-files/U4uy7tRbj4gYYmo/rhc_etas.dat -P $DTG_TOP_DIR/neutral_meson_flux/
    wget https://cernbox.cern.ch/remote.php/dav/public-files/U4uy7tRbj4gYYmo/rhc_pi0s.dat -P $DTG_TOP_DIR/neutral_meson_flux/

fi

if [ -d "$DTG_TOP_DIR/output_files" ]; then
    echo "Output directory already exists, adding to environment..."
    export DTG_OUTPUT_DIR=$DTG_TOP_DIR/output_files
else
    echo "Creating output directory and adding to environment..."
    mkdir -p $DTG_TOP_DIR/output_files 
    export DTG_OUTPUT_DIR=$DTG_TOP_DIR/output_files
fi




