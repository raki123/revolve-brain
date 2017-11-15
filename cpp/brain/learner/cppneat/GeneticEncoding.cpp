#include "GeneticEncoding.h"

#include <iostream>
#include <cmath>

namespace CPPNEAT
{

  GeneticEncodingPtr GeneticEncoding::copy()
  {
    if (not is_layered_)
    {
      GeneticEncodingPtr copy_gen(new GeneticEncoding(false));
      for (const auto &neuron_gene : neurons_)
      {
        NeuronGenePtr copy_neuron(new NeuronGene(*neuron_gene));
        copy_gen->AddNeuron(copy_neuron);
      }
      for (const auto &connection_gene : connections_)
      {
        ConnectionGenePtr copy_conn(new ConnectionGene(*connection_gene));
        copy_gen->AddConnection(copy_conn);
      }
      return copy_gen;
    }
    else
    {
      GeneticEncodingPtr copy_gen(new GeneticEncoding(true));
      for (size_t i = 0; i < layers_.size(); i++)
      {
        bool first = true;
        for (const auto &neuron_gene : layers_[i])
        {
          if (first)
          {
            NeuronGenePtr copy_neuron(new NeuronGene(*neuron_gene));
            copy_gen->AddNeuron(
                    copy_neuron,
                    i,
                    true);
            first = false;
          }
          else
          {
            NeuronGenePtr copy_neuron(new NeuronGene(*neuron_gene));
            copy_gen->AddNeuron(
                    copy_neuron,
                    i,
                    false);
          }
        }
      }
      for (const auto &connection_gene : connections_)
      {
        ConnectionGenePtr copy_conn(new ConnectionGene(*connection_gene));
        copy_gen->AddConnection(copy_conn);
      }
      return copy_gen;
    }
  }

  size_t GeneticEncoding::num_genes()
  {
    if (not is_layered_)
    {
      return neurons_.size() + connections_.size();
    }
    else
    {
      size_t sum = 0;
      for (const auto &layer : layers_)
      {
        sum += layer.size();
      }
      return sum + connections_.size();
    }
  }

  size_t GeneticEncoding::num_neuron_genes()
  {
    if (not is_layered_)
    {
      return neurons_.size();
    }
    else
    {
      size_t sum = 0;
      for (const auto &layer : layers_)
      {
        sum += layer.size();
      }
      return sum;
    }
  }

  size_t GeneticEncoding::num_connection_genes()
  {
    return connections_.size();
  }

  bool GeneticEncoding::HasConnection(
          const size_t _from,
          const size_t _to)
  {
    for (const auto &connection : connections_)
    {
      if (connection->From() == _from
          and connection->To() == _to
          and connection->IsEnabled())
      {
        return true;
      }
    }
    return false;
  }

  double GeneticEncoding::Dissimilarity(
          GeneticEncodingPtr genotype1,
          GeneticEncodingPtr genotype2,
          double excess_coef,
          double disjoint_coef,
          double weight_diff_coef)
  {
    int excess_num = 0, disjoint_num = 0;
    GeneticEncoding::ExcessDisjoint(
            genotype1,
            genotype2,
            excess_num,
            disjoint_num);
    size_t num_genes = std::max(
            genotype1->num_genes(),
            genotype2->num_genes());
    auto gene_pairs = GeneticEncoding::Pairs(
            genotype1->SortedGenes(),
            genotype2->SortedGenes());
    double weight_diff = 0;
    int count = 0;
    for (const auto &pair : gene_pairs)
    {
      if (pair.first not_eq nullptr and pair.second not_eq nullptr)
      {
        if (pair.first->Type() == Gene::CONNECTION_GENE)
        {
          weight_diff += std::abs(
                  boost::dynamic_pointer_cast< ConnectionGene >(
                          pair.first)->Weight()
                  - boost::dynamic_pointer_cast< ConnectionGene >(
                          pair.second)->Weight());
          count++;
        }
      }
    }
    double average_weight_diff = count > 0 ? weight_diff / count : 0;
    double dissimilarity =
            (disjoint_coef * disjoint_num + excess_coef * excess_num) / num_genes +
            weight_diff_coef * average_weight_diff;
    return dissimilarity;

  }

