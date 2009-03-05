#ifndef _MOSES_BUILD_KNOBS_H
#define _MOSES_BUILD_KNOBS_H

#include <boost/utility.hpp>
#include "MosesEda/moses/knob_mapper.h"
#include "MosesEda/moses/using.h"
#include "MosesEda/eda/field_set.h"

#include <ComboReduct/combo/type_tree.h>
#include <ComboReduct/combo/action.h>
#include <ComboReduct/combo/perception.h>

using namespace combo;

typedef std::set<combo::vertex> operator_set;
typedef operator_set::iterator operator_set_it;  

typedef std::set<combo::combo_tree, LADSUtil::size_tree_order<combo::vertex> > 
    combo_tree_ns_set;
typedef combo_tree_ns_set::iterator combo_tree_ns_set_it;



namespace moses {

  //need to call a generator method... (dispatched based on type

  struct build_knobs : boost::noncopyable {

    // Optional arguments used only for Petbrain and actions
    build_knobs(LADSUtil::RandGen& rng, combo_tree& exemplar,const combo::type_tree& t,knob_mapper& mapper,
                const operator_set* os=NULL,
                const combo_tree_ns_set* perceptions=NULL,
                const combo_tree_ns_set* actions=NULL,
		contin_t step_size=1.0,contin_t expansion=2.0,
		eda::field_set::arity_t depth=4);

    void build_logical(combo_tree::iterator it);
    void build_action(combo_tree::iterator it);
    void build_contin(combo_tree::iterator it);
  protected:
    LADSUtil::RandGen& rng;
    combo_tree& _exemplar;
    combo::type_tree _type;
    knob_mapper& _mapper;
    int _arity;
    contin_t _step_size,_expansion;
    eda::field_set::arity_t _depth;

    const operator_set* _os;
    const combo_tree_ns_set* _perceptions;
    const combo_tree_ns_set* _actions;

    void logical_canonize(combo_tree::iterator);
    void add_logical_knobs(combo_tree::iterator it,bool add_if_in_exemplar=true);
    void sample_logical_perms(combo_tree::iterator it,vector<combo_tree>& perms);
    void logical_probe(const combo_tree& tr,combo_tree::iterator it, bool add_if_in_exemplar);
    void logical_cleanup();
    bool disc_probe(combo_tree::iterator parent,disc_knob_base& kb);

    void action_canonize(combo_tree::iterator);
    void add_action_knobs(combo_tree::iterator it,bool add_if_in_exemplar=true);
    void add_simple_action_knobs(combo_tree::iterator it,bool add_if_in_exemplar=true);
    void sample_action_perms(combo_tree::iterator it,vector<combo_tree>& perms);
    void simple_action_probe(combo_tree::iterator it, bool add_if_in_exemplar);
    void action_probe(/*const combo_tree& tr*/vector<combo_tree>& perms,combo_tree::iterator it, bool add_if_in_exemplar);
    void action_cleanup();

    void contin_canonize(combo_tree::iterator);
    void canonize_div(combo_tree::iterator it);
    void add_constant_child(combo_tree::iterator it,contin_t v);
    combo_tree::iterator canonize_times(combo_tree::iterator it);
    void linear_canonize(combo_tree::iterator it);
    void rec_canonize(combo_tree::iterator it);
    void append_linear_combination(combo_tree::iterator it);
    combo_tree::iterator mult_add(combo_tree::iterator it,const vertex& v);
  };

} //~namespace moses

#endif
