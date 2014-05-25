/*
 * opencog/learning/moses/moses/types.cc
 *
 * Copyright (C) 2002-2008 Novamente LLC
 * All Rights Reserved
 *
 * Written by Moshe Looks
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include <sstream>

#include <boost/functional/hash.hpp>

#include "types.h"
#include "complexity.h"


namespace opencog { namespace moses {

using namespace std;

demeID_t::demeID_t(unsigned expansion)
    : string(to_string(expansion)) {}
demeID_t::demeID_t(unsigned expansion, unsigned breadth_first)
    : string(to_string(expansion) + "." + to_string(breadth_first)) {}

size_t scored_combo_tree_hash::operator()(const scored_combo_tree& sct) const
{
    size_t hash = 0;
    const combo::combo_tree& tr = sct.get_tree();
    for (combo::vertex vtx : tr) {
        boost::hash_combine(hash, combo::hash_value(vtx));
    }
    return hash;
}

bool scored_combo_tree_equal::operator()(const scored_combo_tree& tr1,
                                         const scored_combo_tree& tr2) const
{
    return tr1.get_tree() == tr2.get_tree();
}

bool scored_combo_tree_greater::operator()(const scored_combo_tree& bs_tr1,
                                           const scored_combo_tree& bs_tr2) const
{
    const composite_score csc1 = bs_tr1.get_composite_score();
    const composite_score csc2 = bs_tr2.get_composite_score();

    bool gt = (csc1 > csc2);
    if (gt) return true;

    bool lt = (csc1 < csc2);
    if (lt) return false;

    // If we are here, then they are equal.  We are desperate to break
    // a tie, because otherwise, the scored_combo_tree_set will discard
    // anything that compares equal, and we really don't want that.
    score_t sc1 = get_score(csc1);
    score_t sc2 = get_score(csc2);

    gt = (sc1 > sc2);
    if (gt) return true;

    lt = (sc1 < sc2);
    if (lt) return false;

    // Arghh, still tied!  The above already used complexity to break
    // the tie.  We're grasping at straws, here.
    return size_tree_order<vertex>()(bs_tr1.get_tree(), bs_tr2.get_tree());
}

// the empty composite_score ctor returns the worst composite score
const composite_score worst_composite_score = composite_score();

composite_score::composite_score()
    : score(very_worst_score), complexity(least_complexity),
      complexity_penalty(0.0), diversity_penalty(0.0),
      penalized_score(very_worst_score)
{}

composite_score& composite_score::operator=(const composite_score &r)
{
    score = r.score;
    complexity = r.complexity;
    penalized_score = r.penalized_score;
    complexity_penalty = r.complexity_penalty;
    diversity_penalty = r.diversity_penalty;
    multiply_diversity = r.multiply_diversity;
    return *this;
}

bool composite_score::operator<(const composite_score &r) const
{
    score_t lef = penalized_score;
    score_t rig = r.penalized_score;

    if (isnan(lef))
        return !isnan(rig);
    else
        return (lef < rig)
                // Note: I've tried to see if the addition below
                // increases the performance when there is no
                // complexity penalty and it doesn't, over 100 runs
                // solving 3-parity is actually .5s slower (which
                // probably has no statistical significance given it
                // takes 13s in average). But I don't want to conclude
                // too fast, it could have an impact on other
                // problems.
                or (lef == rig and complexity > r.complexity);
}

// Check for equality, to within floating-point error.
bool composite_score::operator==(const composite_score &r) const
{
    // score_t and complexity_t are both defacto floats.
    // equality holds if they are equal within about 7 decimal places.
    // Note: this check is used in iostream_bscored_combo_treeUTest
    // and a simple equality test will fail on some cpu/os combos.
    #define FLOAT_EPS 1.0e-7
    #define CHK_EQ(NAME)  \
        ((NAME == r.get_##NAME()) || /* Try this cheap test first */ \
         (fabs(NAME) > 0.0f) ?  \
            (fabs(r.get_##NAME() / NAME - 1.0f) < FLOAT_EPS) : \
            (fabs(NAME - r.get_##NAME()) < FLOAT_EPS))
    return
        CHK_EQ(score)
        && CHK_EQ(complexity)
        && CHK_EQ(complexity_penalty)
        && CHK_EQ(diversity_penalty)
        && CHK_EQ(penalized_score)
        ;
}

///////////////////////////
// convenience accessors //
///////////////////////////


const composite_penalized_bscore& cget_composite_penalized_bscore(const scored_combo_tree& pbst)
{
    return pbst.cpbscored.first;
}

composite_penalized_bscore& get_composite_penalized_bscore(scored_combo_tree& pbst)
{
    return pbst.cpbscored.first;
}

const cpbscore_demeID& get_cpbscore_demeID(const scored_combo_tree& pbst)
{
    return pbst.cpbscored;
}

cpbscore_demeID& get_cpbscore_demeID(scored_combo_tree& pbst)
{
    return pbst.cpbscored;
}

demeID_t get_demeID(const scored_combo_tree& pbst)
{
    return pbst.cpbscored.second;
}

score_t get_penalized_score(const composite_score& sc)
{
   return sc.get_penalized_score();
}

composite_score& xget_composite_score(composite_penalized_bscore& ctbs)
{
    return ctbs.second;
}

const composite_score& xget_composite_score(const composite_penalized_bscore& ctbs)
{
    return ctbs.second;
}

score_t get_penalized_score(const composite_penalized_bscore& cpb)
{
    return get_penalized_score(xget_composite_score(cpb));
}

score_t get_penalized_score(const scored_combo_tree& sct)
{
    return get_penalized_score(sct.get_composite_score());
}

composite_score& get_composite_score(composite_penalized_bscore& ctbs)
{
    return ctbs.second;
}

score_t get_score(const composite_score& ts)
{
    return ts.get_score();
}

score_t get_score(const composite_penalized_bscore& ts)
{
    return get_score(ts.second);
}

score_t get_score(const scored_combo_tree& bst)
{
    return get_score(cget_composite_penalized_bscore(bst));
}

complexity_t get_complexity(const composite_score& ts)
{
    return ts.get_complexity();
}

complexity_t get_complexity(const composite_penalized_bscore& ts)
{
    return get_complexity(ts.second);
}

complexity_t get_complexity(const scored_combo_tree& bst)
{
    return get_complexity(cget_composite_penalized_bscore(bst));
}

score_t get_complexity_penalty(const composite_score& ts)
{
    return ts.get_complexity_penalty();
}

score_t get_complexity_penalty(const composite_penalized_bscore& ts)
{
    return get_complexity_penalty(ts.second);
}

score_t get_complexity_penalty(const scored_combo_tree& bst)
{
    return get_complexity_penalty(cget_composite_penalized_bscore(bst));
}

score_t get_diversity_penalty(const composite_score& ts)
{
    return ts.get_diversity_penalty();
}

score_t get_diversity_penalty(const composite_penalized_bscore& ts)
{
    return get_diversity_penalty(ts.second);
}

score_t get_diversity_penalty(const scored_combo_tree& bst)
{
    return get_diversity_penalty(cget_composite_penalized_bscore(bst));
}

score_t get_penalty(const composite_score& ts)
{
    return ts.get_penalty();
}

score_t get_penalty(const composite_penalized_bscore& ts)
{
    return get_penalty(ts.second);
}

score_t get_penalty(const scored_combo_tree& bst)
{
    return get_penalty(cget_composite_penalized_bscore(bst));
}

const penalized_bscore& get_pbscore(const composite_penalized_bscore& ts)
{
    return ts.first;
}

penalized_bscore& get_pbscore(composite_penalized_bscore& ts)
{
    return ts.first;
}

const penalized_bscore& get_pbscore(const scored_combo_tree& bst)
{
    return get_pbscore(cget_composite_penalized_bscore(bst));
}

penalized_bscore& get_pbscore(scored_combo_tree& bst)
{
    return get_pbscore(get_composite_penalized_bscore(bst));
}

const behavioral_score& get_bscore(const penalized_bscore& pbs)
{
    return pbs.first;
}

const behavioral_score& get_bscore(const composite_penalized_bscore& cbs)
{
    return get_bscore(cbs.first);
}

const behavioral_score& get_bscore(const scored_combo_tree& bst)
{
    return get_bscore(cget_composite_penalized_bscore(bst));
}

} // ~namespace moses
} // ~namespace opencog