  void GeneticEncoding::ExcessDisjoint(
          GeneticEncodingPtr genotype1,
          GeneticEncodingPtr genotype2,
          int &excess_num,
          int &disjoint_num)
  {
    std::vector< GenePtr > genes_sorted1 = genotype1->SortedGenes();
    std::vector< GenePtr > genes_sorted2 = genotype2->SortedGenes();

    size_t min_mark1 = genes_sorted1[0]->InnovationNumber();
    size_t max_mark1 =
            genes_sorted1[genes_sorted1.size() - 1]->InnovationNumber();

    size_t min_mark2 = genes_sorted2[0]->InnovationNumber();
    size_t max_mark2 =
            genes_sorted2[genes_sorted2.size() - 1]->InnovationNumber();

    std::vector< std::pair< GenePtr, GenePtr>>
            pairs = GeneticEncoding::Pairs(genes_sorted1,
                                           genes_sorted2);

    for (const auto &pair : pairs)
    {
      if (pair.first != nullptr and pair.second == nullptr)
      {
        if (pair.first->InnovationNumber() > (min_mark2 - 1)
            and pair.first->InnovationNumber() < (max_mark2 + 1))
        {
          disjoint_num++;
        }
        else
        {
          excess_num++;
        }
      }
      else if (pair.first == nullptr && pair.second != nullptr)
      {
        if (pair.second->InnovationNumber() > (min_mark1 - 1)
            and pair.second->InnovationNumber() < (max_mark1 + 1))
        {
          disjoint_num++;
        }
        else
        {
          excess_num++;
        }
      }
    }
  }

  std::vector< std::pair< GenePtr, GenePtr > > GeneticEncoding::Pairs(
          std::vector< GenePtr > genes_sorted1,
          std::vector< GenePtr > genes_sorted2)
  {
    size_t num_genes1 = genes_sorted1.size();
    size_t num_genes2 = genes_sorted2.size();

    size_t min_mark1 = genes_sorted1[0]->InnovationNumber();
    size_t max_mark1 =
            genes_sorted1[genes_sorted1.size() - 1]->InnovationNumber();

    size_t min_mark2 = genes_sorted2[0]->InnovationNumber();
    size_t max_mark2 =
            genes_sorted2[genes_sorted2.size() - 1]->InnovationNumber();

    size_t min_mark = std::min(min_mark1, min_mark2);
    size_t max_mark = std::max(max_mark1, max_mark2);

    std::vector< std::pair< GenePtr, GenePtr>> gene_pairs;

    //search for pairs with equal marks
    size_t start_from1 = 0;
    size_t start_from2 = 0;

    size_t mark = min_mark;

    while (mark < max_mark + 1)
    {
      //jump1 and jump2 are here to skip long sequences of empty historical marks
      GenePtr gene1 = nullptr;
      size_t jump1 = mark + 1;
      for (size_t i = start_from1; i < num_genes1; i++)
      {
        if (genes_sorted1[i]->InnovationNumber() == mark)
        {
          gene1 = genes_sorted1[i];
          start_from1 = i;
          break;
        }
        else if (genes_sorted1[i]->InnovationNumber() > mark)
        { //if there is a gap jump over it
          jump1 = genes_sorted1[i]->InnovationNumber();
          start_from1 = i;
          break;
        }
        else if (i == num_genes1 - 1)
        { // if the end of the gene sequence is reached
          jump1 = max_mark + 1;
          start_from1 = i;
          break;
        }
      }

      GenePtr gene2 = nullptr;
      size_t jump2 = mark + 1;
      for (size_t i = start_from2; i < num_genes2; i++)
      {
        if (genes_sorted2[i]->InnovationNumber() == mark)
        {
          gene2 = genes_sorted2[i];
          start_from2 = i;
          break;
        }
        else if (genes_sorted2[i]->InnovationNumber() > mark)
        { //if there is a gap jump over it
          jump2 = genes_sorted2[i]->InnovationNumber();
          start_from2 = i;
          break;
        }
        else if (i == num_genes2 - 1)
        { // if the end of the gene sequence is reached
          jump2 = max_mark + 1;
          start_from2 = i;
          break;
        }
      }

      if (gene1 not_eq nullptr or gene2 not_eq nullptr)
      {
        gene_pairs.push_back({gene1, gene2});
      }

      mark = std::min(jump1, jump2);
    }
    return gene_pairs;
  }

