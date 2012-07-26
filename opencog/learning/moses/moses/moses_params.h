/*
 * opencog/learning/moses/moses/moses_params.h
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
#ifndef _MOSES_MOSES_PARAMS_H
#define _MOSES_MOSES_PARAMS_H

#include <map>
#include <boost/program_options/variables_map.hpp>

namespace opencog {
namespace moses {

/// A map between hostname and number of jobs allocated.
typedef std::map<string, unsigned> jobs_t;

/**
 * parameters to decide how to run moses
 */
struct moses_parameters
{
    moses_parameters(const boost::program_options::variables_map& _vm =
                           boost::program_options::variables_map(),
                     const jobs_t& _jobs = jobs_t(),
                     bool _local = true,
                     int _max_evals = 10000,
                     int _max_gens = -1,
                     score_t _max_score = 0)
        : local(_local), jobs(_jobs), vm(_vm),
          max_evals(_max_evals), max_gens(_max_gens), max_score(_max_score)
    {}

    // Distributed solver control.
    bool local;
    const jobs_t& jobs;
    const boost::program_options::variables_map& vm;

    // total maximun number of evals
    int max_evals;
    // the max number of demes to create and optimize, if negative,
    // then no limit
    int max_gens;
    // the max score
    score_t max_score;
};


} // ~namespace moses
} // ~namespace opencog

#endif
