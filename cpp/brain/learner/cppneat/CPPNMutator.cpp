#include "CPPNMutator.h"

#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>

namespace CPPNEAT
{
  /////////////////////////////////////////////////
  std::vector< Neuron::Ntype > Mutator::AddableTypes(
          std::map< Neuron::Ntype, Neuron::NeuronTypeSpec > _specifications)
  {
    std::vector< Neuron::Ntype > possibleTypes;
    for (const auto &specificationPair : _specifications)
    {
      Neuron::NeuronTypeSpec specification = specificationPair.second;
      auto layer = std::find(
              specification.possible_layers.begin(),
              specification.possible_layers.end(),
              Neuron::HIDDEN_LAYER);
      if (layer not_eq specification.possible_layers.end())
      {
        possibleTypes.push_back(specificationPair.first);
      }
    }
    return possibleTypes;
  }

  /////////////////////////////////////////////////
  Mutator::Mutator(
          std::map< Neuron::Ntype, Neuron::NeuronTypeSpec > brain_spec,
          double new_connection_sigma,
          int innovation_number,
          int max_attempts,
          std::vector< Neuron::Ntype > addable_neurons,
          bool layered
  )
          : brain_spec(brain_spec)
          , new_connection_sigma(new_connection_sigma)
          , innovationNumber(innovation_number)
          , max_attempts(max_attempts)
          , addable_neurons(addable_neurons)
//        , is_layered_(layered)
  {
    std::random_device rd;
    generator.seed(rd());
    if (addable_neurons.size() == 0)
    {
      this->addable_neurons = AddableTypes(brain_spec);
    }

  }

  /////////////////////////////////////////////////
  void Mutator::make_starting_genotype_known(GeneticEncodingPtr genotype)
  {
    for (const auto &connection_gene : genotype->connection_genes_)
    {
      std::pair< int, int > connection_innovation(
              connection_gene->from_,
              connection_gene->to_);
      connectionInnovations[connection_innovation] =
              connection_gene->getInnovNumber();
    }
  }

  /////////////////////////////////////////////////
  void Mutator::RecordInnovations(const std::string &_path)
  {
    std::ofstream outputFile;
    outputFile.open(_path,
                    std::ios::out | std::ios::trunc);
    outputFile << "- in_no: " << innovationNumber << std::endl;
    outputFile << "- connection_innovations: " << std::endl;
    for (std::pair< std::pair< int, int >, int >
              connection_innovation : connectionInnovations)
    {
      outputFile << "  - connection_innovation: " << std::endl;
      outputFile
              << "      mark_from: "
              << connection_innovation.first.first
              << std::endl;
      outputFile
              << "      mark_to: "
              << connection_innovation.first.second
              << std::endl;
      outputFile
              << "      in_no: "
              << connection_innovation.second
              << std::endl;
    }

    outputFile << "- neuron_innovations: " << std::endl;
    for (std::pair< std::pair< int, Neuron::Ntype >, std::vector< int>>
              neuron_innovation : neuron_innovations)
    {
      outputFile << "  - neuron_innovation: " << std::endl;
      outputFile
              << "      conn_split: "
              << neuron_innovation.first
                                  .first
              << std::endl;
      outputFile << "      ntype: " << neuron_innovation.first
                                                        .second << std::endl;
      outputFile << "      in_nos: " << std::endl;
      for (int in_no : neuron_innovation.second)
      {
        outputFile << "      - in_no: " << in_no << std::endl;
      }
    }
    outputFile.close();
  }

  /////////////////////////////////////////////////
  void Mutator::LoadInnovationsFromFirst(const std::string &_path)
  {
    std::ifstream outputFile(_path);
    if (not outputFile.good())
    {
      return;
    }
    outputFile.close();
    YAML::Node yaml_file = YAML::LoadFile(_path);
    if (yaml_file.IsNull())
    {
      std::cout
              << "Failed to load the yaml file. If this is the first run do not worry."
              << std::endl;
      return;
    }
    innovationNumber = yaml_file[0]["in_no"].as< int >();
    connectionInnovations.clear();
    for (const auto &connection : yaml_file[1]["connection_innovations"])
    {
      auto innovation = connection["connection_innovation"];
      auto from = innovation["mark_from"].as< int >();
      auto to = innovation["mark_to"].as< int >();
      auto inNum = innovation["in_no"].as< int >();
      connectionInnovations.insert({{from, to}, inNum});
    }
    neuron_innovations.clear();
    for (const auto &neuron : yaml_file[2]["neuron_innovations"])
    {
      auto innovation = neuron["neuron_innovation"];
      auto split = innovation["conn_split"].as< int >();
      auto nType = static_cast<Neuron::Ntype>(innovation["ntype"].as< int >());
      std::vector< int > inNums;
      for (const auto &inNum : innovation["in_nos"])
      {
        inNums.push_back(inNum["in_no"].as< int >());
      }
      neuron_innovations.insert({{split, nType}, inNums});
    }
  }

