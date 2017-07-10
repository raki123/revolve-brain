#include "ExtCPPNWeights.h"

#include <fstream>
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>

namespace revolve {
namespace brain {


ExtNNController::ExtNNController(std::string modelName,
                                 boost::shared_ptr<CPPNConfig> Config,
                                 const std::vector<ActuatorPtr> &actuators,
                                 const std::vector<SensorPtr> &sensors)
{
  modelName_ = modelName;

  allNeurons_ = Config->allNeurons_;
  inputNeurons_ = Config->inputNeurons_;
  outputNeurons_ = Config->outputNeurons_;
  hiddenNeurons_ = Config->hiddenNeurons_;
  outputPositionMap_ = Config->outputPositionMap_;
  inputPositionMap_ = Config->inputPositionMap_;
  idToNeuron_ = Config->idToNeuron_;
  connections_ = Config->connections_;
  size_t p = 0;
  for (auto sensor : sensors) {
    p += sensor->inputs();
  }
  inputs_ = new double[p];
  p = 0;
  for (auto actuator : actuators) {
    p += actuator->outputs();
  }
  outputs_ = new double[p];
}


ExtNNController::~ExtNNController()
{
  delete[] inputs_;
  delete[] outputs_;
}


void
ExtNNController::update(const std::vector<ActuatorPtr> &actuators,
                        const std::vector<SensorPtr> &sensors,
                        double t,
                        double step)
{
//   	std::cout << "yay! \n";
  //boost::mutex::scoped_lock lock(networkMutex_);

  // Read sensor data into the input buffer
  size_t p = 0;
  for (auto sensor : sensors) {
    sensor->read(&inputs_[p]);
    p += sensor->inputs();
  }

  // Feed inputs into the input neurons
  for (auto it = inputNeurons_.begin(); it != inputNeurons_.end(); ++it) {
    auto inNeuron = *it;
    int pos = inputPositionMap_[inNeuron];
    inNeuron->SetInput(inputs_[pos]);
  }

  // Calculate new states of all neurons
  for (auto it = allNeurons_.begin(); it != allNeurons_.end(); ++it) {
    (*it)->Update(t);
  }


  // Flip states of all neurons
  for (auto it = allNeurons_.begin(); it != allNeurons_.end(); ++it) {
    (*it)->FlipState();
  }

  // std::ofstream debF;
  // debF.open("/home/dmitry/projects/debug/debug_signals", std::ofstream::out | std::ofstream::app);
  for (auto it = outputNeurons_.begin(); it != outputNeurons_.end(); ++it) {
    auto outNeuron = *it;
    int pos = outputPositionMap_[outNeuron];
    outputs_[pos] = outNeuron->GetOutput();

    // debF << pos << "," << outputs_[pos] << std::endl;
  }

  // debF.close();

  // Send new signals to the actuators
  p = 0;
  for (auto actuator: actuators) {
    actuator->update(&outputs_[p],
                     step);
    p += actuator->outputs();
  }
}

std::vector<double> ExtNNController::getPhenotype()
{
  //weights
  std::vector<double> ret(connections_.size(),
                          0);
  for (size_t i = 0; i < connections_.size(); i++) {
    ret[i] = connections_[i]->GetWeight();
  }
  //neuron_parameters
  for (size_t i = 0; i < allNeurons_.size(); i++) {
    //iterator over map is ordered, therefore we always return the same parameter in the same place
    std::map<std::string, double> params = allNeurons_[i]->getNeuronParameters();
    for (auto it = params.begin(); it != params.end(); ++it) {
      ret.push_back(it->second);
    }
  }
  return ret;
}

void ExtNNController::setPhenotype(std::vector<double> weights)
{

  size_t matches = connections_.size();
  for (size_t i = 0; i < connections_.size(); i++) {
    connections_[i]->SetWeight(weights[i]);
  }
  //neuron_parameters
  for (size_t i = 0; i < allNeurons_.size(); i++) {
    //iterator over map is ordered, therefore we always return the same parameter in the same place
    std::map<std::string, double> params = allNeurons_[i]->getNeuronParameters();
    for (auto it = params.begin(); it != params.end(); ++it) {
      params[it->first] = weights[connections_.size() + i];
      matches++;
    }
    allNeurons_[i]->setNeuronParameters(params);
  }
  if (weights.size() != matches) {
    std::cerr << "incorrect amount of weights (" << weights.size() << ") delivered. expected " << matches << std::endl;
    throw std::runtime_error("Weight size error");
  }
  for (NeuronPtr neuron: allNeurons_) {
    neuron->reset();
  }
}

void ExtNNController::writeNetwork(std::ofstream &write_to)
{
  boost::adjacency_list<> graph(allNeurons_.size());
  for (size_t i = 0; i < allNeurons_.size(); i++) {
    std::vector<std::pair<std::string, NeuralConnectionPtr>> connectionsToAdd = allNeurons_[i]->getIncomingConnections();
    for (std::pair<std::string, NeuralConnectionPtr> connectionToAdd : connectionsToAdd) {
      NeuronPtr input = connectionToAdd.second
                                       ->GetInputNeuron();
      int indexInput = std::find(allNeurons_.begin(),
                                 allNeurons_.end(),
                                 input) - allNeurons_.begin();
      boost::add_edge(indexInput,
                      i,
                      graph);
    }
  }
  std::string *names = new std::string[allNeurons_.size()];
  for (size_t i = 0; i < allNeurons_.size(); i++) {
    std::stringstream nodeName;
    nodeName << allNeurons_[i]->Id() + " of type: " + allNeurons_[i]->getType() << std::endl;
    for (std::pair<std::string, double> param : allNeurons_[i]->getNeuronParameters()) {
      nodeName << param.first << ": " << param.second << std::endl;
    }
    names[i] = nodeName.str();
  }
  //, "test");
  boost::write_graphviz(write_to,
                        graph,
                        boost::make_label_writer(names));
  delete[] names;
}
}
}
