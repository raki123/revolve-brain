#include "neat_learner.h"
#include "crossover.h"

#include <algorithm>
#include <iostream>
#include <random>

namespace CPPNEAT {
Learner::Learner(MutatorPtr mutator, Learner::LearningConfiguration conf)
	: active_brain(nullptr)
	, mutator(mutator)
	, asexual(conf.asexual)
	, pop_size(conf.pop_size)
	, tournament_size(conf.tournament_size)
	, num_children(conf.num_children)
	, weight_mutation_probability(conf.weight_mutation_probability)
	, weight_mutation_sigma(conf.weight_mutation_sigma)
	, param_mutation_probability(conf.param_mutation_probability)
	, param_mutation_sigma(conf.param_mutation_sigma)
	, structural_augmentation_probability(conf.structural_augmentation_probability)
	, structural_removal_probability(conf.structural_removal_probability)
	, max_generations(conf.max_generations)
	, speciation_threshold(conf.speciation_threshold)
	, repeat_evaluations(conf.repeat_evaluations)
	, start_from(conf.start_from)
	, generation_number(0)
	, total_brains_evaluated(0) {
	if(pop_size < 2) {
		pop_size = 2;
	}
	if(tournament_size > pop_size) {
		tournament_size = pop_size;
	}
	if(tournament_size < 2) {
		tournament_size = 2;
	}
	if(start_from != nullptr) {
		std::cout << "generating inital population from starting network" << std::endl;
		initialise(std::vector<GeneticEncodingPtr>());
		this->mutator->make_starting_genotype_known(start_from);
	}
	
}

void Learner::initialise(std::vector< GeneticEncodingPtr > init_genotypes) {
	if(init_genotypes.empty()) {
		brain_population = get_init_brains();
	} else {
		brain_population = init_genotypes;
	}
	for(GeneticEncodingPtr brain : brain_population) {
		evaluation_queue.push_back(brain);
	}
	active_brain = evaluation_queue.back();
	evaluation_queue.pop_back();	
}
//     @trollius.coroutine
//     def initialize(self, world, init_genotypes=None):
//         if init_genotypes is None:
//             brain_population = self.get_init_brains()
//         else:
//             brain_population = init_genotypes
// 
//         for br in brain_population:
//             validate_genotype(br, "initial generation created invalid genotype")
//             self.evaluation_queue.append(br)
// 
//         first_brain = self.evaluation_queue.popleft()
// 
//         self.reset_fitness()
//         yield From(self.activate_brain(world, first_brain))
std::vector< GeneticEncodingPtr > Learner::get_init_brains() {
	std::vector<GeneticEncodingPtr> init_pop;
	int i = 0;
	while(i++ < pop_size) {
		GeneticEncodingPtr mutated_genotype = start_from->copy();
		apply_structural_mutation(mutated_genotype);
		
		mutator->mutate_weights(mutated_genotype, weight_mutation_probability, weight_mutation_sigma);
		
		mutator->mutate_neuron_params(mutated_genotype, param_mutation_probability, param_mutation_sigma);
		init_pop.push_back(mutated_genotype);
	}
	return init_pop;
}

void Learner::reportFitness(std::string id, GeneticEncodingPtr genotype, double fitness) {
	std::cout << "Evalutation over\n" << "Evaluated " << ++total_brains_evaluated << " brains \n" << "Last fitness: " << fitness << std::endl;
	
	fitness_buffer.push_back(fitness);
	if(fitness_buffer.size() == repeat_evaluations) {
		double sum = 0;
		for(double add : fitness_buffer) {
			sum+= add;
		}
		double average_fitness = sum/repeat_evaluations;
		brain_fitness[active_brain] = average_fitness;
		brain_velocity[active_brain] = average_fitness;
		
		if(evaluation_queue.size() == 0) {
			share_fitness();
			
			produce_new_generation();
			std::reverse(evaluation_queue.begin(), evaluation_queue.end());
			generation_number++;
		}
		active_brain = evaluation_queue.back();
		evaluation_queue.pop_back();
		fitness_buffer.clear();
		if(generation_number >= max_generations) {
			std::cout << "Maximum number of generations reached" << std::endl;
			std::exit(0);
		}
	}
}
GeneticEncodingPtr Learner::getNewGenome(std::string id) {
	return active_brain;
}

void Learner::share_fitness() {
	std::map<GeneticEncodingPtr, double> new_fitness;
	for(auto it : brain_fitness) {
		GeneticEncodingPtr cur_brain = it.first;
		double cur_fitness = it.second;
		int species_size = 1;
		for(auto it2 : brain_fitness) {
			GeneticEncodingPtr other_brain = it2.first;
			if(other_brain != cur_brain) {
				if(GeneticEncoding::get_dissimilarity(other_brain, cur_brain,1,1,1) < speciation_threshold) {
					species_size++;
				}
			}
		}
		new_fitness[cur_brain] = cur_fitness/species_size;
	}
	brain_fitness = new_fitness;
}
bool fitness_cmp(std::pair<GeneticEncodingPtr,double> genotype1, std::pair<GeneticEncodingPtr, double> genotype2) {
	return genotype1.second > genotype2.second;
}
void Learner::produce_new_generation() {
	std::vector<std::pair<GeneticEncodingPtr, double>> fitness_pairs;
	for(auto it : brain_fitness) {
		fitness_pairs.push_back(std::pair<GeneticEncodingPtr, double>(it.first, it.second));
	}
	std::vector<std::pair<GeneticEncodingPtr, double>> velocity_pairs;
	for(auto it : brain_velocity) {
		velocity_pairs.push_back(std::pair<GeneticEncodingPtr, double>(it.first, it.second));
	}
	std::sort(fitness_pairs.begin(), fitness_pairs.end(), fitness_cmp);
	std::sort(velocity_pairs.begin(), velocity_pairs.end(), fitness_cmp);
	
	std::vector<std::pair<GeneticEncodingPtr, GeneticEncodingPtr>> parent_pairs;
	int i = 0;
	while(i++ < num_children) {
		std::pair<GeneticEncodingPtr, GeneticEncodingPtr> selected = select_for_tournament(fitness_pairs);
		parent_pairs.push_back(selected);
	}
	for(std::pair<GeneticEncodingPtr, GeneticEncodingPtr> parents : parent_pairs) {
		GeneticEncodingPtr child_genotype = produce_child(parents.first, parents.second);
		evaluation_queue.push_back(child_genotype);
	}
	i = 0;
	while(i++ < (pop_size - num_children)) {
		evaluation_queue.push_back(velocity_pairs[i].first);
	}
	brain_fitness.clear();
	brain_velocity.clear();
}

GeneticEncodingPtr Learner::produce_child(GeneticEncodingPtr parent1, GeneticEncodingPtr parent2) {
	GeneticEncodingPtr child_genotype;
	if(asexual) {
		child_genotype = parent1->copy();
	} else {
		child_genotype = Crossover::crossover(parent1, parent2);
	}
	
	mutator->mutate_weights(child_genotype, weight_mutation_probability, weight_mutation_sigma);
	
	mutator->mutate_neuron_params(child_genotype, param_mutation_probability, param_mutation_sigma);
	
	apply_structural_mutation(child_genotype);
	
	return child_genotype;
}

void Learner::apply_structural_mutation(GeneticEncodingPtr genotype) {
	std::uniform_real_distribution<double> uniform(0,1);
	mutator->mutate_structure(genotype, structural_augmentation_probability);
	
	if(uniform(generator) < structural_removal_probability) {
		if(uniform(generator) < 0.5) {
			mutator->remove_connection_mutation(genotype);
		} else {
			mutator->remove_neuron_mutation(genotype);
		}
	}
}

//ALERT::not sure if real tournament selection
std::pair< GeneticEncodingPtr, GeneticEncodingPtr > Learner::select_for_tournament(std::vector< std::pair< GeneticEncodingPtr, double > > candidates) {
	std::shuffle(candidates.begin(), candidates.end(), generator);
	candidates = std::vector<std::pair<GeneticEncodingPtr, double>>(candidates.begin(), candidates.begin() + tournament_size);
	std::sort(candidates.begin(), candidates.end(), fitness_cmp);
	return std::pair<GeneticEncodingPtr, GeneticEncodingPtr>(candidates[0].first, candidates[1].first);
}

}