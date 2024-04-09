#include "record_root.h"


using std::list;        using std::cout;   using std::endl;


//const double pi0_file_evts = 1525299.;
//const double eta_file_evts = 149190.;
const double g4numi_pot    = 5e5;

double meson_energy; 
double dm_weight, dm_energy, dm_px, dm_py, dm_pz, dm_origin_x, dm_origin_y, dm_origin_z, dm_origin_t0, vx, vy, vz, vt, L, L1, L2;
double dm_weight_other, dm_energy_other, dm_px_other, dm_py_other, dm_pz_other, dm_origin_x_other, dm_origin_y_other, dm_origin_z_other, dm_origin_t0_other, vx_other, vy_other, vz_other, vt_other, L_other, L1_other, L2_other;
int event_number;
int dm_id;
int dm_id_other;  

//POT variables
std::string channel_name, dm_mom;
double branching_ratio, ntrials, tot_pot;

//Model variables
double alphaD, dm_mass, dp_mass, epsilon;


// double electron variables
int electron_id;
double electron_energy, electron_px, electron_py, electron_pz, electron_origin_x, electron_origin_y, electron_origin_z, electron_origin_t0, electron_vx, electron_vy, electron_vz;

TTree* make_event_tree(){

    TTree* outtree = new TTree("event_tree", "Tree containing paticles and their parameters");

    outtree->Branch("event_number", &event_number);
    outtree->Branch("id", &dm_id);
    outtree->Branch("parent_energy",&meson_energy);
    outtree->Branch("dm_energy", &dm_energy);
    outtree->Branch("dm_weight", &dm_weight);
    outtree->Branch("dm_px", &dm_px);
    outtree->Branch("dm_py", &dm_py);
    outtree->Branch("dm_pz", &dm_pz);

    outtree->Branch("dm_origin_x", &dm_origin_x);
    outtree->Branch("dm_origin_y", &dm_origin_y);
    outtree->Branch("dm_origin_z", &dm_origin_z);
    outtree->Branch("dm_origin_t0", &dm_origin_t0);
    
    outtree->Branch("dm_vx", &vx);
    outtree->Branch("dm_vy", &vy);
    outtree->Branch("dm_vz", &vz);
    outtree->Branch("dm_vt", &vt);

    outtree->Branch("dm_L", &L);
    outtree->Branch("dm_L1", &L1);
    outtree->Branch("dm_L2", &L2);


    outtree->Branch("dmOther_id",&dm_id_other);
    outtree->Branch("dmOther_energy", &dm_energy_other);
    outtree->Branch("dmOther_weight", &dm_weight_other);
    outtree->Branch("dmOther_px", &dm_px_other);
    outtree->Branch("dmOther_py", &dm_py_other);
    outtree->Branch("dmOther_pz", &dm_pz_other);

    outtree->Branch("dm_origin_x_other", &dm_origin_x_other);
    outtree->Branch("dm_origin_y_other", &dm_origin_y_other);
    outtree->Branch("dm_origin_z_other", &dm_origin_z_other);
    outtree->Branch("dm_origin_t0_other", &dm_origin_t0_other);

    outtree->Branch("dmOther_vx", &vx_other);
    outtree->Branch("dmOther_vy", &vy_other);
    outtree->Branch("dmOther_vz", &vz_other);
    outtree->Branch("dmOther_vt", &vt_other);

    outtree->Branch("dmOther_L", &L_other);
    outtree->Branch("dmOther_L1", &L1_other);
    outtree->Branch("dmOther_L2", &L2_other);

    outtree->Branch("channel_name", &dm_mom);


    return outtree;

}

TTree* make_etree(){
    TTree* etree = new TTree("elecron_tree", "Tree containing electrons");
    // ELECTRON VARIABLES
    etree->Branch("electron_id", &electron_id);
    etree->Branch("electron_energy", &electron_energy);
    etree->Branch("electron_px", &electron_px);
    etree->Branch("electron_py", &electron_py);
    etree->Branch("electron_pz", &electron_pz);
    etree->Branch("electron_origin_x", &electron_origin_x);
    etree->Branch("electron_origin_y", &electron_origin_y);
    etree->Branch("electron_origin_z", &electron_origin_z);
    etree->Branch("electron_origin_t0", &electron_origin_t0);
    etree->Branch("electron_vx", &electron_vx);

    return etree;
}

