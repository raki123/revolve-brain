#include "CPPNCrossover.h"

#include <random>

namespace CPPNEAT {

GeneticEncodingPtr
Crossover::crossover(GeneticEncodingPtr genotype_more_fit,
                     GeneticEncodingPtr genotype_less_fit)
{
  assert(genotype_less_fit->is_layered_ == genotype_more_fit->is_layered_);
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution<double> uniform(0,
                                                 1);
  genotype_more_fit = genotype_more_fit->copy();
  genotype_less_fit = genotype_less_fit->copy();

  std::vector<GenePtr> genes_better = genotype_more_fit->get_sorted_genes();
  std::vector<GenePtr> genes_worse = genotype_less_fit->get_sorted_genes();
  std::vector<std::pair<GenePtr, GenePtr>> gene_pairs = GeneticEncoding::get_pairs(genes_better,
                                                                                   genes_worse);

  std::vector<GenePtr> child_genes;
  for (std::pair<GenePtr, GenePtr> pair : gene_pairs) {
    if (pair.first != nullptr && pair.second != nullptr) {
      if (uniform(mt) < 0.5) {
        child_genes.push_back(pair.first);
      } else {
        child_genes.push_back(pair.second);
      }
    } else if (pair.first != nullptr) {
      child_genes.push_back(pair.first);
    }
  }
  if (!genotype_less_fit->is_layered_) {
    GeneticEncodingPtr child_genotype(new GeneticEncoding(false));
    for (GenePtr gene : child_genes) {
      if (gene->gene_type == Gene::NEURON_GENE) {
        child_genotype->add_neuron_gene(boost::dynamic_pointer_cast<NeuronGene>(gene));
      } else if (gene->gene_type == Gene::CONNECTION_GENE) {
        child_genotype->add_connection_gene(boost::dynamic_pointer_cast<ConnectionGene>(gene));
      }
    }
    return child_genotype;
  } else {
    //what helps us tremendously here is the fact that a gene is only in the child if it is in the more fit parent
    //therefore we can use the same layer structure as in the more fit parent here
    genotype_more_fit->connection_genes_
                     .clear();
    for (GenePtr gene : child_genes) {
      if (gene->gene_type == Gene::NEURON_GENE) {
        std::pair<unsigned int, unsigned int> index = genotype_more_fit->convert_in_to_layer_index(gene->getInnovNumber());
        genotype_more_fit->layers_[index.first][index.second] = boost::dynamic_pointer_cast<NeuronGene>(gene);
      } else if (gene->gene_type == Gene::CONNECTION_GENE) {
        genotype_more_fit->add_connection_gene(boost::dynamic_pointer_cast<ConnectionGene>(gene));
      }
    }
    return genotype_more_fit;
  }
}
}