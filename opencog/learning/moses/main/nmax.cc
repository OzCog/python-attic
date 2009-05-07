/*
 * opencog/learning/moses/main/nmax.cc
 *
 * Copyright (C) 2002-2008 Novamente LLC
 * All Rights Reserved
 *
 * Written by Predrag Janicic
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
#include "edaopt.h"
#include <opencog/util/mt19937ar.h>

int main(int argc,char** argv) { 
    opencog::cassert(TRACE_INFO, argc==6);
    optargs args(argc,argv);
    int n=lexical_cast<int>(argv[5]);
    cout_log_best_and_gen logger;
    
    opencog::MT19937RandGen rng(args.rand_seed);

    field_set fs(field_set::disc_spec(n),args.length); //all n-arry
    instance_set<int> population(args.popsize,fs);
    foreach(instance& inst,population)
        generate(fs.begin_disc(inst), fs.end_disc(inst),
                 bind(&opencog::RandGen::randint, ref(rng), n));

    optimize(population,args.n_select,args.n_generate,args.max_gens,n_max(fs),
             terminate_if_gte<int>((n-1)*args.length),
             tournament_selection(2,rng),
             univariate(),local_structure_probs_learning(),
             replace_the_worst(),logger,rng);
}