TTree* make_pot_tree(){

    TTree* pot_tree = new TTree("pot_tree", "POT counts");

    pot_tree->Branch("channel_name", &channel_name);
    pot_tree->Branch("branching_ratio", &branching_ratio);
    pot_tree->Branch("ntrials", &ntrials);
    pot_tree->Branch("tot_pot", &tot_pot);

    return pot_tree;
}

TTree* make_model_tree(){

    TTree* model_tree = new TTree("model_tree", "POT counts");

    model_tree->Branch("dm_mass", &dm_mass);
    model_tree->Branch("dp_mass", &dp_mass);

    model_tree->Branch("alphaD", &alphaD);
    model_tree->Branch("epsilon", &epsilon);

    return model_tree;
}


void record_root(TTree *outtree, TTree* etree, list<Particle> &partlist, int nevent, bool isOther, std::string channel_name, std::shared_ptr<detector> det){

    bool dm_found = false;
    event_number = nevent;
    
    for(list<Particle>::iterator it = partlist.begin(); it != partlist.end(); it++){
        /* Filling with parent meson info */
        
        if(it->name == "pion" || it->name == "eta"){
            meson_energy = it->E; 
        }

        else if(it->name == "DM"){
            
            dm_mom    = channel_name;

            if(!isOther){

                dm_id = it->origin_id; 
                dm_weight = it->w;
                dm_energy = it->E;
                dm_px     = it->px;
                dm_py     = it->py;
                dm_pz     = it->pz;
           
                dm_origin_x     = it->origin_coords[0];
                dm_origin_y     = it->origin_coords[1];
                dm_origin_z     = it->origin_coords[2];
                dm_origin_t0    = it->origin_coords[3];

                vx  = it->end_coords[0];
                vy  = it->end_coords[1];
                vz  = it->end_coords[2];
                vt  = it->end_coords[3];

                L1        = it->crossing[0];
                L2        = it->crossing[1];
                L         = det->Ldet(*it);

                dm_found = true;

                

                
            }


            else{
                dm_id_other = it->origin_id;
                dm_weight_other = it->w;
                dm_energy_other = it->E;
                dm_px_other     = it->px;
                dm_py_other     = it->py;
                dm_pz_other     = it->pz;
  
                dm_origin_x_other     = it->origin_coords[0];
                dm_origin_y_other     = it->origin_coords[1];
                dm_origin_z_other     = it->origin_coords[2];
                dm_origin_t0_other    = it->origin_coords[3];

                vx_other     = it->end_coords[0];
                vy_other     = it->end_coords[1];
                vz_other     = it->end_coords[2];
                vt_other     = it->end_coords[3];

                L1_other     = it->crossing[0];
                L2_other     = it->crossing[1];
                L_other      = det->Ldet(*it);
            }

             outtree->Fill();

        }  else if(it->name == "electron"){
            //cout<<"Electron found"<<endl;
            electron_id = it->origin_id;
            electron_energy = it->E;
            electron_px = it->px;
            electron_py = it->py;
            electron_pz = it->pz;
            electron_origin_x = it->origin_coords[0];
            electron_origin_y = it->origin_coords[1];
            electron_origin_z = it->origin_coords[2];
            electron_origin_t0 = it->origin_coords[3];
            //electron_vx = it->end_coords[0];
            //electron_vy = it->end_coords[1];
            //electron_vz = it->end_coords[2];
            etree->Fill();

        }

       
        
    }
    
    
}

void record_pot(TTree *pot_tree, std::vector<std::shared_ptr<DMGenerator> > DMGen_list, std::vector<double> n_mesons){



    for(int i = 0; i< DMGen_list.size(); ++i){

        channel_name    = DMGen_list[i]->Channel_Name();

        branching_ratio = DMGen_list[i]->BranchingRatio();
        ntrials         = DMGen_list[i]->NTrials();

        double file_evts(-1);
        if(channel_name == "pi0_decay"){
            std::cout << "Number of files used for Pi0 POT: " << n_mesons[i] << std::endl;    
             file_evts = n_mesons[i];
        }  
        else if(channel_name == "eta_decay"){
            std::cout << "Number of files used for Eta POT: " << n_mesons[i] << std::endl; 
            file_evts = n_mesons[i];
        }

        tot_pot = (g4numi_pot * ntrials / file_evts) / branching_ratio;

        pot_tree->Fill();

    }
    


}

void record_model(TTree *model_tree, double alD, double kappa, double mdm, double mv){

    alphaD = alD;
    epsilon = kappa;
    dm_mass = mdm;
    dp_mass = mv;
    
    model_tree->Fill();

}
