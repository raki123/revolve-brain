#ifndef CPPNEAT_LEARNER_H_
#define CPPNEAT_LEARNER_H_

//#define CPPNEAT_DEBUG

#include "cppneat/types.h"
#include "cppneat/mutator.h"
#include "learner.h"

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
		int max_generations;
		double speciation_threshold;
		int repeat_evaluations;
		GeneticEncodingPtr start_from;
		int initial_structural_mutations;
		double interspecies_mate_probability;
	};
	Learner(MutatorPtr mutator, LearningConfiguration conf);
	void initialise(std::vector<GeneticEncodingPtr> init_genotypes);
	void initialise_from_yaml(std::string yaml_path);
	
	void apply_structural_mutation(GeneticEncodingPtr genotype);
	
	//standard parameters
	static const bool ASEXUAL;
	static const int POP_SIZE;
	static const int TOURNAMENT_SIZE;
	static const int NUM_CHILDREN;
	static const double WEIGHT_MUTATION_PROBABILITY;
	static const double WEIGHT_MUTATION_SIGMA;
	static const double PARAM_MUTATION_PROBABILITY;
	static const double PARAM_MUTATION_SIGMA;
	static const double STRUCTURAL_AUGMENTATION_PROBABILITY;
	static const double STRUCTURAL_REMOVAL_PROBABILITY;
	static const int MAX_GENERATIONS;
	static const double SPECIATION_TRESHOLD;
	static const int REPEAT_EVALUATIONS;
	static const int INITIAL_STRUCTURAL_MUTATIONS;
	static const double INTERSPECIES_MATE_PROBABILITY;
private:
	virtual void reportFitness(std::string id, GeneticEncodingPtr genotype, double fitness);
	virtual GeneticEncodingPtr getNewGenome(std::string id);
	std::vector<GeneticEncodingPtr> get_init_brains();
	void writeGenome(std::string robot_name, GeneticEncodingPtr genome);
	void share_fitness();
	void produce_new_generation();
	GeneticEncodingPtr produce_child(GeneticEncodingPtr parent1, GeneticEncodingPtr parent2);
	std::pair<GeneticEncodingPtr, GeneticEncodingPtr> select_for_tournament(std::vector<std::pair<GeneticEncodingPtr, double>> candidates, unsigned int tourn_size);


	GeneticEncodingPtr active_brain;
	double fitness;
	std::vector<GeneticEncodingPtr> evaluation_queue;
	std::vector<GeneticEncodingPtr> brain_population;
	std::map<GeneticEncodingPtr, double> brain_fitness;
	std::map<GeneticEncodingPtr, double> brain_velocity;
	std::map<GeneticEncodingPtr, std::vector<GeneticEncodingPtr>> species;
	std::vector<double> fitness_buffer;
	int generation_number;
	int total_brains_evaluated;
	
	MutatorPtr mutator;
	
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
	int max_generations;
	double speciation_threshold;
	int repeat_evaluations;
	GeneticEncodingPtr start_from;
	int initial_structural_mutations;
	double interspecies_mate_probability;
	std::mt19937 generator;

}; 
}

#endif //CPPNEAT_LEARNER_H_