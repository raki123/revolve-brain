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
          , sigma_(new_connection_sigma)
          , innovationNumber_(innovation_number)
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
  void Mutator::make_starting_genotype_known(GeneticEncodingPtr _genotype)
  {
    for (const auto &connection : _genotype->connections_)
    {
      this->connectionInnovations_[{connection->from_, connection->to_}] =
              connection->InnovationNumber();
    }
  }

  /////////////////////////////////////////////////
  void Mutator::RecordInnovations(const std::string &_path)
  {
    std::ofstream outputFile;
    outputFile.open(_path,
                    std::ios::out | std::ios::trunc);
    outputFile << "- in_no: " << innovationNumber_ << std::endl;
    outputFile << "- connection_innovations: " << std::endl;
    for (const auto &connection : connectionInnovations_)
    {
      outputFile << "  - connection_innovation: " << std::endl;
      outputFile
              << "      mark_from: "
              << connection.first.first
              << std::endl;
      outputFile
              << "      mark_to: "
              << connection.first.second
              << std::endl;
      outputFile
              << "      in_no: "
              << connection.second
              << std::endl;
    }

    outputFile << "- neuron_innovations: " << std::endl;
    for (const auto &neuron : neuronInnovations_)
    {
      outputFile << "  - neuron_innovation: " << std::endl;
      outputFile
              << "      conn_split: "
              << neuron.first.first
              << std::endl;
      outputFile << "      ntype: " << neuron.first.second << std::endl;
      outputFile << "      in_nos: " << std::endl;
      for (int in_no : neuron.second)
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
    innovationNumber_ = yaml_file[0]["in_no"].as< int >();
    connectionInnovations_.clear();
    for (const auto &connection : yaml_file[1]["connection_innovations"])
    {
      auto innovation = connection["connection_innovation"];
      auto from = innovation["mark_from"].as< int >();
      auto to = innovation["mark_to"].as< int >();
      auto inNum = innovation["in_no"].as< int >();
      connectionInnovations_.insert({{from, to}, inNum});
    }
    neuronInnovations_.clear();
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
      neuronInnovations_.insert({{split, nType}, inNums});
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

  void Mutator::MutateNeuronParams(
          GeneticEncodingPtr genotype,
          double probability,
          double sigma)
  {
    std::uniform_real_distribution< double > uniform(0, 1);
    if (not genotype->is_layered_)
    {
      for (const auto &neuron_gene : genotype->neurons_)
      {
        if (uniform(generator) < probability)
        {
          auto neuron_params =
                  brain_spec[neuron_gene->neuron->type_].param_specs;
          if (not neuron_params.empty())
          {
            std::uniform_int_distribution< size_t > uniform_int(
                    0, neuron_params.size() - 1);
            auto param = neuron_params[uniform_int(generator)];
            auto cur_val = neuron_gene->neuron->neuron_params[param.name];
            std::normal_distribution< double > normal(0, sigma);
            cur_val += normal(generator);
            neuron_gene->neuron->SetParameters(cur_val, param);
          }
        }
      }
    }
    else
    {
      for (const auto &layer : genotype->layers_)
      {
        for (const auto &neuron_gene : layer)
        {
          if (uniform(generator) < probability)
          {
            auto neuron_params =
                    brain_spec[neuron_gene->neuron->type_].param_specs;
            if (not neuron_params.empty())
            {
              std::uniform_int_distribution< size_t > uniform_int(
                      0, neuron_params.size() - 1);
              auto param = neuron_params[uniform_int(generator)];
              auto cur_val = neuron_gene->neuron->neuron_params[param.name];
              std::normal_distribution< double > normal(0, sigma);
              cur_val += normal(generator);
              neuron_gene->neuron->SetParameters(cur_val, param);
            }
          }
        }
      }
    }
  }

  /////////////////////////////////////////////////
  void Mutator::MutateWeights(
          GeneticEncodingPtr genotype,
          double probability,
          double sigma)
  {
    std::uniform_real_distribution< double > uniform(0, 1);
    std::normal_distribution< double > normal(0, sigma);
    for (const auto &connection_gene : genotype->connections_)
    {
      if (uniform(generator) < probability)
      {
        connection_gene->weight += normal(generator);
      }
    }
  }

  /////////////////////////////////////////////////
  void Mutator::MutateStructure(
          GeneticEncodingPtr genotype,
          double probability)
  {
    std::uniform_real_distribution< double > uniform(0, 1);
    if (uniform(generator) < probability)
    {
      if (genotype->connections_.empty())
      {
        MutateConnection(genotype, this->sigma_);
      }
      else
      {
        if (uniform(generator) < 0.5)
        {
          MutateConnection(genotype, this->sigma_);
        }
        else
        {
          MutateNeuron(genotype, this->sigma_);
        }
      }
    }
  }

  /////////////////////////////////////////////////
  bool Mutator::MutateConnection(
          GeneticEncodingPtr genotype,
          double sigma)
  {
    if (not genotype->is_layered_)
    {
      std::uniform_int_distribution< size_t > choice(
              0, genotype->neurons_.size() - 1);
      auto from = genotype->neurons_[choice(generator)];
      auto to = genotype->neurons_[choice(generator)];
      auto markFrom = from->InnovationNumber();
      auto markTo = to->InnovationNumber();

      int num_attempts = 1;

      while (genotype->connection_exists(markFrom, markTo)
             or to->neuron->layer == Neuron::INPUT_LAYER)
      {
        from = genotype->neurons_[choice(generator)];
        to = genotype->neurons_[choice(generator)];
        markFrom = from->InnovationNumber();
        markTo = to->InnovationNumber();

        num_attempts++;
        if (num_attempts > max_attempts)
        {
          return false;
        }
      }
      std::normal_distribution< double > normal(0, sigma);
      this->AddConnection(
              markFrom,
              markTo,
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
      auto mark_from = neuron_from->InnovationNumber();
      auto mark_to = neuron_to->InnovationNumber();

      int num_attempts = 1;

      while (genotype->connection_exists(mark_from, mark_to)
             or index_from.first >= index_to.first)
      {
        index_from = genotype->convert_index_to_layer_index(choice(generator));
        index_to = genotype->convert_index_to_layer_index(choice(generator));
        neuron_from = genotype->layers_[index_from.first][index_from.second];
        neuron_to = genotype->layers_[index_to.first][index_to.second];
        mark_from = neuron_from->InnovationNumber();
        mark_to = neuron_to->InnovationNumber();

        num_attempts++;
        if (num_attempts > max_attempts)
        {
          return false;
        }
      }
      std::normal_distribution< double > normal(0, sigma);
      AddConnection(mark_from,
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
  void Mutator::MutateNeuron(
          GeneticEncodingPtr genotype,
          double sigma)
  {
    assert(not genotype->connections_.empty());
    assert(not addable_neurons.empty());
    if (not genotype->is_layered_)
    {
      std::uniform_int_distribution< size_t > choice1(
              0, genotype->connections_.size() - 1);
      auto split_id = choice1(generator);
      auto split = genotype->connections_[split_id];

      auto old_weight = split->weight;
      auto mark_from = split->from_;
      auto mark_to = split->to_;

      genotype->remove_connection_gene(split_id);
      auto neuron_from = boost::dynamic_pointer_cast< NeuronGene >(
              genotype->Find(mark_from))->neuron;
      auto neuron_to = boost::dynamic_pointer_cast< NeuronGene >(
              genotype->Find(mark_to))->neuron;

      std::uniform_int_distribution< size_t > choice2(
              0, addable_neurons.size() - 1);
      auto new_neuron_type = addable_neurons[choice2(generator)];

      auto new_neuron_params = RandomParameters(
              brain_spec[new_neuron_type],
              sigma);

      NeuronPtr neuron_middle
              (new Neuron("augment" + std::to_string(innovationNumber_ + 1),
                          Neuron::HIDDEN_LAYER,
                          new_neuron_type,
                          new_neuron_params));
      int mark_middle = AddNeuron(neuron_middle,
                                  genotype,
                                  split);
      AddConnection(mark_from,
                    mark_middle,
                    old_weight,
                    genotype,
                    "");
      AddConnection(mark_middle,
                    mark_to,
                    1.0,
                    genotype,
                    "");
    }
    else
    {
      std::uniform_int_distribution< size_t > choice1(
              0, genotype->connections_.size() - 1);
      auto split_id = choice1(generator);
      auto split = genotype->connections_[split_id];

      auto old_weight = split->weight;
      auto mark_from = split->from_;
      auto mark_to = split->to_;

      genotype->remove_connection_gene(split_id);
      auto neuron_from = boost::dynamic_pointer_cast< NeuronGene >(
              genotype->Find(mark_from))->neuron;
      auto neuron_to = boost::dynamic_pointer_cast< NeuronGene >(
              genotype->Find(mark_to))->neuron;

      std::uniform_int_distribution< int > choice2(
              0, addable_neurons.size() - 1);
      auto new_neuron_type = addable_neurons[choice2(generator)];

      auto new_neuron_params = RandomParameters(
              brain_spec[new_neuron_type],
              sigma);

      NeuronPtr neuron_middle(new Neuron(
              "augment" + std::to_string(innovationNumber_ + 1),
              Neuron::HIDDEN_LAYER,
              new_neuron_type,
              new_neuron_params));
      auto mark_middle = AddNeuron(neuron_middle,
                                   genotype,
                                   split);
#ifdef CPPNEAT_DEBUG
      if (not genotype->is_valid()) {
          std::cerr << "add neuron mutation caused invalid genotye1" << std::endl;
          throw std::runtime_error("mutation error");
      }
#endif
      AddConnection(mark_from,
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
      AddConnection(mark_middle,
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

  void Mutator::RemoveConnection(GeneticEncodingPtr genotype)
  {
    if (genotype->connections_.size() == 0)
    {
      return;
    }
    std::uniform_int_distribution< int > choice(
            0, genotype->connections_.size() - 1);
    genotype->connections_.erase(
            genotype->connections_.begin() + choice(generator));
  }

  void Mutator::RemoveNeuron(GeneticEncodingPtr genotype)
  {
    std::vector< size_t > hidden_neuron_ids;
    for (size_t i = 0; i < genotype->neurons_.size(); i++)
    {
      if (genotype->neurons_[i]->neuron
                                    ->layer == Neuron::HIDDEN_LAYER)
      {
        hidden_neuron_ids.push_back(i);
      }
    }
    std::uniform_int_distribution< size_t > choice(
            0, hidden_neuron_ids.size() - 1);
    auto gene_id = hidden_neuron_ids[choice(generator)];
    auto neuron_gene = genotype->neurons_[gene_id];
    auto neuron_mark = neuron_gene->InnovationNumber();

    std::vector< int > bad_connections;
    for (unsigned int i = 0; i < genotype->connections_
                                         .size(); i++)
    {
      if (genotype->connections_[i]->from_ == neuron_mark ||
          genotype->connections_[i]->to_ == neuron_mark)
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

  int Mutator::AddNeuron(
          NeuronPtr neuron,
          GeneticEncodingPtr genotype,
          ConnectionGenePtr split)
  {
    auto connection_split_in = split->InnovationNumber();
    std::pair< int, Neuron::Ntype > neuron_pair(
            connection_split_in,
            neuron->type_);
    if (neuronInnovations_.find(neuron_pair) != neuronInnovations_.end())
    {
      size_t i = 0;
      while (i < neuronInnovations_[neuron_pair].size()
             and genotype->Find(neuronInnovations_[neuron_pair][i])
                 not_eq nullptr)
      {
        i++;
      }
      //some previous innovation is not are already present in the genome-> add a it
      if (i < neuronInnovations_[neuron_pair].size())
      {
        NeuronGenePtr new_neuron_gene(new NeuronGene(
                neuron,
                neuronInnovations_[neuron_pair][i],
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
        return new_neuron_gene->InnovationNumber();
      }
    }
    //new innovation -> add new neuron with new innovation number
    NeuronGenePtr new_neuron_gene(new NeuronGene(
            neuron,
            ++innovationNumber_,
            true,
            "none",
            -1));
    //in base case a new vector is constructed here
    neuronInnovations_[neuron_pair].push_back(innovationNumber_);
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
    return new_neuron_gene->InnovationNumber();
  }

  int Mutator::AddConnection(
          int mark_from,
          int mark_to,
          double weight,
          GeneticEncodingPtr genotype,
          std::string socket)
  {
    std::pair< size_t, size_t > innovation_pair(
            mark_from,
            mark_to);
    if (connectionInnovations_.find(innovation_pair) != connectionInnovations_.end())
    {
      auto found =
              genotype->Find(connectionInnovations_[innovation_pair]);
      if (found != nullptr)
      {
        boost::dynamic_pointer_cast< ConnectionGene >(found)->setEnabled(true);
        return connectionInnovations_[innovation_pair];
      }
      else
      {
        ConnectionGenePtr new_conn_gene(new ConnectionGene(
                mark_to,
                mark_from,
                weight,
                connectionInnovations_[innovation_pair],
                true,
                "none",
                -1,
                socket));
        genotype->add_connection_gene(new_conn_gene);
        return new_conn_gene->InnovationNumber();
      }
    }
    ConnectionGenePtr new_conn_gene(new ConnectionGene(
            mark_to,
            mark_from,
            weight,
            ++innovationNumber_,
            true,
            "none",
            -1,
            socket));
    connectionInnovations_[innovation_pair] = innovationNumber_;
    genotype->add_connection_gene(new_conn_gene);
    return new_conn_gene->InnovationNumber();
  }
}