  void Mutator::LoadInnovationsFromSecond(const std::string &_path)
  {
    std::ifstream outputFile(_path);
    if (not outputFile.good())
    {
      return;
    }
    outputFile.close();
    YAML::Node yaml_file = YAML::LoadFile(_path);
    if (yaml_file.IsNull())
    {
      std::cout
              << "Failed to load the yaml file. If this is the first run do not worry."
              << std::endl;
      return;
    }


  }

  void Mutator::mutate_neuron_params(
          GeneticEncodingPtr genotype,
          double probability,
          double sigma)
  {
    std::uniform_real_distribution< double > uniform(0,
                                                     1);
    if (not genotype->is_layered_)
    {
      for (const auto &neuron_gene : genotype->neuron_genes_)
      {
        if (uniform(generator) < probability)
        {
          auto neuron_params =
                  brain_spec[neuron_gene->neuron->neuron_type].param_specs;
          if (neuron_params.size() > 0)
          {
            std::uniform_int_distribution< size_t > uniform_int(
                    0, neuron_params.size() - 1);
            auto param = neuron_params[uniform_int(generator)];
            auto cur_val = neuron_gene->neuron->neuron_params[param.name];
            std::normal_distribution< double > normal(0, sigma);
            cur_val += normal(generator);
            neuron_gene->neuron->set_neuron_param(cur_val, param);
          }
        }
      }
    }
    else
    {
      for (std::vector< NeuronGenePtr > layer : genotype->layers_)
      {
        for (const auto &neuron_gene : layer)
        {
          if (uniform(generator) < probability)
          {
            std::vector< Neuron::ParamSpec > neuron_params =
                    brain_spec[neuron_gene->neuron->neuron_type].param_specs;
            if (not neuron_params.empty())
            {
              std::uniform_int_distribution< size_t > uniform_int(
                      0, neuron_params.size() - 1);
              auto param = neuron_params[uniform_int(generator)];
              auto cur_val = neuron_gene->neuron->neuron_params[param.name];
              std::normal_distribution< double > normal(0, sigma);
              cur_val += normal(generator);
              neuron_gene->neuron->set_neuron_param(cur_val, param);
            }
          }
        }
      }
    }
  }

  /////////////////////////////////////////////////
  void Mutator::mutate_weights(
          GeneticEncodingPtr genotype,
          double probability,
          double sigma)
  {
    std::uniform_real_distribution< double > uniform(0,
                                                     1);
    std::normal_distribution< double > normal(0,
                                              sigma);
    for (const auto &connection_gene : genotype->connection_genes_)
    {
      if (uniform(generator) < probability)
      {
        connection_gene->weight += normal(generator);
      }
    }
  }

  /////////////////////////////////////////////////
  void Mutator::mutate_structure(
          GeneticEncodingPtr genotype,
          double probability)
  {
    std::uniform_real_distribution< double > uniform(0,
                                                     1);
    if (uniform(generator) < probability)
    {
      if (genotype->connection_genes_.empty())
      {
        add_connection_mutation(genotype, new_connection_sigma);
      }
      else
      {
        if (uniform(generator) < 0.5)
        {
          add_connection_mutation(genotype, new_connection_sigma);
        }
        else
        {
          add_neuron_mutation(genotype, new_connection_sigma);
        }
      }
    }
  }

