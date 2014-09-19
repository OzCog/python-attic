/** eval-candidate.cc --- 
 *
 * Copyright (C) 2013 OpenCog Foundation
 * Copyright (C) 2014 Aidyia Limited
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
 * Evaluate the score of a candidate w.r.t. a fitness function. All
 * fitness functions available to moses are available (or at least
 * supposed to be).
 */

#include <boost/math/special_functions/binomial.hpp>
#include <boost/range/algorithm_ext/for_each.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>

#include <opencog/util/iostreamContainer.h>
#include <opencog/util/oc_assert.h>

#include <opencog/comboreduct/combo/iostream_combo.h>
#include <opencog/comboreduct/table/table_io.h>
#include <opencog/learning/moses/moses/types.h>
#include <opencog/learning/moses/moses/complexity.h>

#include "../scoring/discriminating_bscore.h"
#include "../scoring/behave_cscore.h"
#include "../scoring/bscores.h"
#include "../scoring/precision_bscore.h"

#include "eval-candidate.h"

using namespace std;
using namespace opencog;
using namespace moses;
using namespace combo;
using boost::str;
using boost::trim;
using boost::math::binomial_coefficient;

/**
 * Convert a string representing a combo program in a combo_tree.
 *
 * @param combo_prog_str   the string containing the combo program
 * @param labels           a vector of labels
 * @return                 the combo_tree
 */
combo_tree str2combo_tree_label(const std::string& combo_prog_str,
                                const std::vector<std::string>& labels)
{
    // combo pogram with place holders
    std::string combo_prog_ph_str = l2ph(combo_prog_str, labels);
    std::stringstream ss(combo_prog_ph_str);
    combo_tree tr;
    ss >> tr;
    return tr;
}

vector<string> get_all_combo_tree_str(const eval_candidate_params& ecp)
{
    vector<string> res;
    // from files
    for (const std::string& combo_prg : ecp.combo_program_files) {
        ifstream in(combo_prg);
        if (in) {
            while (in.good()) {
                string line;
                getline(in, line);
                if(line.empty())
                    continue;
                res.push_back(line);
            }
        } else {
            logger().error("Error: file %s can not be found.",
                           combo_prg.c_str());
            exit(1);
        }
    }

    return res;
}

std::ostream& ostream_scored_trees(std::ostream& out,
                                   const vector<combo_tree>& trs,
                                   const vector<composite_score>& css) {
    unsigned size = trs.size();
    OC_ASSERT(size == css.size());
    for (unsigned i = 0; i < size; ++i)
        out << css[i].get_score() << " " << trs[i] << std::endl;
    return out;
}

