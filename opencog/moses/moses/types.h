#ifndef _MOSES_TYPES_H
#define _MOSES_TYPES_H

#include <LADSUtil/functional.h>
#include <LADSUtil/foreach.h>

#include <ComboReduct/combo/vertex.h>
#include <ComboReduct/combo/complexity.h>

namespace moses {
  
  //basic types
  typedef double score_t;
  typedef combo::complexity_t complexity_t;

  typedef std::pair<score_t,complexity_t> tree_score;
  typedef LADSUtil::tagged_item<combo::combo_tree,tree_score> scored_tree;

  typedef std::vector<float> behavioral_score;

  typedef LADSUtil::tagged_item<behavioral_score,tree_score> behavioral_tree_score;
  typedef LADSUtil::tagged_item<combo::combo_tree,
			    behavioral_tree_score> behavioral_scored_tree;
  
  extern const tree_score worst_possible_score;

  //convenience accessors
  inline const combo::combo_tree& get_tree(const scored_tree& st) { 
    return st.first; 
  }
  inline const combo::combo_tree& get_tree(const behavioral_scored_tree& bst) { 
    return bst.first;
  }

  inline complexity_t get_complexity(const tree_score& ts) { 
    return ts.second; 
  }
  inline complexity_t get_complexity(const behavioral_tree_score& ts) { 
    return get_complexity(ts.second);
  }
  inline complexity_t get_complexity(const behavioral_scored_tree& bst) { 
    return get_complexity(bst.second);
  }
  inline complexity_t get_complexity(const scored_tree& st) { 
    return get_complexity(st.second);
  }

  inline score_t get_score(const tree_score& ts) { 
    return ts.first;
  }
  inline score_t get_score(const behavioral_tree_score& ts) { 
    return get_score(ts.second);
  }
  inline score_t get_score(const behavioral_scored_tree& bst) { 
    return get_score(bst.second);
  }
  inline score_t get_score(const scored_tree& st) { 
    return get_score(st.second);
  }

}

inline std::ostream& operator<<(std::ostream& out,const moses::tree_score& ts) {
  return (out << "[score=" << ts.first << ", complexity=" << -ts.second << "]");
}
inline std::ostream& operator<<(std::ostream& out,
				const moses::behavioral_tree_score& s) {
  out << "[ ";
  foreach (float f,s.first)
    out << f << " ";
  out << "], " << s.second;
  return out;
}

#endif
