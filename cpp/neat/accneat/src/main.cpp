/*
  Copyright 2001 The University of Texas at Austin

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/
//#include "std.h" // Must be included first. Precompiled header with standard library includes.
#include <unistd.h>
// #include "experiments/experiment.h"
#include "neat.h"
#include "util/rng.h"
#include "util/util.h"

#include <iostream>
#include <string>
#include <map>

using namespace NEAT;

#define DEFAULT_RNG_SEED 1
#define DEFAULT_MAX_GENS 10000

void usage() {
//     std::cerr << "usage: neat [OPTIONS]... experiment_name" << std::endl;
//     std::cerr << std::endl;
//     std::cerr << "experiment names: ";
//     auto names = Experiment::get_names();
//     for(size_t i = 0; i < names.size(); i++) {
//         if(i != 0)
//             std::cerr << ", ";
//         std::cerr << names[i];
//     }
//     std::cerr << std::endl;
//     std::cerr << std::endl;
// 
//     std::cerr << "OPTIONS" << std::endl;
//     std::cerr << "  -f                   Force deletion of any data from previous run." << std::endl;
//     std::cerr << "  -c num_experiments   (default=" << env->num_runs << ")" << std::endl;
//     std::cerr << "  -r RNG_seed          (default=" << DEFAULT_RNG_SEED << ")" << std::endl;
//     std::cerr << "  -n population_size   (default=" << env->pop_size << ")" << std::endl;
//     std::cerr << "  -x max_generations   (default=" << DEFAULT_MAX_GENS << ")" << std::endl;
//     std::cerr << "  -s search_type       {phased, blended, complexify} (default=phased)" << std::endl;
// 
// 
//     exit(1);
}

template<typename T>
T parse_enum(const char *opt, std::string str, std::map<std::string,T> vals) {
    auto it = vals.find(str);
    if(it == vals.end()) {
        error("Invalid value for " << opt << ": " << str);
    }
    return it->second;
}

int parse_int(const char *opt, const char *str) {
    try {
        return std::stoi(str);
    } catch(...) {
        error("Expecting integer argument for " << opt << ", found '" << str << "'.");
    }
}

int main(int argc, char *argv[]) {

    int rng_seed = DEFAULT_RNG_SEED;
    int maxgens = DEFAULT_MAX_GENS;
    bool force_delete = false;

    {
        int opt;
        while( (opt = getopt(argc, argv, "fc:r:p:g:n:x:s:")) != -1) {
            switch(opt) {
            case 'f':
                force_delete = true;
                break;
            case 'c':
                env->num_runs = parse_int("-c", optarg);
                break;
            case 'r':
                rng_seed = parse_int("-r", optarg);
                break;
            case 'n':
                env->pop_size = parse_int("-n", optarg);
                break;
            case 'x':
                maxgens = parse_int("-x", optarg);
                break;
            case 's':
                env->search_type = parse_enum<GeneticSearchType>("-s", optarg, {
                        {"phased", GeneticSearchType::PHASED},
                        {"blended", GeneticSearchType::BLENDED},
                        {"complexify", GeneticSearchType::COMPLEXIFY}
                    });
                break;
            default:
                error("Invalid option: -" << (char)opt);
            }
        }
    }

    int nargs = argc - optind;
    if(nargs == 0) {
        usage();
    } else if(nargs > 1) {
        error("Unexpected argument: " << argv[optind+1]);
    }

    if(force_delete) {
        sh("rm -rf experiment_*");
    } else if(exists("experiment_1")) {
        error("Already exists: experiment_1.\nMove your experiment directories or use -f to delete them automatically.")
    }

    if(env->search_type == GeneticSearchType::BLENDED) {
        env->mutate_delete_node_prob *= 0.1;
        env->mutate_delete_link_prob *= 0.1;
    }

//     const char *experiment_name = argv[optind++];
// 
//     Experiment *exp = Experiment::get(experiment_name);
//     if(exp == nullptr) {
//         trap("No such experiment: " << experiment_name);
//     }
// 
//     rng_t rng{rng_seed};
//     exp->run(rng, maxgens);

    return(0);
}

