#include "GeneticEncoding.h"

#include <iostream>
#include <cmath>

namespace CPPNEAT {

GeneticEncodingPtr
GeneticEncoding::copy()
{
  if (!is_layered_) {
    GeneticEncodingPtr copy_gen(new GeneticEncoding(false));
    for (NeuronGenePtr neuron_gene : neuron_genes_) {
      NeuronGenePtr copy_neuron(new NeuronGene(*neuron_gene));
      copy_gen->add_neuron_gene(copy_neuron);
    }
    for (ConnectionGenePtr connection_gene : connection_genes_) {
      ConnectionGenePtr copy_conn(new ConnectionGene(*connection_gene));
      copy_gen->add_connection_gene(copy_conn);
    }
    return copy_gen;
  } else {
    GeneticEncodingPtr copy_gen(new GeneticEncoding(true));
    for (size_t i = 0; i < layers_.size(); i++) {
      bool first = true;
      for (NeuronGenePtr neuron_gene : layers_[i]) {
        if (first) {
          NeuronGenePtr copy_neuron(new NeuronGene(*neuron_gene));
          copy_gen->add_neuron_gene(copy_neuron,
                                    i,
                                    true);
          first = false;
        } else {
          NeuronGenePtr copy_neuron(new NeuronGene(*neuron_gene));
          copy_gen->add_neuron_gene(copy_neuron,
                                    i,
                                    false);
        }
      }
    }
    for (ConnectionGenePtr connection_gene : connection_genes_) {
      ConnectionGenePtr copy_conn(new ConnectionGene(*connection_gene));
      copy_gen->add_connection_gene(copy_conn);
    }
    return copy_gen;
  }
}

size_t
GeneticEncoding::num_genes()
{
  if (!is_layered_) {
    return neuron_genes_.size() + connection_genes_.size();
  } else {
    size_t sum = 0;
    for (std::vector<NeuronGenePtr> layer : layers_) {
      sum += layer.size();
    }
    return sum + connection_genes_.size();
  }
}

size_t
GeneticEncoding::num_neuron_genes()
{
  if (!is_layered_) {
    return neuron_genes_.size();
  } else {
    size_t sum = 0;
    for (std::vector<NeuronGenePtr> layer : layers_) {
      sum += layer.size();
    }
    return sum;
  }
}

size_t
GeneticEncoding::num_connection_genes()
{
  return connection_genes_.size();
}

bool GeneticEncoding::connection_exists(int mark_from,
                                        int mark_to)
{
  for (auto it : connection_genes_) {
    if ((*it).mark_from == mark_from && (*it).mark_to == mark_to && (*it).isEnabled()) {
      return true;
    }
  }
  return false;
}

double GeneticEncoding::get_dissimilarity(GeneticEncodingPtr genotype1,
                                          GeneticEncodingPtr genotype2,
                                          double excess_coef,
                                          double disjoint_coef,
                                          double weight_diff_coef)
{
  int excess_num = 0, disjoint_num = 0;
  GeneticEncoding::get_excess_disjoint(genotype1,
                                       genotype2,
                                       excess_num,
                                       disjoint_num);
  size_t num_genes = std::max(genotype1->num_genes(),
                           genotype2->num_genes());
  std::vector<std::pair<GenePtr, GenePtr>> gene_pairs = GeneticEncoding::get_pairs(genotype1->get_sorted_genes(),
                                                                                   genotype2->get_sorted_genes());
  double weight_diff = 0;
  int count = 0;
  for (std::pair<GenePtr, GenePtr> pair : gene_pairs) {
    if (pair.first != nullptr && pair.second != nullptr) {
      if (pair.first
              ->gene_type == Gene::CONNECTION_GENE) {
        weight_diff += std::abs(boost::dynamic_pointer_cast<ConnectionGene>(pair.first)->weight
                                - boost::dynamic_pointer_cast<ConnectionGene>(pair.second)->weight);
        count++;
      }
    }
  }
  double average_weight_diff = count > 0 ? weight_diff / count : 0;
  double dissimilarity = (disjoint_coef * disjoint_num + excess_coef * excess_num) / num_genes +
                         weight_diff_coef * average_weight_diff;
  return dissimilarity;

}

void GeneticEncoding::get_excess_disjoint(GeneticEncodingPtr genotype1,
                                          GeneticEncodingPtr genotype2,
                                          int &excess_num,
                                          int &disjoint_num)
{
  std::vector<GenePtr> genes_sorted1 = genotype1->get_sorted_genes();
  std::vector<GenePtr> genes_sorted2 = genotype2->get_sorted_genes();

  size_t min_mark1 = genes_sorted1[0]->getInnovNumber();
  size_t max_mark1 = genes_sorted1[genes_sorted1.size() - 1]->getInnovNumber();

  size_t min_mark2 = genes_sorted2[0]->getInnovNumber();
  size_t max_mark2 = genes_sorted2[genes_sorted2.size() - 1]->getInnovNumber();

  std::vector<std::pair<GenePtr, GenePtr>> pairs = GeneticEncoding::get_pairs(genes_sorted1,
                                                                              genes_sorted2);

  for (std::pair<GenePtr, GenePtr> pair : pairs) {
    if (pair.first != nullptr && pair.second == nullptr) {
      if (pair.first->getInnovNumber() > (min_mark2 - 1) && pair.first->getInnovNumber() < (max_mark2 + 1)) {
        disjoint_num++;
      } else {
        excess_num++;
      }
    } else if (pair.first == nullptr && pair.second != nullptr) {
      if (pair.second->getInnovNumber() > (min_mark1 - 1) && pair.second->getInnovNumber() < (max_mark1 + 1)) {
        disjoint_num++;
      } else {
        excess_num++;
      }
    }
  }
}

std::vector<std::pair<GenePtr, GenePtr> >
GeneticEncoding::get_pairs(std::vector<GenePtr> genes_sorted1,
                           std::vector<GenePtr> genes_sorted2)
{
  size_t num_genes1 = genes_sorted1.size();
  size_t num_genes2 = genes_sorted2.size();

  size_t min_mark1 = genes_sorted1[0]->getInnovNumber();
  size_t max_mark1 = genes_sorted1[genes_sorted1.size() - 1]->getInnovNumber();

  size_t min_mark2 = genes_sorted2[0]->getInnovNumber();
  size_t max_mark2 = genes_sorted2[genes_sorted2.size() - 1]->getInnovNumber();

  size_t min_mark = std::min(min_mark1,
                          min_mark2);
  size_t max_mark = std::max(max_mark1,
                          max_mark2);

  std::vector<std::pair<GenePtr, GenePtr>> gene_pairs;

  //search for pairs with equal marks
  size_t start_from1 = 0;
  size_t start_from2 = 0;

  size_t mark = min_mark;

  while (mark < max_mark + 1) {
    //jump1 and jump2 are here to skip long sequences of empty historical marks
    GenePtr gene1 = nullptr;
    size_t jump1 = mark + 1;
    for (size_t i = start_from1; i < num_genes1; i++) {
      if (genes_sorted1[i]->getInnovNumber() == mark) {
        gene1 = genes_sorted1[i];
        start_from1 = i;
        break;
      } else if (genes_sorted1[i]->getInnovNumber() > mark) { //if there is a gap jump over it
        jump1 = genes_sorted1[i]->getInnovNumber();
        start_from1 = i;
        break;
      } else if (i == num_genes1 - 1) { // if the end of the gene sequence is reached
        jump1 = max_mark + 1;
        start_from1 = i;
        break;
      }
    }

    GenePtr gene2 = nullptr;
    size_t jump2 = mark + 1;
    for (size_t i = start_from2; i < num_genes2; i++) {
      if (genes_sorted2[i]->getInnovNumber() == mark) {
        gene2 = genes_sorted2[i];
        start_from2 = i;
        break;
      } else if (genes_sorted2[i]->getInnovNumber() > mark) { //if there is a gap jump over it
        jump2 = genes_sorted2[i]->getInnovNumber();
        start_from2 = i;
        break;
      } else if (i == num_genes2 - 1) { // if the end of the gene sequence is reached
        jump2 = max_mark + 1;
        start_from2 = i;
        break;
      }
    }

    if (gene1 != nullptr || gene2 != nullptr) {
      gene_pairs.push_back(std::pair<GenePtr, GenePtr>(gene1,
                                                       gene2));
    }

    mark = std::min(jump1,
                    jump2);
  }
  return gene_pairs;
}

std::vector<std::pair<int, int> >
GeneticEncoding::get_space_map(std::vector<GeneticEncodingPtr> genotypes,
                               std::map<Neuron::Ntype, Neuron::NeuronTypeSpec> brain_spec)
{
  std::vector<std::vector<GenePtr>> sorted_gene_vectors;
  for (GeneticEncodingPtr genotype : genotypes) {
    sorted_gene_vectors.push_back(genotype->get_sorted_genes());
  }

  size_t glob_min_in = sorted_gene_vectors[0][0]->getInnovNumber();
  size_t glob_max_in = sorted_gene_vectors[0][sorted_gene_vectors.size() - 1]->getInnovNumber();

  for (std::vector<GenePtr> gene_vector : sorted_gene_vectors) {
    size_t min_in = gene_vector[0]->getInnovNumber();
    size_t max_in = gene_vector[gene_vector.size() - 1]->getInnovNumber();

    if (min_in < glob_min_in) {
      glob_min_in = min_in;
    }
    if (max_in > glob_max_in) {
      glob_max_in = max_in;
    }
  }

  std::vector<std::pair<int, int>> in_param_numbers;
  //TODO::check of 0 is necessary
  //TODO::make this faster
  for (size_t in = 0; in <= glob_max_in; in++) {
    GenePtr cur_gene = nullptr;
    for (std::vector<GenePtr> gene_vector : sorted_gene_vectors) {
      bool total_break = false;
      for (GenePtr gene : gene_vector) {
        if (gene->getInnovNumber() == in) {
          cur_gene = gene;
          total_break = true;
          break;
        } else if (gene->getInnovNumber() > in) {
          break;
        }
      }
      if (total_break) {
        break;
      }
    }
    if (cur_gene == nullptr) {
      in_param_numbers.push_back(std::pair<int, int>(in,
                                                     0));
    } else {
      if (cur_gene->gene_type == Gene::CONNECTION_GENE) {
        in_param_numbers.push_back(std::pair<int, int>(in,
                                                       1));
      } else if (cur_gene->gene_type == Gene::NEURON_GENE) {
        Neuron::Ntype neuron_type = boost::dynamic_pointer_cast<NeuronGene>(cur_gene)->neuron
                                                                                     ->neuron_type;
        Neuron::NeuronTypeSpec neuron_spec = brain_spec[neuron_type];
        in_param_numbers.push_back(std::pair<int, int>(in,
                                                       neuron_spec.param_specs
                                                                  .size()));
      }
    }
  }
  return in_param_numbers;
}

//ALERT::only works non-layered but seems to be not needed
void GeneticEncoding::adopt(GeneticEncodingPtr adoptee)
{
  get_sorted_genes();
  std::vector<GenePtr> adoptee_sorted_genes = adoptee->get_sorted_genes();
  std::vector<std::pair<GenePtr, GenePtr>> pairs = GeneticEncoding::get_pairs(adoptee_sorted_genes,
                                                                              all_genes_sorted);
  for (std::pair<GenePtr, GenePtr> pair : pairs) {
    //adopt genes that i dont have
    if (pair.first != nullptr && pair.second == nullptr) {
      if (pair.first
              ->gene_type == Gene::NEURON_GENE) {
        add_neuron_gene(boost::dynamic_pointer_cast<NeuronGene>(pair.first));
      } else if (pair.first
                     ->gene_type == Gene::CONNECTION_GENE) {
        add_connection_gene(boost::dynamic_pointer_cast<ConnectionGene>(pair.first));
      }
    }
  }
}

bool gene_cmp(GenePtr gene1,
              GenePtr gene2)
{
  return gene1->getInnovNumber() < gene2->getInnovNumber();
}

std::vector<GenePtr>
GeneticEncoding::get_sorted_genes()
{
  if (!all_genes_valid) {
    all_genes_sorted.clear();
    if (!is_layered_) {
      for (NeuronGenePtr neuron_gene : neuron_genes_) {
        all_genes_sorted.push_back(boost::dynamic_pointer_cast<Gene>(neuron_gene));
      }
      for (ConnectionGenePtr connection_gene : connection_genes_) {
        all_genes_sorted.push_back(boost::dynamic_pointer_cast<Gene>(connection_gene));
      }
    } else {
      for (std::vector<NeuronGenePtr> layer : layers_) {
        for (NeuronGenePtr neuron_gene : layer) {
          all_genes_sorted.push_back(boost::dynamic_pointer_cast<Gene>(neuron_gene));
        }
      }
      for (ConnectionGenePtr connection_gene : connection_genes_) {
        all_genes_sorted.push_back(boost::dynamic_pointer_cast<Gene>(connection_gene));
      }
    }
    std::sort(all_genes_sorted.begin(),
              all_genes_sorted.end(),
              gene_cmp);
    all_genes_valid = true;
  }
  return all_genes_sorted;
}

std::pair<int, int>
GeneticEncoding::min_max_innov_numer()
{
  get_sorted_genes();
  return std::pair<int, int>(all_genes_sorted[0]->getInnovNumber(),
                             all_genes_sorted[all_genes_sorted.size() - 1]->getInnovNumber());
}

GenePtr GeneticEncoding::find_gene_by_in(const size_t innov_number)
{
  get_sorted_genes();
  for (GenePtr gene : all_genes_sorted) {
    if (gene->getInnovNumber() == innov_number) {
      return gene;
    }
  }
  return nullptr;
}

//non-layered
void GeneticEncoding::add_neuron_gene(NeuronGenePtr neuron_gene)
{
  get_sorted_genes();
  neuron_genes_.push_back(neuron_gene);
  auto ins = std::upper_bound(all_genes_sorted.begin(),
                              all_genes_sorted.end(),
                              boost::dynamic_pointer_cast<Gene>(neuron_gene),
                              gene_cmp);
  all_genes_sorted.insert(ins,
                          boost::dynamic_pointer_cast<Gene>(neuron_gene));
}

//layered
void GeneticEncoding::add_neuron_gene(NeuronGenePtr neuron_gene,
                                      int layer,
                                      bool is_new_layer)
{
  get_sorted_genes();
  if (is_new_layer) {
    layers_.emplace(layers_.begin() + layer, std::vector<NeuronGenePtr>(1, neuron_gene));
  } else {
    layers_[layer].push_back(neuron_gene);
  }
  auto ins = std::upper_bound(all_genes_sorted.begin(),
                              all_genes_sorted.end(),
                              boost::dynamic_pointer_cast<Gene>(neuron_gene),
                              gene_cmp);
  all_genes_sorted.insert(ins, boost::dynamic_pointer_cast<Gene>(neuron_gene));
}


void GeneticEncoding::add_connection_gene(ConnectionGenePtr connection_gene)
{
  get_sorted_genes();
  connection_genes_.push_back(connection_gene);
  auto ins = std::upper_bound(all_genes_sorted.begin(),
                              all_genes_sorted.end(),
                              boost::dynamic_pointer_cast<Gene>(connection_gene),
                              gene_cmp);
  all_genes_sorted.insert(ins,
                          boost::dynamic_pointer_cast<Gene>(connection_gene));
}

void GeneticEncoding::remonve_neuron_gene(int index)
{
  GenePtr old = boost::dynamic_pointer_cast<Gene>(neuron_genes_[index]);
  neuron_genes_.erase(neuron_genes_.begin() + index);
  auto it = std::find(all_genes_sorted.begin(),
                      all_genes_sorted.end(),
                      old);
  all_genes_sorted.erase(it);
}

void GeneticEncoding::remove_neuron_gene(int layer,
                                         int index)
{
  GenePtr old = boost::dynamic_pointer_cast<Gene>(layers_[layer][index]);
  if (layers_[layer].size() == 1) {
    layers_.erase(layers_.begin() + layer);
  } else {
    layers_[layer].erase(layers_[layer].begin() + index);
  }
  auto it = std::find(all_genes_sorted.begin(),
                      all_genes_sorted.end(),
                      old);
  all_genes_sorted.erase(it);
}

void GeneticEncoding::remove_connection_gene(int index)
{
  GenePtr old = boost::dynamic_pointer_cast<Gene>(connection_genes_[index]);
  connection_genes_.erase(connection_genes_.begin() + index);
  auto it = std::find(all_genes_sorted.begin(),
                      all_genes_sorted.end(),
                      old);
  all_genes_sorted.erase(it);
}

bool GeneticEncoding::neuron_exists(const size_t innov_number)
{
  if (!is_layered_) {
    for (NeuronGenePtr gene : neuron_genes_) {
      if (gene->getInnovNumber() == innov_number) {
        return true;
      }
    }
    return false;
  } else {
    for (std::vector<NeuronGenePtr> layer : layers_) {
      for (NeuronGenePtr neuron_gene : layer) {
        if (neuron_gene->getInnovNumber() == innov_number) {
          return true;
        }
      }
    }
    return false;
  }
}

#ifdef CPPNEAT_DEBUG
bool GeneticEncoding::is_valid()
{
    if(!layered)
    {
        for(ConnectionGenePtr connection_gene : connection_genes) {
            if(!neuron_exists(connection_gene->mark_from) || !neuron_exists(connection_gene->mark_to)) {
                if(!neuron_exists(connection_gene->mark_from)) {
                    std::cerr << "neuron with mark " << connection_gene->mark_from << " doesnt exist" <<std::endl;
                }
                if(!neuron_exists(connection_gene->mark_to)) {
                    std::cerr << "neuron with mark " << connection_gene->mark_to << " doesnt exist" <<std::endl;
                }
                return false;
            }
        }
    } else {
        for(ConnectionGenePtr connection_gene : connection_genes) {
            if(!neuron_exists(connection_gene->mark_from) || !neuron_exists(connection_gene->mark_to)) {
                if(!neuron_exists(connection_gene->mark_from)) {
                    std::cerr << "neuron with mark " << connection_gene->mark_from << " doesnt exist" <<std::endl;
                }
                if(!neuron_exists(connection_gene->mark_to)) {
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

std::pair<size_t, size_t>
GeneticEncoding::convert_index_to_layer_index(size_t index)
{
  size_t layer = 0;
  size_t in_layer = 0;
  size_t i = 0;
  while (i < index) {
    if (layers_[layer].size() == in_layer + 1) {
      layer++;
      in_layer = 0;
    } else {
      in_layer++;
    }
    i++;
  }
  return std::pair<size_t, size_t>(layer, in_layer);
}

std::pair<size_t, size_t>
GeneticEncoding::convert_in_to_layer_index(const size_t innov_number)
{
  size_t layer = 0;
  size_t in_layer = 0;
  while (layers_[layer][in_layer]->getInnovNumber() != innov_number) {
    if (layers_[layer].size() == in_layer + 1) {
      layer++;
      in_layer = 0;
    } else {
      in_layer++;
    }
  }
  return std::pair<size_t, size_t>(layer,
                                               in_layer);
}

}