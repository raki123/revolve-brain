#include "RafCPGController.h"

#include <fstream>
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>


namespace revolve {
namespace brain {


RafCPGController::RafCPGController(std::string model_name,
                                   CPPNConfigPtr _config,
                                   const std::vector<ActuatorPtr> &actuators,
                                   const std::vector<SensorPtr> &sensors)
{
  modelName_ = model_name;

  allNeurons_ = _config->allNeurons_;
  inputNeurons_ = _config->inputNeurons_;
  outputNeurons_ = _config->outputNeurons_;
  hiddenNeurons_ = _config->hiddenNeurons_;
  outputPositionMap_ = _config->outputPositionMap_;
  inputPositionMap_ = _config->inputPositionMap_;
  idToNeuron_ = _config->idToNeuron_;
  connections_ = _config->connections_;

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

RafCPGController::~RafCPGController()
{
  delete[] inputs_;
  delete[] outputs_;
}

void RafCPGController::update(const std::vector<ActuatorPtr> &actuators,
                              const std::vector<SensorPtr> &sensors,
                              double t,
                              double step)
{
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

  for (auto it = outputNeurons_.begin(); it != outputNeurons_.end(); ++it) {
    auto outNeuron = *it;
    int pos = outputPositionMap_[outNeuron];
    outputs_[pos] = outNeuron->GetOutput();
  }

  // Send new signals to the actuators
  p = 0;
  for (auto actuator: actuators) {
    actuator->update(&outputs_[p], step);
    p += actuator->outputs();
  }
}

CPPNConfigPtr RafCPGController::getPhenotype()
{
  boost::shared_ptr<CPPNConfig> config(new CPPNConfig());
  config->allNeurons_ = allNeurons_;
  config->inputNeurons_ = inputNeurons_;
  config->outputNeurons_ = outputNeurons_;
  config->hiddenNeurons_ = hiddenNeurons_;
  config->outputPositionMap_ = outputPositionMap_;
  config->inputPositionMap_ = inputPositionMap_;
  config->idToNeuron_ = idToNeuron_;
  config->connections_ = connections_;

  return config;
}

void RafCPGController::setPhenotype(CPPNConfigPtr _config)
{
  allNeurons_ = _config->allNeurons_;
  for (NeuronPtr neuron: allNeurons_) {
    neuron->reset();
  }
  inputNeurons_ = _config->inputNeurons_;
  outputNeurons_ = _config->outputNeurons_;
  hiddenNeurons_ = _config->hiddenNeurons_;
  outputPositionMap_ = _config->outputPositionMap_;
  inputPositionMap_ = _config->inputPositionMap_;
  idToNeuron_ = _config->idToNeuron_;
  connections_ = _config->connections_;
}

void RafCPGController::writeNetwork(std::ofstream &write_to)
{
  boost::adjacency_list<> graph(allNeurons_.size());
  for (size_t i = 0; i < allNeurons_.size(); i++) {
    std::vector<std::pair<std::string, NeuralConnectionPtr>> connectionsToAdd = allNeurons_[i]->getIncomingConnections();
    for (std::pair<std::string, NeuralConnectionPtr> connectionToAdd : connectionsToAdd) {
      NeuronPtr input = connectionToAdd.second->GetInputNeuron();
      long indexInput = std::find(allNeurons_.begin(), allNeurons_.end(), input) - allNeurons_.begin();
      boost::add_edge(indexInput, i, graph);
    }
  }
  std::string *names = new std::string[allNeurons_.size()];
  for (size_t i = 0; i < allNeurons_.size(); ++i) {
    std::stringstream nodeName;
    nodeName << allNeurons_[i]->Id() + " of type: " + allNeurons_[i]->getType() << std::endl;
    for (std::pair<std::string, double> param : allNeurons_[i]->getNeuronParameters()) {
      nodeName << param.first << ": " << param.second << std::endl;
    }
    names[i] = nodeName.str();
  }

  boost::write_graphviz(write_to, graph, boost::make_label_writer(names));
  delete[] names;
}

}
}