int main(int argc, char** argv)
{
    namespace po = boost::program_options;
    eval_candidate_params ecp;
    unsigned long rand_seed;
    string log_level;
    static const string default_log_file("eval-candidate.log");
    string log_file;

    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Produce help message.\n")

        ("r,random-seed", po::value<unsigned long>(&rand_seed)->default_value(1),
         "Random seed.\n")

        // IO
        ("input-file,i", po::value<string>(&ecp.input_file),
         "DSV file containing inputs and outputs.\n")

        ("target-feature,u", po::value<string>(&ecp.target_feature_str),
         "Name of the target feature.\n")

        ("combo-program-file,C", po::value<vector<string>>(&ecp.combo_program_files),
         "File containing combo programs. "
         "Can be used several times for several files.\n")

        ("output-file,o", po::value<string>(&ecp.output_file),
         "File to write the results. If none is given it write on the stdout.\n")

        ("level,l", po::value<string>(&log_level)->default_value("INFO"),
         "Log level, possible levels are NONE, ERROR, WARN, INFO, "
         "DEBUG, FINE. Case does not matter.\n")

        ("log-file,f",
         po::value<string>(&log_file)->default_value(default_log_file),
         "File name where to write the log.\n")

        // Parameters
        ("problem,H", po::value<string>(&ecp.problem)->default_value(f_one),
         "Scorer to run. Supported scorers all erquire an input table. "
         "Supported scorers are:\n\n"
         "\trecall: show recall\n"
         "\tprerec: show precision\n"
         "\tf_one:  show F1-score (geometric mean of precision and recall)\n"
         "\tbep:    break-even point (difference between precision and recall)\n"
         "\tit:     accuracy scorer\n"
         "\tpre:    precision-activation scorer\n")

        ("alpha,Q",
         po::value<double>(&ecp.activation_pressure)->default_value(1.0),
         "pre scorer: Activation pressure.\n"
         "recall, prerec, bep scorers: Hardness.\n"
         "\nIf the score is not between the minimum and mixiimum, it is "
         "penalized with the pressure/hardness penalty.\n" )

        (",q",
         po::value<double>(&ecp.min_activation)->default_value(0.0),
         "pre scorer: Minimum activation.\n"
         "prerec scorer: Minimum recall.\n"
         "recall scorer: Minimum precision.\n"
         "bep    scorer: Minimum difference between precision and recall.\n"
         "\nIf the score is not between the minimum and mixiimum, it is "
         "penalized with the pressure/hardness penalty.\n" )

        (",w",
         po::value<double>(&ecp.max_activation)->default_value(1.0),
         "pre scorer: Maximum activation.\n"
         "prerec scorer: Maximum recall.\n"
         "recall scorer: Maximum precision.\n"
         "bep    scorer: Maximum difference between precision and recall.\n"
         "\nIf the score is not between the minimum and mixiimum, it is "
         "penalized with the pressure/hardness penalty.\n" )
        ;

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
    }
    catch (po::error& e) {
        OC_ASSERT(false, "Fatal error: invalid or duplicated argument:\n\t%s\n",
                  e.what());
    }
    po::notify(vm);

    if (vm.count("help") || argc == 1) {
        cout << desc << endl;
        return 1;
    }

    // Set logger
    logger().setFilename(log_file);
    trim(log_level);
    Logger::Level level = logger().getLevelFromString(log_level);
    if (level != Logger::BAD_LEVEL)
        logger().setLevel(level);
    else {
        cerr << "Error: Log level " << log_level
             << " is incorrect (see --help)." << endl;
        exit(1);
    }
    logger().setBackTraceLevel(Logger::ERROR);

    // Record original command line
    std::stringstream ss;
    for (int i=0; i<argc; i++) {
        ss << " " << argv[i];
    }
    logger().info() << "Command line:" << ss.str();

    // init random generator
    randGen().seed(rand_seed);

    // get all combo tree strings (from command line and file)
    vector<string> all_combo_tree_str = get_all_combo_tree_str(ecp);

    // read data ITable
    Table table = loadTable(ecp.input_file, ecp.target_feature_str);
    ITable& it = table.itable;

    // parse combo programs
    vector<combo_tree> trs;
    for (const string& tr_str : all_combo_tree_str) {
        combo_tree tr = str2combo_tree_label(tr_str, it.get_labels());
        if (logger().isDebugEnabled()) {
            logger().fine() << "Combo str: " << tr_str;
            logger().debug() << "Parsed combo: " << tr;
        }
        trs.push_back(tr);
    }

    // Define scorer (only support f_one for now)
    bscore_base* bscore = NULL;
    if ("recall" == ecp.problem) {
        bscore = new recall_bscore(table.compressed(),
            ecp.min_activation, ecp.max_activation, ecp.activation_pressure);
    }
    else if ("prerec" == ecp.problem) {
        bscore = new prerec_bscore(table.compressed(),
            ecp.min_activation, ecp.max_activation, ecp.activation_pressure);
    }
    else if ("bep" == ecp.problem) {
        bscore = new bep_bscore(table.compressed(),
            ecp.min_activation, ecp.max_activation, ecp.activation_pressure);
    }
    else if ("f_one" == ecp.problem) {
        bscore = new f_one_bscore(table.compressed());
    }
    else if ("it" == ecp.problem) {
        bscore = new ctruth_table_bscore(table.compressed());
    }
    else if ("pre" == ecp.problem) {
        bscore = new precision_bscore(table.compressed(),
            ecp.activation_pressure, ecp.min_activation, ecp.max_activation);
    }
    else {
        OC_ASSERT(false, "Unknown scorer type.");
    }

    behave_cscore bcscore(*bscore);

    // Evaluate the fitness score of each program
    vector<composite_score> css;
    for (const combo_tree& tr : trs)
        css.push_back(bcscore.get_cscore(tr));

    // Output the trees preceded by their scores
    if(ecp.output_file.empty())
        ostream_scored_trees(cout, trs, css);
    else {
        ofstream of(ecp.output_file.c_str());
        ostream_scored_trees(of, trs, css);
    }
}
