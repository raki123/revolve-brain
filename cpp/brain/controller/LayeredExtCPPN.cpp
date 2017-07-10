#include "LayeredExtCPPN.h"

#include <fstream>
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>


namespace revolve {
namespace brain {


LayeredExtNNController::LayeredExtNNController(std::string modelName,
                                               boost::shared_ptr<LayeredExtNNConfig> Config,
                                               const std::vector<ActuatorPtr> &actuators,
                                               const std::vector<SensorPtr> &sensors)
{
  modelName_ = modelName;

  layers_ = Config->layers_;
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


LayeredExtNNController::~LayeredExtNNController()
{
  delete[] inputs_;
  delete[] outputs_;
}


void
LayeredExtNNController::update(const std::vector<ActuatorPtr> &actuators,
                               const std::vector<SensorPtr> &sensors,
                               double t,
                               double step)
{
  // Read sensor data into the input buffer
  size_t p = 0;
  for (auto sensor : sensors) {
    sensor->read(&inputs_[p]);
    p += sensor->inputs();
  }

  // Feed inputs into the input neurons
  for (auto it = layers_[0].begin(); it != layers_[0].end(); ++it) {
    auto inNeuron = *it;
    int pos = inputPositionMap_[inNeuron];
    inNeuron->SetInput(inputs_[pos]);
  }

  // Calculate new states of all neurons
  for (std::vector<NeuronPtr> layer : layers_) {
    for (NeuronPtr neuron : layer) {
      neuron->Update(t);
    }
    for (NeuronPtr neuron : layer) {
      neuron->FlipState();
    }
  }

  for (auto it = layers_[layers_.size() - 1].begin(); it != layers_[layers_.size() - 1].end(); ++it) {
    auto outNeuron = *it;
    int pos = outputPositionMap_[outNeuron];
    outputs_[pos] = outNeuron->GetOutput();
  }

  // Send new signals to the actuators
  p = 0;
  for (auto actuator: actuators) {
    actuator->update(&outputs_[p],
                     step);
    p += actuator->outputs();
  }
}

boost::shared_ptr<LayeredExtNNConfig>
LayeredExtNNController::getGenotype()
{
  boost::shared_ptr<LayeredExtNNConfig> Config(new LayeredExtNNConfig());
  Config->layers_ = layers_;
  Config->outputPositionMap_ = outputPositionMap_;
  Config->inputPositionMap_ = inputPositionMap_;
  Config->idToNeuron_ = idToNeuron_;
  Config->connections_ = connections_;
  return Config;
}

void
LayeredExtNNController::setGenotype(boost::shared_ptr<LayeredExtNNConfig> Config)
{
  layers_ = Config->layers_;
  outputPositionMap_ = Config->outputPositionMap_;
  inputPositionMap_ = Config->inputPositionMap_;
  idToNeuron_ = Config->idToNeuron_;
  connections_ = Config->connections_;
}

void
LayeredExtNNController::writeNetwork(std::ofstream &write_to)
{
  std::vector<NeuronPtr> allNeurons_;
  for (auto v : layers_) {
    allNeurons_.insert(allNeurons_.end(),
                       v.begin(),
                       v.end());
  }
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
