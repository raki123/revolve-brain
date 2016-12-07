#ifndef NEAT_MUTATOR_H_
#define NEAT_MUTATOR_H_

#include "genetic_encoding.h"

#include <map>
#include <string>
#include <vector>
#include <random>

//class responsible for mutation
namespace CPPNEAT {
class Mutator {
public:
	
	Mutator(std::map<Neuron::Ntype, Neuron::NeuronTypeSpec> brain_spec,
		double new_connection_sigma,
		int innovation_number,
		int max_attempts,
		std::vector<Neuron::Ntype> addable_neurons);
	
	static std::vector<Neuron::Ntype> get_addable_types(std::map<Neuron::Ntype,Neuron::NeuronTypeSpec> brain_spec);
	
	void mutate_neuron_params(GeneticEncodingPtr genotype, double probability, double sigma);
	void mutate_weights(GeneticEncodingPtr genotype, double probability, double sigma);
	
	void mutate_structure(GeneticEncodingPtr genotype, double probability);
	bool add_connection_mutation(GeneticEncodingPtr genotype, double sigma);
	void add_neuron_mutation(GeneticEncodingPtr genotype);
	void remove_connection_mutation(GeneticEncodingPtr genotype);
	void remove_neuron_mutation(GeneticEncodingPtr genotype);
	int add_neuron(NeuronPtr neuron, GeneticEncodingPtr genotype);
	int add_connection(int mark_from, int mark_to, double weight, GeneticEncodingPtr genotype, std::string socket);
	

	
	

	
private:
	std::map<Neuron::Ntype, Neuron::NeuronTypeSpec> brain_spec;
	double new_connection_sigma;
	int innovation_number;
	int max_attempts;
	std::vector<Neuron::Ntype> addable_neurons;
	std::mt19937 generator;
};
}

#endif  //NEAT_MUTATOR_H_