#include <set>
#include <string>
#include <algorithm>
#include <iostream>

#include <LADSUtil/mt19937ar.h>
#include <LADSUtil/numeric.h>

#include <ComboReduct/combo/vertex.h>
#include <ComboReduct/ant_combo_vocabulary/ant_combo_vocabulary.h>
#include <ComboReduct/reduct/reduct.h>

#include "MosesEda/moses/moses.h"
#include "MosesEda/moses/optimization.h"
#include "MosesEda/moses/scoring_functions.h"
#include "MosesEda/moses/ant_scoring.h"
#include <boost/lexical_cast.hpp>

using namespace moses;
using namespace reduct;
using namespace boost;
using namespace ant_combo;
using namespace std;
using namespace LADSUtil;


//typedef std::set<combo::vertex> operator_set;
//typedef operator_set::iterator operator_set_it;  

//typedef std::set<combo::combo_tree, LADSUtil::size_tree_order<combo::vertex> > 
//    combo_tree_ns_set;

int main(int argc,char** argv) { 
  int max_evals,rand_seed;
  try {
    if (argc!=3)
      throw "foo";
    rand_seed=lexical_cast<int>(argv[1]);
    max_evals=atoi(argv[2]);
  } catch (...) {
    cerr << "usage: " << argv[0] << " seed maxevals" << endl;
    exit(1);
  }

  type_tree tt(id::lambda_type);
  tt.append_children(tt.begin(),id::action_result_type,1);

  ant_score scorer;
  ant_bscore bscorer;

  LADSUtil::MT19937RandGen rng(rand_seed);

  operator_set os;
  combo_tree_ns_set perceptions;
  combo_tree_ns_set actions;

  actions.insert(combo_tree(instance(id::turn_left)));
  actions.insert(combo_tree(instance(id::turn_right)));
  actions.insert(combo_tree(instance(id::move_forward)));

  perceptions.insert(combo_tree(instance(id::is_food_ahead)));

  ordered_programs op;

/*
  metapopulation<ant_score,ant_bscore,iterative_hillclimbing> 
    metapop(rng,combo_tree(id::sequential_and),tt,action_reduction(),
	    scorer,
	    bscorer,
            iterative_hillclimbing(rng));
  //had to put namespace moses otherwise gcc-4.1 complains that it is ambiguous
  moses::moses(metapop,max_evals,0,&os,&perceptions,&actions,op);
*/

/*
  metapopulation<ant_score,ant_bscore,univariate_optimization> 
    metapop(rng,combo_tree(id::sequential_and),tt,action_reduction(),
	    scorer,
	    bscorer,
            univariate_optimization(rng));
  moses::moses(metapop,max_evals,0,&os,&perceptions,&actions,op);
*/


 metapopulation<ant_score,ant_bscore,sliced_iterative_hillclimbing> 
    metapop(rng,combo_tree(id::sequential_and),tt,action_reduction(),
	    scorer,
	    bscorer,
            sliced_iterative_hillclimbing(rng));
  moses::moses_sliced(metapop,max_evals,0,&os,&perceptions,&actions,op);

	
}

