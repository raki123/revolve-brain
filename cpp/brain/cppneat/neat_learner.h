#ifndef CPPNEAT_LEARNER_H_
#define CPPNEAT_LEARNER_H_

#include "types.h"
#include "mutatur.h"
#include "../split_cpg/learner.h"

#include <map>
#include <string>
#include <vector>
#include <random>

//crossover between genotypes
namespace CPPNEAT {
class Learner : public revolve::brain::Learner<GeneticEncodingPtr> {
public:
	struct LearningConfiguration {
		bool asexual;
		int pop_size;
		int tournament_size;
		int num_children;
		double weight_mutation_probability;
		double weight_mutation_sigma;
		double param_mutation_probability;
		double param_mutation_sigma;
		double structural_augmentation_probability;
		double structural_removal_probability;
		double max_generations;
		double speciation_threshold;
		int repeat_evaluations;
		GeneticEncodingPtr start_from;
	};
	Learner(Mutator mutator, LearningConfiguration conf);
	void initialise(std::vector<GeneticEncodingPtr> init_genotypes);
private:
	virtual void reportFitness(std::string id, GeneticEncodingPtr genotype, double fitness);
	virtual GeneticEncodingPtr getNewGenome(std::string id);

	
private:
	std::vector<GeneticEncodingPtr> get_init_brains();
	void share_fitness();
	void produce_new_generation();
	GeneticEncodingPtr produce_child(GeneticEncodingPtr parent1, GeneticEncodingPtr parent2);
	void apply_structural_mutation(GeneticEncodingPtr genotype);
	std::pair<GeneticEncodingPtr, GeneticEncodingPtr> select_for_tournament(std::vector<std::pair<GeneticEncodingPtr, double>> candidates);


	GeneticEncodingPtr active_brain;
	double fitness;
	std::vector<GeneticEncodingPtr> evaluation_queue;
	std::vector<GeneticEncodingPtr> brain_population;
	std::map<GeneticEncodingPtr, double> brain_fitness;
	std::map<GeneticEncodingPtr, double> brain_velocity;
	std::vector<double> fitness_buffer;
	int generation_number;
	int total_brains_evaluated;
	
	Mutator mutator;
	
	bool asexual;
	int pop_size;
	int tournament_size;
	int num_children;
	double weight_mutation_probability;
	double weight_mutation_sigma;
	double param_mutation_probability;
	double param_mutation_sigma;
	double structural_augmentation_probability;
	double structural_removal_probability;
	double max_generations;
	double speciation_threshold;
	int repeat_evaluations;
	GeneticEncodingPtr start_from;
	std::mt19937 generator;

}; 
}

#endif //CPPNEAT_LEARNER_H_