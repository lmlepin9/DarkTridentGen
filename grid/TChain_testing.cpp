#include <iostream>
#include <string>
#include <vector> 
#include <sys/stat.h>
#include "TTree.h"
#include "TChain.h"


void events_ratio_06(const double mass_point, const std::string dm_type){

    std::string base_dir;

    if(mass_point >= 0.1){
        base_dir = "/pnfs/uboone/scratch/users/lmoralep/DarkTridentGen/run_90/16-03-2023/%.1f/%s/%s/files/";
    }

    else{
        base_dir = "/pnfs/uboone/scratch/users/lmoralep/DarkTridentGen/run_90/16-03-2023/%.2f/%s/%s/files/";
    }
    const std::vector<std::string> decay_types = {"pi0_decay","eta_decay"};
    const int n_files{50};

    double final_events{0.};
    for(std::string dchannel: decay_types){

        if(mass_point >= 0.1 && dchannel == "pi0_decay"){
            continue;
        }


        double temp_pot{0.};
        double total_pot{0.};
        TChain *ch = new TChain("event_tree");
        ch->SetBranchAddress("final_weight",&temp_pot);
        for(int i = 0; i <= n_files; i++){
            char path_to_file[280];
            std::string temp_file;

            if(mass_point >= 0.1){
                temp_file = base_dir + "hepevt_%.1f_%s_%d.root";
            }

            else{
                temp_file = base_dir + "hepevt_%.2f_%s_%d.root";
            }
    
            sprintf(path_to_file, temp_file.c_str(),mass_point,dchannel.c_str(),dm_type.c_str(),mass_point,dm_type.c_str(),i);
            ch->Add(path_to_file);
        }

        for(int i = 0; i < ch->GetEntries(); i++){
            ch->GetEntry(i);
            total_pot+=temp_pot;
        }

        std::cout << "Total events in " << dchannel << ": " << ch->GetEntries()*(1e21)/(total_pot) << std::endl;
        final_events+=ch->GetEntries()*(1e21)/(total_pot);

    }
    std::cout << "Total events for this mass point " << mass_point << ": " << final_events << std::endl;


}



// FUNCTION FOR MASS RATIO: 2.0 

void events_ratio_2(const double mass_point, const std::string dm_type){

    std::string base_dir;

    if(mass_point == 0.1){
        base_dir = "/pnfs/uboone/scratch/users/lmoralep/DarkTridentGen/run_80/16-03-2023/%.1f/%s/%s/files/";
    }

    else{
        base_dir = "/pnfs/uboone/scratch/users/lmoralep/DarkTridentGen/run_80/16-03-2023/%.2f/%s/%s/files/";
    }
    const std::vector<std::string> decay_types = {"pi0_decay","eta_decay"};
    const int n_files{50};

    double final_events{0.};
    for(std::string dchannel: decay_types){

        if(mass_point >= 0.04 && dchannel == "pi0_decay"){
            continue;
        }


        double temp_pot{0.};
        double total_pot{0.};
        TChain *ch = new TChain("event_tree");
        ch->SetBranchAddress("final_weight",&temp_pot);
        for(int i = 0; i <= n_files; i++){
            char path_to_file[280];
            std::string temp_file;

            if(mass_point == 0.1){
                temp_file = base_dir + "hepevt_%.1f_%s_%d.root";
            }

            else{
                temp_file = base_dir + "hepevt_%.2f_%s_%d.root";
            }
    
            sprintf(path_to_file, temp_file.c_str(),mass_point,dchannel.c_str(),dm_type.c_str(),mass_point,dm_type.c_str(),i);
            ch->Add(path_to_file);
        }

        for(int i = 0; i < ch->GetEntries(); i++){
            ch->GetEntry(i);
            total_pot+=temp_pot;
        }

        std::cout << "Total events in " << dchannel << ": " << ch->GetEntries()*(1e21)/(total_pot) << std::endl;
        final_events+=ch->GetEntries()*(1e21)/(total_pot);

    }
    std::cout << "Total events for this mass point " << mass_point << ": " << final_events << std::endl;


}





void mass_loop(){
    std::vector<double> mass_ratio_06 = {0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.1, 0.2, 0.3, 0.4};
    std::vector<double> mass_ratio_2 = {0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.1, 0.11, 0.12, 0.13};

    for(double m: mass_ratio_2){
        events_ratio_2(m,"fermion");
    }
}