  std::vector< std::pair< int, int > > GeneticEncoding::get_space_map(
          std::vector< GeneticEncodingPtr > genotypes,
          std::map< Neuron::Ntype, Neuron::NeuronTypeSpec > brain_spec)
  {
    std::vector< std::vector< GenePtr>> sorted_gene_vectors;
    for (const auto &genotype : genotypes)
    {
      sorted_gene_vectors.push_back(genotype->SortedGenes());
    }

    size_t glob_min_in = sorted_gene_vectors[0][0]->InnovationNumber();
    size_t glob_max_in =
            sorted_gene_vectors[0][sorted_gene_vectors.size() - 1]->InnovationNumber();

    for (const auto &gene_vector : sorted_gene_vectors)
    {
      size_t min_in = gene_vector[0]->InnovationNumber();
      size_t max_in = gene_vector[gene_vector.size() - 1]->InnovationNumber();

      if (min_in < glob_min_in)
      {
        glob_min_in = min_in;
      }
      if (max_in > glob_max_in)
      {
        glob_max_in = max_in;
      }
    }

    std::vector< std::pair< int, int>> in_param_numbers;
    //TODO::check of 0 is necessary
    //TODO::make this faster
    for (size_t in = 0; in <= glob_max_in; in++)
    {
      GenePtr cur_gene = nullptr;
      for (std::vector< GenePtr > gene_vector : sorted_gene_vectors)
      {
        bool total_break = false;
        for (const auto &gene : gene_vector)
        {
          if (gene->InnovationNumber() == in)
          {
            cur_gene = gene;
            total_break = true;
            break;
          }
          else if (gene->InnovationNumber() > in)
          {
            break;
          }
        }
        if (total_break)
        {
          break;
        }
      }
      if (cur_gene == nullptr)
      {
        in_param_numbers.push_back({in, 0});
      }
      else
      {
        if (cur_gene->Type() == Gene::CONNECTION_GENE)
        {
          in_param_numbers.push_back({in, 1});
        }
        else if (cur_gene->Type() == Gene::NEURON_GENE)
        {
          Neuron::Ntype neuron_type =
                  boost::dynamic_pointer_cast< NeuronGene >(cur_gene)->neuron
                                                                     ->type_;
          Neuron::NeuronTypeSpec neuron_spec = brain_spec[neuron_type];
          in_param_numbers.push_back({in, neuron_spec.param_specs.size()});
        }
      }
    }
    return in_param_numbers;
  }

//ALERT::only works non-layered but seems to be not needed
  void GeneticEncoding::adopt(GeneticEncodingPtr adoptee)
  {
    this->SortedGenes();
    std::vector< GenePtr > adoptee_sorted_genes = adoptee->SortedGenes();
    auto pairs = GeneticEncoding::Pairs(
            adoptee_sorted_genes,
            all_genes_sorted);
    for (const auto &pair : pairs)
    {
      //adopt genes that i dont have
      if (pair.first not_eq nullptr and pair.second == nullptr)
      {
        if (pair.first->Type() == Gene::NEURON_GENE)
        {
          this->AddNeuron(
                  boost::dynamic_pointer_cast< NeuronGene >(pair.first));
        }
        else if (pair.first->Type() == Gene::CONNECTION_GENE)
        {
          this->AddConnection(
                  boost::dynamic_pointer_cast< ConnectionGene >(pair.first));
        }
      }
    }
  }

