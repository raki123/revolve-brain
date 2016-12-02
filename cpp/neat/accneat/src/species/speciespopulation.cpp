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
#include "genomemanager.h"
#include "organism.h"
#include "species.h"
#include "speciespopulation.h"
#include "util/timer.h"
#include "util/util.h"

#include <assert.h>

#ifdef WITH_OPENMP
#include <omp.h>
#endif

using namespace NEAT;
using namespace std;

SpeciesPopulation::SpeciesPopulation(rng_t rng,
                                     vector<unique_ptr<Genome>> &seeds)
    : norgs(seeds.size())
    , generation(0)
    , orgs(rng, seeds, seeds.size())
    , highest_fitness(0.0)
    , highest_last_changed(0) {

    spawn();
}

SpeciesPopulation::~SpeciesPopulation() {
    std::vector<Species*>::iterator curspec;
    std::vector<SpeciesOrganism*>::iterator curorg;

    if (species.begin()!=species.end()) {
        for(curspec=species.begin();curspec!=species.end();++curspec) {
            delete (*curspec);
        }
    }
}

size_t SpeciesPopulation::size() {
    return norgs;
}

Organism *SpeciesPopulation::get(size_t index) {
    return &orgs.curr()[index];
}

unique_ptr<Organism> SpeciesPopulation::make_copy(size_t index) {
    SpeciesOrganism *copy = new SpeciesOrganism( (SpeciesOrganism&)*get(index) );
    return unique_ptr<Organism>(copy);
}

void SpeciesPopulation::verify() {
    for(auto &org: orgs.curr())
        org.genome->verify();
}

void SpeciesPopulation::spawn() {
    orgs.init_phenotypes();
         std::cout << "i get here 7.5" << std::endl;
    //Separate the new SpeciesPopulation into species
    speciate();
}

void SpeciesPopulation::speciate() {
    last_species = 0;
             std::cout << "i get here 8" << std::endl;
    for(SpeciesOrganism &org: orgs.curr()) {
        assert(org.species == nullptr);
        for(Species *s: species) {
            if( env->genome_manager->are_compatible(*org.genome,
                                                    *s->first()->genome) ) {
                org.species = s;
                break;
            }
        }
                 std::cout << "i get here 9" << std::endl;
        if(!org.species) {
            Species *s = new Species(++last_species);
            species.push_back(s);
            org.species = s;
        }
        org.species->add_Organism(&org);
    }
}

void SpeciesPopulation::write(std::ostream& out) {
    for(auto &s: species)
        s->print_to_file(out);
}

