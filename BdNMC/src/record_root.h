#ifndef GUARD_record_root_h
#define GUARD_record_root_h

#include <list>
#include <vector>

#include <TTree.h>

#include "Particle.h"
#include "DMgenerator.h"
#include "detector.h"

void record_root(TTree *outtree, TTree *etree, std::list<Particle> &, int nevent, bool isOther, std::string channel_name, std::shared_ptr<detector> det);
void record_model(TTree *model_tree, double alD, double kappa, double mdm, double mv);
void record_pot(TTree *pot_tree, std::vector<std::shared_ptr<DMGenerator> > DMGen_list, std::vector<double> n_mesons);

TTree* make_event_tree();
TTree* make_pot_tree();
TTree* make_model_tree();
TTree* make_etree();



#endif