  bool gene_cmp(
          GenePtr gene1,
          GenePtr gene2)
  {
    return gene1->InnovationNumber() < gene2->InnovationNumber();
  }

  std::vector< GenePtr >
  GeneticEncoding::SortedGenes()
  {
    if (not all_genes_valid)
    {
      all_genes_sorted.clear();
      if (not is_layered_)
      {
        for (const auto &neuron_gene : neurons_)
        {
          all_genes_sorted.push_back(boost::dynamic_pointer_cast< Gene >(
                  neuron_gene));
        }
        for (const auto &connection_gene : connections_)
        {
          all_genes_sorted.push_back(boost::dynamic_pointer_cast< Gene >(
                  connection_gene));
        }
      }
      else
      {
        for (std::vector< NeuronGenePtr > layer : layers_)
        {
          for (const auto &neuron_gene : layer)
          {
            all_genes_sorted.push_back(boost::dynamic_pointer_cast< Gene >(
                    neuron_gene));
          }
        }
        for (const auto &connection_gene : connections_)
        {
          all_genes_sorted.push_back(boost::dynamic_pointer_cast< Gene >(
                  connection_gene));
        }
      }
      std::sort(all_genes_sorted.begin(),
                all_genes_sorted.end(),
                gene_cmp);
      all_genes_valid = true;
    }
    return all_genes_sorted;
  }

  std::pair< int, int > GeneticEncoding::min_max_innov_numer()
  {
    this->SortedGenes();
    return {all_genes_sorted[0]->InnovationNumber(),
            all_genes_sorted[all_genes_sorted.size() - 1]->InnovationNumber()};
  }

  GenePtr GeneticEncoding::Find(const size_t innov_number)
  {
    SortedGenes();
    for (const auto &gene : all_genes_sorted)
    {
      if (gene->InnovationNumber() == innov_number)
      {
        return gene;
      }
    }
    return nullptr;
  }

//non-layered
  void GeneticEncoding::AddNeuron(NeuronGenePtr neuron_gene)
  {
    SortedGenes();
    neurons_.push_back(neuron_gene);
    auto ins = std::upper_bound(
            all_genes_sorted.begin(),
            all_genes_sorted.end(),
            boost::dynamic_pointer_cast< Gene >(neuron_gene),
            gene_cmp);
    all_genes_sorted.insert(
            ins,
            boost::dynamic_pointer_cast< Gene >(neuron_gene));
  }

//layered
  void GeneticEncoding::AddNeuron(
          NeuronGenePtr _neuron,
          const size_t _layer,
          const bool _newLayer)
  {
    SortedGenes();
    if (_newLayer)
    {
      layers_.emplace(
              layers_.begin() + _layer,
              std::vector< NeuronGenePtr >(1, _neuron));
    }
    else
    {
      layers_[_layer].push_back(_neuron);
    }
    auto ins = std::upper_bound(
            all_genes_sorted.begin(),
            all_genes_sorted.end(),
            boost::dynamic_pointer_cast< Gene >(_neuron),
            gene_cmp);
    all_genes_sorted.insert(
            ins,
            boost::dynamic_pointer_cast< Gene >(_neuron));
  }


  void GeneticEncoding::AddConnection(ConnectionGenePtr _connection)
  {
    SortedGenes();
    connections_.push_back(_connection);
    auto ins = std::upper_bound(
            all_genes_sorted.begin(),
            all_genes_sorted.end(),
            boost::dynamic_pointer_cast< Gene >(_connection),
            gene_cmp);
    all_genes_sorted.insert(
            ins,
            boost::dynamic_pointer_cast< Gene >(_connection));
  }

  void GeneticEncoding::RemoveNeuron(const size_t index)
  {
    GenePtr old = boost::dynamic_pointer_cast< Gene >(neurons_[index]);
    neurons_.erase(neurons_.begin() + index);
    auto it = std::find(
            all_genes_sorted.begin(),
            all_genes_sorted.end(),
            old);
    all_genes_sorted.erase(it);
  }