void SpeciesPopulation::next_generation() {
#ifndef NDEBUG
    for(SpeciesOrganism &org: orgs.curr()) {
        assert(org.generation == generation);
    }
#endif

    generation++;

    real_t total=0.0; //Used to compute average fitness over all Organisms
    real_t overall_average;  //The average modified fitness among ALL organisms

    //The fractional parts of expected offspring that can be
    //Used only when they accumulate above 1 for the purposes of counting
    //Offspring
    real_t skim;
    int total_expected;  //precision checking
    int total_organisms = norgs; // todo: get rid of this variable
    assert(total_organisms == env->pop_size);
    int max_expected;
    Species *best_species = nullptr;
    int final_expected;

    std::vector<Species*> sorted_species;  //Species sorted by max fit org in Species
    int half_pop;

    //We can try to keep the number of species constant at this number
    int num_species=species.size();

    for(Species *s: species) {
        s->compute_average_fitness();
        s->compute_max_fitness();
    }

    //Stick the Species pointers into a new Species list for sorting
    for(Species *s: species) {
        sorted_species.push_back(s);
    }

    //Sort the Species by max fitness (Use an extra list to do this)
    //These need to use ORIGINAL fitness
    //sorted_species.qsort(order_species);
    std::sort(sorted_species.begin(), sorted_species.end(), order_species);

    //Flag the lowest performing species over age 20 every 30 generations
    //NOTE: THIS IS FOR COMPETITIVE COEVOLUTION STAGNATION DETECTION
//     if(generation % 30 == 0) {
//         for(size_t i = sorted_species.size(); i > 0; i--) {
//             Species *s = sorted_species[i - 1];
//             if(s->age >= 20) {
//                 s->obliterate = true;
//                 break;
//             }
//         }
//     }

    std::cout<<"Number of Species: "<<num_species<<std::endl;

    //Use Species' ages to modify the objective fitness of organisms
    // in other words, make it more fair for younger species
    // so they have a chance to take hold
    //Also penalize stagnant species
    //Then adjust the fitness using the species size to "share" fitness
    //within a species.
    //Then, within each Species, mark for death
    //those below survival_thresh*average
    for(Species *s: species) {
        s->adjust_fitness();
    }

    //Go through the organisms and add up their fitnesses to compute the
    //overall average
    for(SpeciesOrganism &o: orgs.curr()) {
        total += o.adjusted_fitness;
    }
    overall_average=total/total_organisms;
    std::cout<<"Generation "<<generation<<": "<<"overall_average = "<<overall_average<<std::endl;

    //Now compute expected number of offspring for each individual organism
    for(SpeciesOrganism &o: orgs.curr()) {
        o.expected_offspring = o.adjusted_fitness / overall_average;
    }

    //Now add those offspring up within each Species to get the number of
    //offspring per Species
    skim=0.0;
    total_expected=0;
    for(Species *s: species) {
        skim = s->count_offspring(skim);
        total_expected += s->expected_offspring;
    }

    //Need to make up for lost foating point precision in offspring assignment
    //If we lost precision, give an extra baby to the best Species
    if (total_expected<total_organisms) {
        //Find the Species expecting the most
        max_expected=0;
        final_expected=0;
        for(Species *s: species) {
            if (s->expected_offspring >= max_expected) {
                max_expected = s->expected_offspring;
                best_species = s;
            }
            final_expected += s->expected_offspring;
        }
        //Give the extra offspring to the best species
        ++(best_species->expected_offspring);
        final_expected++;

        //If we still arent at total, there is a problem
        //Note that this can happen if a stagnant Species
        //dominates the population and then gets killed off by its age
        //Then the whole population plummets in fitness
        //If the average fitness is allowed to hit 0, then we no longer have
        //an average we can use to assign offspring.
        if (final_expected < total_organisms) {
            for(Species *s: species) {
                s->expected_offspring = 0;
            }
            best_species->expected_offspring = total_organisms;
        }
    }

    //Sort the Species by max fitness (Use an extra list to do this)
    //These need to use ORIGINAL fitness
    //sorted_species.qsort(order_species);
    std::sort(sorted_species.begin(), sorted_species.end(), order_species);
//     SpeciesOrganism  champ(*(sorted_species[0]->first()));
    //Check for SpeciesPopulation-level stagnation
    bool new_highest_fitness = false;
    {
        SpeciesOrganism *pop_champ = sorted_species[0]->first();
        if(pop_champ->eval.fitness > highest_fitness) {
            real_t old_highest = highest_fitness;
            highest_fitness = pop_champ->eval.fitness;
            highest_last_changed=0;
            new_highest_fitness = true;

            printf("NEW POPULATION RECORD FITNESS: %lg, delta=%lg @ gen=%d\n",
                   highest_fitness, highest_fitness - old_highest, generation);
        } else {
            ++highest_last_changed;
            new_highest_fitness = false;

            printf("%zu generations since last population fitness record: %lg\n",
                   size_t(highest_last_changed), highest_fitness);
        }
    }

    //Check for stagnation- if there is stagnation, perform delta-coding
    if (highest_last_changed >= env->dropoff_age+5) {
        highest_last_changed = 0;
        half_pop = total_organisms / 2;

        sorted_species[0]->first()->super_champ_offspring = half_pop;
        sorted_species[0]->expected_offspring = half_pop;
        sorted_species[0]->age_of_last_improvement = sorted_species[0]->age;

        if(sorted_species.size() > 1) {
            sorted_species[1]->first()->super_champ_offspring = total_organisms - half_pop;
            sorted_species[1]->expected_offspring = total_organisms - half_pop;
            sorted_species[1]->age_of_last_improvement = sorted_species[1]->age;

            //Get rid of all species under the first 2
            for(size_t i = 2, n = sorted_species.size(); i < n; i++) {
                sorted_species[i]->expected_offspring = 0;
            }
        } else {
            sorted_species[0]->first()->super_champ_offspring += total_organisms - half_pop;
            sorted_species[0]->expected_offspring += total_organisms - half_pop;
        }
    }

    //Kill off all Organisms marked for death.  The remainder
    //will be allowed to reproduce.
    for(Species *s: species) {
        s->remove_eliminated();
    }


    if(total_expected > (int)norgs) {
        warn("total_expected (" << total_expected << ") > size (" << norgs << ")");
    }

    orgs.next_generation(generation);

    //Initialize the parms for each reproduce invocation
    struct reproduce_parms_t {
        Species *species;
        int ioffspring;
    } reproduce_parms[norgs];

    {
        size_t iorg = 0;
        for(size_t i = 0, n = species.size(); i < n; i++) {
            Species *s = species[i];

            for(int j = 0; (j < s->expected_offspring) && (iorg < norgs); j++) {
                reproduce_parms[iorg].species = s;
                reproduce_parms[iorg].ioffspring = j;
                iorg++;
            }
        }
        assert(iorg == norgs);
    }

    //Create the next generation.
    {
        static Timer timer("reproduce");
        timer.start();

#ifdef WITH_OPENMP
#pragma omp parallel for
#endif
        for(size_t iorg = 0; iorg < norgs; iorg++) {
            SpeciesOrganism &baby = orgs.curr()[iorg];
            reproduce_parms_t &parms = reproduce_parms[iorg];

            assert(baby.population_index == iorg);

            parms.species->reproduce(parms.ioffspring,
                                     baby,
                                     env->genome_manager,
                                     sorted_species);
        }

        env->genome_manager->finalize_generation(new_highest_fitness);

        timer.stop();
    }

    {
        static Timer timer("speciate");
        timer.start();

        {
#ifdef WITH_OPENMP
#pragma omp parallel for
#endif
            for(size_t i = 0; i < norgs; i++) {
                SpeciesOrganism &org = orgs.curr()[i];
                Species *origin_species = reproduce_parms[i].species;

                if(env->genome_manager->are_compatible(*org.genome,
                                                       *origin_species->first()->genome)) {
                    org.species = origin_species;
                } else {
                    org.species = nullptr;

                    for(Species *s: species) {
                        if(s->size() && (s != origin_species)) {
                            if(env->genome_manager->are_compatible(*org.genome,
                                                                   *s->first()->genome)) {
                                org.species = s;
                                break;
                            }
                        }
                    }
                }
            }
        }

        size_t index_new_species = species.size();

        for(SpeciesOrganism &org: orgs.curr()) {
            if(!org.species) {
                //It didn't fit into any of the existing species. Check if it fits
                //into one we've just created.
                for(size_t i = index_new_species, n = species.size();
                    i < n;
                    i++) {

                    Species *s = species[i];
                    if(env->genome_manager->are_compatible(*org.genome,
                                                           *s->first()->genome)) {
                        org.species = s;
                        break;
                    }
                }
                //It didn't fit into a newly created species, so make one for it.
                if(!org.species) {
                    org.species = new Species(++last_species, true);
                    species.push_back(org.species);
                }
            }
            org.species->add_Organism(&org);
        }

        timer.stop();
    }

//     //Stick the Species pointers into a new Species list for sorting
//     for(Species *s: species) {
//         sorted_species.push_back(s);
//     }
//     //Sort the Species by max fitness (Use an extra list to do this)
//     //These need to use ORIGINAL fitness
//     //sorted_species.qsort(order_species);
//     std::sort(sorted_species.begin(), sorted_species.end(), order_species);
//     champ.generation++;
//     (*std::find(species.begin(), species.end(), sorted_species[sorted_species.size()-1]))->organisms[0] = &champ;
//     
    //Destroy and remove the old generation from the organisms and species
    for(Species *s: species) {
        s->remove_generation(generation - 1);
    }

    //Remove all empty Species and age ones that survive
    //As this happens, create master organism list for the new generation
    {
        size_t nspecies = 0;

        for(size_t i = 0; i < species.size(); i++) {
            Species *s = species[i];
            if(s->organisms.empty()) {
                delete s;
            } else {
                species[nspecies++] = s;

                //Age surviving Species
                if(s->novel) {
                    s->novel = false;
                } else {
                    s->age++;
                }
            }
        }

        species.resize(nspecies);
    }
    //Create the neural nets for the new organisms.
    orgs.init_phenotypes();

#ifndef NDEBUG
    for(SpeciesOrganism &org: orgs.curr()) {
        assert(org.generation == generation);
    }
#endif
}
