/** eval-diversity.cc --- 
 *
 * Copyright (C) 2012 OpenCog Foundation
 *
 * Author: Nil Geisweiller <ngeiswei@gmail.com>
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

/**
 * Tool that loads several moses outputs or tables and compute the
 * diversity of the pool of candidates.
 */

#include <boost/program_options.hpp>

#include <opencog/util/oc_assert.h>
#include <opencog/util/iostreamContainer.h>
#include <opencog/comboreduct/combo/table_io.h>
#include <opencog/learning/moses/moses/types.h>

#include "eval-diversity.h"

using namespace std;
using namespace boost::program_options;
using namespace opencog;
using namespace moses;

// given the sequence of distances write the results
void write_results(const eval_diversity_params& edp, const vector<float>& dsts) {
    if (edp.output_file.empty())
        ostream_results(cout, edp, dsts);
    else {
        ofstream out(edp.output_file.c_str());
        ostream_results(out, edp, dsts);
    }
}

int main(int argc, char** argv) {
    eval_diversity_params edp;
    
    // Declare the supported options.
    options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Produce help message.\n")

        ("input-file,i", value<vector<string>>(&edp.input_files),
         "DSV file containing a target feature (indicated by option -f)"
         "used to compute the distance between the other files. Can be used"
         "several times to enter several input files. "
         "The distance will be computed between each target across all files.\n")

        ("target-feature,u", value<string>(&edp.target_feature),
         "Name of the target feature to load.\n")

        ("moses-file,m", value<vector<string>>(&edp.moses_files),
         "File containing the candidates as output by moses. The output "
         "must contain the bscores (see option -t or moses). "
         "Can be used several times for several files.\n")
        
        ("output-file,o", value<string>(&edp.output_file),
         "File to write the results. If none is given it write on the stdout.\n")

        ("display-stats", value<bool>(&edp.display_stats)->default_value(true),
         "Display statistics.\n")

        ("display-values", value<bool>(&edp.display_values)->default_value(false),
         "Display the actually values (distances).\n")

        ("diversity-p-norm",
         value<float>(&edp.diversity_p_norm)->default_value(2.0),
         "Set the parameter of the p-norm used to compute the distance between "
         "behavioral scores used for the diversity penalty. A value of 1.0 "
         "correspond to the Manhatan distance. A value of 2.0 corresponds to "
         "the Euclidean distance. A value of 0.0 or less correspond to the "
         "max component-wise. Any other value corresponds to the general case.\n")

        ;

        variables_map vm;
    try {
        store(parse_command_line(argc, argv, desc), vm);
    }
    catch (error& e) {
        OC_ASSERT(false, "Fatal error: invalid or duplicated argument:\n\t%s\n",
                  e.what());
    }
    notify(vm);

    if (vm.count("help") || argc == 1) {
        cout << desc << endl;
        return 1;
    }

    if (!edp.moses_files.empty()) {
        // load the bscores
        vector<behavioral_score> bscores;
        vector<bscored_combo_tree> bcts;
        foreach(string file, edp.moses_files) {
            ifstream in(file);
            in.exceptions(ifstream::failbit | ifstream::badbit | ifstream::eofbit);
            while (in.good()) {
                try {
                    bcts.push_back(istream_bscored_combo_tree(in));
                } catch(...) {}
            }
        }

        // compute the distances between the bscores
        vector<float> dsts;
        for (unsigned i = 0; i < bcts.size(); ++i)
            for (unsigned j = 0; j < i; ++j)
                dsts.push_back(lp_distance(get_bscore(bcts[i]), get_bscore(bcts[j]),
                                           edp.diversity_p_norm));

        // write the results
        write_results(edp, dsts);
    }

    if (!edp.input_files.empty()) {        
        // load the target features (assumed to be of float type) and
        // dump them in vectors of floats
        vector<vector<float>> targets;
        foreach(string file, edp.input_files) {
            vector<float> target;
            boost::transform(combo::loadOTable(file, edp.target_feature),
                             back_inserter(target), combo::cast_contin);
            targets.push_back(target);
        }

        // compute the distances between the targets
        vector<float> dsts;
        for (unsigned i = 0; i < targets.size(); ++i)
            for (unsigned j = 0; j < i; ++j)
                dsts.push_back(p_norm(targets[i], targets[j],
                                      edp.diversity_p_norm));

        // write the results
        write_results(edp, dsts);        
    }
}