  /////////////////////////////////////////////////
  bool Mutator::add_connection_mutation(
          GeneticEncodingPtr genotype,
          double sigma)
  {
    if (not genotype->is_layered_)
    {
      std::uniform_int_distribution< size_t > choice(
              0, genotype->neuron_genes_.size() - 1);
      auto neuron_from = genotype->neuron_genes_[choice(generator)];
      auto neuron_to = genotype->neuron_genes_[choice(generator)];
      auto mark_from = neuron_from->getInnovNumber();
      auto mark_to = neuron_to->getInnovNumber();

      int num_attempts = 1;

      while (genotype->connection_exists(mark_from, mark_to)
             or neuron_to->neuron->layer == Neuron::INPUT_LAYER)
      {
        neuron_from = genotype->neuron_genes_[choice(generator)];
        neuron_to = genotype->neuron_genes_[choice(generator)];
        mark_from = neuron_from->getInnovNumber();
        mark_to = neuron_to->getInnovNumber();

        num_attempts++;
        if (num_attempts > max_attempts)
        {
          return false;
        }
      }
      std::normal_distribution< double > normal(0, sigma);
      add_connection(mark_from,
                     mark_to,
                     normal(generator),
                     genotype,
                     "");
#ifdef CPPNEAT_DEBUG
      if (not genotype->is_valid()) {
          std::cerr << "add connection mutation caused invalid genotye" << std::endl;
          throw std::runtime_error("mutation error");
      }
#endif
      return true;
    }
    else
    {
      std::uniform_int_distribution< size_t > choice(
              0, genotype->num_neuron_genes() - 1);
      auto index_from =
              genotype->convert_index_to_layer_index(choice(generator));
      auto index_to = genotype->convert_index_to_layer_index(choice(generator));
      auto neuron_from = genotype->layers_[index_from.first][index_from.second];
      auto neuron_to = genotype->layers_[index_to.first][index_to.second];
      auto mark_from = neuron_from->getInnovNumber();
      auto mark_to = neuron_to->getInnovNumber();

      int num_attempts = 1;

      while (genotype->connection_exists(mark_from, mark_to)
             or index_from.first >= index_to.first)
      {
        index_from = genotype->convert_index_to_layer_index(choice(generator));
        index_to = genotype->convert_index_to_layer_index(choice(generator));
        neuron_from = genotype->layers_[index_from.first][index_from.second];
        neuron_to = genotype->layers_[index_to.first][index_to.second];
        mark_from = neuron_from->getInnovNumber();
        mark_to = neuron_to->getInnovNumber();

        num_attempts++;
        if (num_attempts > max_attempts)
        {
          return false;
        }
      }
      std::normal_distribution< double > normal(0, sigma);
      add_connection(mark_from,
                     mark_to,
                     normal(generator),
                     genotype,
                     "");
#ifdef CPPNEAT_DEBUG
      if (not genotype->is_valid()) {
          std::cerr << "add connection mutation caused invalid genotye" << std::endl;
          throw std::runtime_error("mutation error");
      }
#endif
      return true;
    }
  }

  /////////////////////////////////////////////////
  std::map< std::string, double > RandomParameters(
          Neuron::NeuronTypeSpec param_specs,
          double sigma)
  {
    std::map< std::string, double > params;
    std::random_device rd;
    std::mt19937 generator(rd());
    std::normal_distribution< double > normal(0, sigma);
    for (Neuron::ParamSpec spec : param_specs.param_specs)
    {
      params[spec.name] = normal(generator);

    }
    return params;
  }

