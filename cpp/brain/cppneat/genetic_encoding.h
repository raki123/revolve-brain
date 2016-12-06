#ifndef NEAT_GENETIC_ENCODING_H_
#define NEAT_GENETIC_ENCODING_H_

#include "types.h"
#include "neuron.h"

#include <vector>
#include <utility>
//class for the encoding of one genotype
namespace CPPNEAT {
class GeneticEncoding {

public:
	GeneticEncoding(std::vector<NeuronGenePtr> neuron_genes, std::vector<ConnectionGenePtr> connection_genes) 
		: neuron_genes(neuron_genes)
		, connection_genes(connection_genes) {}
	GeneticEncoding() {}
	
	GeneticEncodingPtr copy();
	
	int num_genes();
	int num_neuron_genes();
	int num_connection_genes();
	bool connection_exists(int mark_from, int mark_to);
	
	static double get_dissimilarity(GeneticEncodingPtr genotype1, GeneticEncodingPtr genotype2, double excess_coef = 1, double disjoint_coef = 1, double weight_diff_coef = 1);
	static void get_excess_disjoint(GeneticEncodingPtr genotype1, GeneticEncodingPtr genotype2, int &excess_num, int &disjoint_num);
	static std::vector<std::pair<GenePtr, GenePtr>> get_pairs(std::vector<GenePtr> genes_sorted1, std::vector<GenePtr> genes_sorted2);
	static std::vector<std::pair<int,int>> get_space_map(std::vector<GeneticEncodingPtr> genotypes, std::map<Neuron::Ntype, Neuron::NeuronTypeSpec> brain_spec);
	
	void adopt(GeneticEncodingPtr adoptee);
	std::vector<GenePtr> get_sorted_genes();
	std::pair<int,int> min_max_innov_numer();
	GenePtr find_gene_by_in(int innov_number);
	void add_neuron_gene(NeuronGenePtr neuron_gene);	
	void add_connection_gene(ConnectionGenePtr connection_gene);
	void remove_connection_gene(int index);
	void remonve_neuron_gene(int index);
	bool neuron_exists(int innov_number);

   
public:
	std::vector<NeuronGenePtr> neuron_genes;
	std::vector<ConnectionGenePtr> connection_genes;
	
private:
	std::vector<GenePtr> all_genes_sorted;
	bool all_genes_valid;
	
}; 
}

#endif //NEAT_GENETIC_ENCODING_H_