  void GeneticEncoding::RemoveNeuron(
          const size_t _layer,
          const size_t _index)
  {
    GenePtr old = boost::dynamic_pointer_cast< Gene >(layers_[_layer][_index]);
    if (layers_[_layer].size() == 1)
    {
      layers_.erase(layers_.begin() + _layer);
    }
    else
    {
      layers_[_layer].erase(layers_[_layer].begin() + _index);
    }
    auto it = std::find(
            all_genes_sorted.begin(),
            all_genes_sorted.end(),
            old);
    all_genes_sorted.erase(it);
  }

  void GeneticEncoding::RemoveConnection(const size_t _index)
  {
    GenePtr old = boost::dynamic_pointer_cast< Gene >(connections_[_index]);
    connections_.erase(connections_.begin() + _index);
    auto it = std::find(
            all_genes_sorted.begin(),
            all_genes_sorted.end(),
            old);
    all_genes_sorted.erase(it);
  }

  bool GeneticEncoding::HasNeuron(const size_t _innovationNumber)
  {
    if (is_layered_)
    {
      for (const auto &layer : this->layers_)
      {
        for (const auto &neuron : layer)
        {
          if (neuron->InnovationNumber() == _innovationNumber)
          {
            return true;
          }
        }
      }
      return false;
    }
    else
    {
      for (const auto &neuron : this->neurons_)
      {
        if (neuron->InnovationNumber() == _innovationNumber)
        {
          return true;
        }
      }
      return false;
    }
  }

#ifdef CPPNEAT_DEBUG
  bool GeneticEncoding::is_valid()
  {
      if (not layered)
      {
          for(ConnectionGenePtr connection_gene : connection_genes) {
              if (not neuron_exists(connection_gene->mark_from) || not neuron_exists(connection_gene->mark_to)) {
                  if (not neuron_exists(connection_gene->mark_from)) {
                      std::cerr << "neuron with mark " << connection_gene->mark_from << " doesnt exist" <<std::endl;
                  }
                  if (not neuron_exists(connection_gene->mark_to)) {
                      std::cerr << "neuron with mark " << connection_gene->mark_to << " doesnt exist" <<std::endl;
                  }
                  return false;
              }
          }
      } else {
          for(ConnectionGenePtr connection_gene : connection_genes) {
              if (not neuron_exists(connection_gene->mark_from) || not neuron_exists(connection_gene->mark_to)) {
                  if (not neuron_exists(connection_gene->mark_from)) {
                      std::cerr << "neuron with mark " << connection_gene->mark_from << " doesnt exist" <<std::endl;
                  }
                  if (not neuron_exists(connection_gene->mark_to)) {
                      std::cerr << "neuron with mark " << connection_gene->mark_to << " doesnt exist" <<std::endl;
                  }
                  return false;
              }
              if(convert_in_to_layer_index(connection_gene->mark_from).first  >= convert_in_to_layer_index(connection_gene->mark_to).first) {
                  std::cerr << "layer of neuron with in " << connection_gene->mark_from << " is geq than layer of neuron with in " << connection_gene->mark_to << std::endl;
                  return false;
              }
          }
      }
      return true;
  }
#endif

  std::pair< size_t, size_t > GeneticEncoding::convert_index_to_layer_index(
          size_t index)
  {
    size_t layer = 0;
    size_t in_layer = 0;
    size_t i = 0;
    while (i < index)
    {
      if (layers_[layer].size() == in_layer + 1)
      {
        layer++;
        in_layer = 0;
      }
      else
      {
        in_layer++;
      }
      i++;
    }
    return {layer, in_layer};
  }

  std::pair< size_t, size_t > GeneticEncoding::convert_in_to_layer_index(
          const size_t innov_number)
  {
    size_t layer = 0;
    size_t in_layer = 0;
    while (layers_[layer][in_layer]->InnovationNumber() != innov_number)
    {
      if (layers_[layer].size() == in_layer + 1)
      {
        layer++;
        in_layer = 0;
      }
      else
      {
        in_layer++;
      }
    }
    return {layer, in_layer};
  }

}