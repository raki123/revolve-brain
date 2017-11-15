#include "CPPNCrossover.h"

#include <random>

namespace CPPNEAT
{
  GeneticEncodingPtr
  Crossover::crossover(
          GeneticEncodingPtr _moreFitGenome,
          GeneticEncodingPtr _lessFitGenome)
  {
    assert(_lessFitGenome->is_layered_ == _moreFitGenome->is_layered_);
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution< double > uniform(0, 1);
    _moreFitGenome = _moreFitGenome->copy();
    _lessFitGenome = _lessFitGenome->copy();

    auto genes_better = _moreFitGenome->SortedGenes();
    auto genes_worse = _lessFitGenome->SortedGenes();
    auto gene_pairs = GeneticEncoding::Pairs(
            genes_better,
            genes_worse);

    std::vector< GenePtr > child_genes;
    for (const auto &pair : gene_pairs)
    {
      if (pair.first != nullptr && pair.second != nullptr)
      {
        if (uniform(mt) < 0.5)
        {
          child_genes.push_back(pair.first);
        }
        else
        {
          child_genes.push_back(pair.second);
        }
      }
      else if (pair.first not_eq nullptr)
      {
        child_genes.push_back(pair.first);
      }
    }
    if (not _lessFitGenome->is_layered_)
    {
      GeneticEncodingPtr child_genotype(new GeneticEncoding(false));
      for (const auto &gene : child_genes)
      {
        if (gene->Type() == Gene::NEURON_GENE)
        {
          child_genotype->AddNeuron(
                  boost::dynamic_pointer_cast< NeuronGene >(gene));
        }
        else if (gene->Type() == Gene::CONNECTION_GENE)
        {
          child_genotype->AddConnection(
                  boost::dynamic_pointer_cast< ConnectionGene >(gene));
        }
      }
      return child_genotype;
    }
    else
    {
      //what helps us tremendously here is the fact that a gene is only in
      // the child if it is in the more fit parent therefore we can use the
      // same layer structure as in the more fit parent here
      _moreFitGenome->connections_
                       .clear();
      for (const auto &gene : child_genes)
      {
        if (gene->Type() == Gene::NEURON_GENE)
        {
          auto index = _moreFitGenome->convert_in_to_layer_index(
                  gene->InnovationNumber());
          _moreFitGenome->layers_[index.first][index.second] =
                  boost::dynamic_pointer_cast< NeuronGene >(gene);
        }
        else if (gene->Type() == Gene::CONNECTION_GENE)
        {
          _moreFitGenome->AddConnection(
                  boost::dynamic_pointer_cast< ConnectionGene >(gene));
        }
      }
      return _moreFitGenome;
    }
  }
}