  /////////////////////////////////////////////////
  void Mutator::add_neuron_mutation(
          GeneticEncodingPtr genotype,
          double sigma)
  {
    assert(not genotype->connection_genes_.empty());
    assert(not addable_neurons.empty());
    if (not genotype->is_layered_)
    {
      std::uniform_int_distribution< size_t > choice1(
              0, genotype->connection_genes_.size() - 1);
      auto split_id = choice1(generator);
      auto split = genotype->connection_genes_[split_id];

      auto old_weight = split->weight;
      auto mark_from = split->from_;
      auto mark_to = split->to_;

      genotype->remove_connection_gene(split_id);
      auto neuron_from = boost::dynamic_pointer_cast< NeuronGene >(
              genotype->find_gene_by_in(mark_from))->neuron;
      auto neuron_to = boost::dynamic_pointer_cast< NeuronGene >(
              genotype->find_gene_by_in(mark_to))->neuron;

      std::uniform_int_distribution< size_t > choice2(
              0, addable_neurons.size() - 1);
      auto new_neuron_type = addable_neurons[choice2(generator)];

      auto new_neuron_params = RandomParameters(
              brain_spec[new_neuron_type],
              sigma);

      NeuronPtr neuron_middle
              (new Neuron("augment" + std::to_string(innovationNumber + 1),
                          Neuron::HIDDEN_LAYER,
                          new_neuron_type,
                          new_neuron_params));
      int mark_middle = add_neuron(neuron_middle,
                                   genotype,
                                   split);
      add_connection(mark_from,
                     mark_middle,
                     old_weight,
                     genotype,
                     "");
      add_connection(mark_middle,
                     mark_to,
                     1.0,
                     genotype,
                     "");
    }
    else
    {
      std::uniform_int_distribution< size_t > choice1(
              0, genotype->connection_genes_.size() - 1);
      auto split_id = choice1(generator);
      auto split = genotype->connection_genes_[split_id];

      auto old_weight = split->weight;
      auto mark_from = split->from_;
      auto mark_to = split->to_;

      genotype->remove_connection_gene(split_id);
      auto neuron_from = boost::dynamic_pointer_cast< NeuronGene >(
              genotype->find_gene_by_in(mark_from))->neuron;
      auto neuron_to = boost::dynamic_pointer_cast< NeuronGene >(
              genotype->find_gene_by_in(mark_to))->neuron;

      std::uniform_int_distribution< int > choice2(
              0, addable_neurons.size() - 1);
      auto new_neuron_type = addable_neurons[choice2(generator)];

      auto new_neuron_params = RandomParameters(
              brain_spec[new_neuron_type],
              sigma);

      NeuronPtr neuron_middle(new Neuron(
              "augment" + std::to_string(innovationNumber + 1),
              Neuron::HIDDEN_LAYER,
              new_neuron_type,
              new_neuron_params));
      auto mark_middle = add_neuron(neuron_middle,
                                    genotype,
                                    split);
#ifdef CPPNEAT_DEBUG
      if (not genotype->is_valid()) {
          std::cerr << "add neuron mutation caused invalid genotye1" << std::endl;
          throw std::runtime_error("mutation error");
      }
#endif
      add_connection(mark_from,
                     mark_middle,
                     old_weight,
                     genotype,
                     "");
#ifdef CPPNEAT_DEBUG
      if (not genotype->is_valid()) {
          std::cerr << "add neuron mutation caused invalid genotye2" << std::endl;
          throw std::runtime_error("mutation error");
      }
#endif
      add_connection(mark_middle,
                     mark_to,
                     1.0,
                     genotype,
                     "");
    }
#ifdef CPPNEAT_DEBUG
    if (not genotype->is_valid()) {
        std::cerr << "add neuron mutation caused invalid genotye" << std::endl;
        throw std::runtime_error("mutation error");
    }
#endif
  }

  void
  Mutator::remove_connection_mutation(GeneticEncodingPtr genotype)
  {
    if (genotype->connection_genes_.size() == 0)
    {
      return;
    }
    std::uniform_int_distribution< int > choice(
            0, genotype->connection_genes_.size() - 1);
    genotype->connection_genes_.erase(
            genotype->connection_genes_.begin() + choice(generator));
  }

  void
  Mutator::remove_neuron_mutation(GeneticEncodingPtr genotype)
  {
    std::vector< size_t > hidden_neuron_ids;
    for (size_t i = 0; i < genotype->neuron_genes_.size(); i++)
    {
      if (genotype->neuron_genes_[i]->neuron
                                    ->layer == Neuron::HIDDEN_LAYER)
      {
        hidden_neuron_ids.push_back(i);
      }
    }
    std::uniform_int_distribution< size_t > choice(
            0, hidden_neuron_ids.size() - 1);
    auto gene_id = hidden_neuron_ids[choice(generator)];
    auto neuron_gene = genotype->neuron_genes_[gene_id];
    auto neuron_mark = neuron_gene->getInnovNumber();

    std::vector< int > bad_connections;
    for (unsigned int i = 0; i < genotype->connection_genes_
                                         .size(); i++)
    {
      if (genotype->connection_genes_[i]->from_ == neuron_mark ||
          genotype->connection_genes_[i]->to_ == neuron_mark)
      {
        bad_connections.push_back(i);
      }
    }
    for (size_t i = bad_connections.size() - 1; i >= 0; i--)
    {
      genotype->remove_connection_gene(bad_connections[i]);
    }
    genotype->remonve_neuron_gene(gene_id);
  }

