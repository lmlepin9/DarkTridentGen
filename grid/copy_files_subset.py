#!/usr/bin/env python
import sys 
import os 
import glob 


N_TOTAL_FILES = 200 

def copy_files_subset(input_dir, output_dir):
    i_file = 0 
    for dir_name in os.listdir(input_dir):
        print(dir_name/)
        if(i_file == N_TOTAL_FILES):
            break 




if __name__ == "__main__":
    copy_files_subset(sys.argv[1], sys.argv[2])