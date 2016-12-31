#include "neat_learner.h"
#include "cppneat/crossover.h"

#include <algorithm>
#include <iostream>
#include <random>

namespace CPPNEAT {
Learner::Learner(MutatorPtr mutator, Learner::LearningConfiguration conf)
	: active_brain(nullptr)
	, generation_number(0)
	, total_brains_evaluated(0)
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
	, initial_structural_mutations(conf.initial_structural_mutations) {
	std::random_device rd;
	generator.seed(rd());
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
	} else {
		std::cout << "no starting network given, initialise has to be called" << std::endl;
	}
	
}

void Learner::initialise(std::vector< GeneticEncodingPtr > init_genotypes) {
	if(init_genotypes.empty()) {
		brain_population = get_init_brains();
	} else {
		std::cout << "initialised with starting population" << std::endl;
		brain_population = init_genotypes;
	}
	for(GeneticEncodingPtr brain : brain_population) {
		evaluation_queue.push_back(brain);
	}
	active_brain = evaluation_queue.back();
	evaluation_queue.pop_back();	
}

std::vector< GeneticEncodingPtr > Learner::get_init_brains() {
	std::vector<GeneticEncodingPtr> init_pop;
	int i = 0;
	while(i++ < pop_size) {
		GeneticEncodingPtr mutated_genotype = start_from->copy();
		if(!mutated_genotype->is_valid()) {
			std::cerr << "copying caused invalid genotype" << std::endl;
		}
		for(int j = 0; j < initial_structural_mutations && initial_structural_mutations > 0; j++) {
			apply_structural_mutation(mutated_genotype);
		}
		
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
	//speciate
	std::map<GeneticEncodingPtr, std::vector<GeneticEncodingPtr>> old_species = species;
	species.clear();
	//choose representative from previous generation (or do nothing for first run)
	for(std::pair<GeneticEncodingPtr, std::vector<GeneticEncodingPtr>> sppair : old_species) 
	{
		if(sppair.second.size() > 0) //remove species without survivors could also be done somewhere else but doesnt really matter
		{
			std::uniform_int_distribution<int> choose(0,sppair.second.size()-1);
			GeneticEncodingPtr representative = sppair.second[choose(generator)];
			species.insert(std::make_pair(representative, std::vector<GeneticEncodingPtr>()));
		}
	}
	
	for(std::pair<GeneticEncodingPtr,double> cur_brain : brain_velocity) 
	{
		bool added = false;
		//search for matching species
		for(std::pair<GeneticEncodingPtr, std::vector<GeneticEncodingPtr>> sppair : species) 
		{
			//TODO:: coefficients?
			if(GeneticEncoding::get_dissimilarity(sppair.first, cur_brain.first,1,1,0.4) < speciation_threshold) 
			{
				added = true;
				species[sppair.first].push_back(cur_brain.first);
				break;
			}
		}
		//add new species in case of no matches
		if(!added) 
		{
			species.insert(std::make_pair(cur_brain.first, std::vector<GeneticEncodingPtr>(1, cur_brain.first)));
		}
	}
	//actual sharing
	std::map<GeneticEncodingPtr, double> new_fitness;
	for(std::pair<GeneticEncodingPtr, std::vector<GeneticEncodingPtr>> sppair : species)
	{
		for(GeneticEncodingPtr brain : sppair.second) 
		{
			new_fitness[brain] = brain_velocity[brain]/sppair.second.size();
		}
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

std::pair< GeneticEncodingPtr, GeneticEncodingPtr > Learner::select_for_tournament(std::vector< std::pair< GeneticEncodingPtr, double > > candidates) {
	std::shuffle(candidates.begin(), candidates.end(), generator);
	candidates = std::vector<std::pair<GeneticEncodingPtr, double>>(candidates.begin(), candidates.begin() + tournament_size);
	std::sort(candidates.begin(), candidates.end(), fitness_cmp);
	return std::pair<GeneticEncodingPtr, GeneticEncodingPtr>(candidates[0].first, candidates[1].first);
}

const bool Learner::ASEXUAL = false;
const int Learner::POP_SIZE = 50;
const int Learner::TOURNAMENT_SIZE = 40;
const int Learner::NUM_CHILDREN = 45;
const double Learner::WEIGHT_MUTATION_PROBABILITY = 0.8;
const double Learner::WEIGHT_MUTATION_SIGMA = 5.0;
const double Learner::PARAM_MUTATION_PROBABILITY = 0.8;
const double Learner::PARAM_MUTATION_SIGMA = 0.25;
const double Learner::STRUCTURAL_AUGMENTATION_PROBABILITY = 0.8;
const double Learner::STRUCTURAL_REMOVAL_PROBABILITY = 0;
const int Learner::MAX_GENERATIONS = 20;
const double Learner::SPECIATION_TRESHOLD = 0.03;
const int Learner::REPEAT_EVALUATIONS = 1;
const int Learner::INITIAL_STRUCTURAL_MUTATIONS = 1;

}