  int Mutator::add_neuron(
          NeuronPtr neuron,
          GeneticEncodingPtr genotype,
          ConnectionGenePtr split)
  {
    auto connection_split_in = split->getInnovNumber();
    std::pair< int, Neuron::Ntype > neuron_pair(
            connection_split_in,
            neuron->neuron_type);
    if (neuron_innovations.find(neuron_pair) != neuron_innovations.end())
    {
      size_t i = 0;
      while (i < neuron_innovations[neuron_pair].size()
             and genotype->find_gene_by_in(neuron_innovations[neuron_pair][i])
                 != nullptr)
      {
        i++;
      }
      //some previous innovation is not are already present in the genome-> add a it
      if (i < neuron_innovations[neuron_pair].size())
      {
        NeuronGenePtr new_neuron_gene(new NeuronGene(
                neuron,
                neuron_innovations[neuron_pair][i],
                true,
                "none",
                -1));
        if (not genotype->is_layered_)
        {
          genotype->add_neuron_gene(new_neuron_gene);
        }
        else
        {
          auto mark_from = split->from_;
          auto mark_to = split->to_;
          auto index_from = genotype->convert_in_to_layer_index(mark_from);
          auto index_to = genotype->convert_in_to_layer_index(mark_to);
          assert(index_from.first < index_to.first);
          genotype->add_neuron_gene(
                  new_neuron_gene,
                  index_from.first + 1,
                  //we need a new layer iff the two layers are "right next to each other"
                  index_from.first + 1 == index_to.first);

        }
        return new_neuron_gene->getInnovNumber();
      }
    }
    //new innovation -> add new neuron with new innovation number
    NeuronGenePtr new_neuron_gene(new NeuronGene(
            neuron,
            ++innovationNumber,
            true,
            "none",
            -1));
    //in base case a new vector is constructed here
    neuron_innovations[neuron_pair].push_back(innovationNumber);
    if (not genotype->is_layered_)
    {
      genotype->add_neuron_gene(new_neuron_gene);
    }
    else
    {
      auto mark_from = split->from_;
      auto mark_to = split->to_;
      auto index_from = genotype->convert_in_to_layer_index(mark_from);
      auto index_to = genotype->convert_in_to_layer_index(mark_to);
      assert(index_from.first < index_to.first);
      genotype->add_neuron_gene(
              new_neuron_gene,
              index_from.first + 1,
              //we need a new layer iff the two layers are "right next to each other"
              index_from.first + 1 == index_to.first);
    }
    return new_neuron_gene->getInnovNumber();
  }

  int Mutator::add_connection(
          int mark_from,
          int mark_to,
          double weight,
          GeneticEncodingPtr genotype,
          std::string socket)
  {
    std::pair< size_t, size_t > innovation_pair(
            mark_from,
            mark_to);
    if (connectionInnovations.find(innovation_pair) != connectionInnovations.end())
    {
      auto found =
              genotype->find_gene_by_in(connectionInnovations[innovation_pair]);
      if (found != nullptr)
      {
        boost::dynamic_pointer_cast< ConnectionGene >(found)->setEnabled(true);
        return connectionInnovations[innovation_pair];
      }
      else
      {
        ConnectionGenePtr new_conn_gene(new ConnectionGene(
                mark_to,
                mark_from,
                weight,
                connectionInnovations[innovation_pair],
                true,
                "none",
                -1,
                socket));
        genotype->add_connection_gene(new_conn_gene);
        return new_conn_gene->getInnovNumber();
      }
    }
    ConnectionGenePtr new_conn_gene(new ConnectionGene(
            mark_to,
            mark_from,
            weight,
            ++innovationNumber,
            true,
            "none",
            -1,
            socket));
    connectionInnovations[innovation_pair] = innovationNumber;
    genotype->add_connection_gene(new_conn_gene);
    return new_conn_gene->getInnovNumber();
  }
}