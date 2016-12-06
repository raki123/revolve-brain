#include "crossover.h"
#include "mutatur.h"
#include "gene.h"
#include "neuron_gene.h"
#include "connection_gene.h"

#include <map>
#include <string>
#include <vector>

namespace CPPNEAT {

GeneticEncodingPtr Crossover::crossover(GeneticEncodingPtr genotype_more_fit, GeneticEncodingPtr genotype_less_fit) {
	std::uniform_real_distribution<double> uniform(0,1);
	genotype_more_fit = genotype_more_fit->copy();
	genotype_less_fit = genotype_less_fit->copy();
	
	std::vector<GenePtr> genes_better = genotype_more_fit->get_sorted_genes();
	std::vector<GenePtr> genes_worse = genotype_less_fit->get_sorted_genes();
	std::vector<std::pair<GenePtr, GenePtr>> gene_pairs = GeneticEncoding::get_pairs(genes_better, genes_worse);
	
	std::vector<GenePtr> child_genes;
	for(std::pair<GenePtr, GenePtr> pair : gene_pairs) {
		if(pair.first != nullptr && pair.second != nullptr) {
			if(uniform(Mutator::generator) < 0.5) {
				child_genes.push_back(pair.first);
			} else {
				child_genes.push_back(pair.second);
			}
		} else if(pair.first != nullptr) {
			child_genes.push_back(pair.first);
		}
	}
	
	GeneticEncodingPtr child_genotype(new GeneticEncoding());
	for(GenePtr gene : child_genes) {
		if(gene->gene_type == Gene::NEURON_GENE) {
			child_genotype->add_neuron_gene(boost::dynamic_pointer_cast<NeuronGene>(gene));
		} else if(gene->gene_type == Gene::CONNECTION_GENE) {
			child_genotype->add_connection_gene(boost::dynamic_pointer_cast<ConnectionGene>(gene));
		}
	}
	return child_genotype;
}
}