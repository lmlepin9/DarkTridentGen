#include <iostream>
#include <string>
#include <vector> 
#include <sys/stat.h>
#include "TTree.h"
#include "TChain.h"



double get_total_events(const std::string &base_dir ,const double m,const double target_pot){

    std::vector<std::string> decay_modes = {"pi0_decay", "eta_decay"};
    double total_final_events{0.};

    for(std::string dchannel: decay_modes){
        //std::cout << dchannel << std::endl;
        double total_pot{0.};
        double temp_pot{0.};
        int n_entries{0}; 
        char path_to_file[180];

        if(m >=0.1){
                std::string temp_file = base_dir + "hepevt_events_%s_ma_%.1f.root";
                sprintf(path_to_file,temp_file.c_str(),dchannel.c_str(),m);

            }
        else{
                std::string temp_file = base_dir + "hepevt_events_%s_ma_%.2f.root";
                sprintf(path_to_file,temp_file.c_str(),dchannel.c_str(),m);
        }
        std::cout << path_to_file << std::endl;

        TFile *f = new TFile(path_to_file,"READ");
        TTree *tree = new TTree();
        tree = (TTree*) f->Get("event_tree");
        tree->SetBranchAddress("final_weight",&temp_pot);


        std::cout << tree->GetEntries() << std::endl;
        std::cout << temp_pot << std::endl;
        for(int i = 0; i < tree->GetEntries(); i++){
            tree->GetEntry(i);
            if(temp_pot > 0.){total_pot+=temp_pot; n_entries+=1; }
            else{continue;}
        }


        std::cout << total_pot << std::endl;
        std::cout << n_entries << std::endl;
        double total_events = n_entries*(target_pot/(total_pot));
        total_final_events += total_events;
        std::cout << "Total events " << dchannel << ": " <<total_events << std::endl;
        tree->Delete();
        f->Close();
        f->Delete();
    }


    return total_final_events;
}


int main(){

    double target_pot{1e21};
    std::vector<std::string> decay_modes = {"pi0_decay", "eta_decay"};
    std::vector<double> masses =  {0.09};//{0.01, 0.02,  0.03,  0.04,  0.05,  0.06,  0.07,  0.08,  0.09,  0.1,  0.2,  0.3,  0.4};
    for(double m: masses){
        double total_per_mass = 0.; 
        for(std::string dm: decay_modes){
            double total_pot = 0.; 
            char path_to_file[180];
            double temp_pot = 0.;
            int n_entries = 0; 
            if(m >=0.1){
                sprintf(path_to_file,"/pnfs/uboone/persistent/users/lmoralep/DarkTridentGen/fermion_ratio_0.6/hepevt_events_%s_ma_%.1f.root",dm.c_str(),m);

            }
            else{
                sprintf(path_to_file,"/pnfs/uboone/persistent/users/lmoralep/DarkTridentGen/fermion_ratio_0.6/hepevt_events_%s_ma_%.2f.root",dm.c_str(),m);
            }
            std::cout << "Trying to open file: " << path_to_file << std::endl;
            struct stat sb; 
            if(stat(path_to_file,&sb) != 0){std::cout << path_to_file << " doesn't exist..." << std::endl; continue;}
            TFile *f = new TFile(path_to_file,"READ");
            TTree *tree = new TTree();
            tree = (TTree*) f->Get("event_tree");
            tree->SetBranchAddress("final_weight",&temp_pot);
            for(int i = 0; i < tree->GetEntries(); i++){
                tree->GetEntry(i);
                if(temp_pot > 0. && !std::isinf(temp_pot)){total_pot+=temp_pot; n_entries+=1; }
                else{continue;}

            }
            std::cout << total_pot << std::endl;
            double total_events = n_entries*(target_pot/(total_pot));
            total_per_mass += total_events;
            std::cout << "Total events " << dm << ": " <<total_events << std::endl;
            delete tree; 
            delete f;
            
        }
        std::cout << "Total events for mass " << m << ": " << total_per_mass << std::endl;
        
    }
   


    return 0; 
}