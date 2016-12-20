#include "ext_nn_weights.h"

#include <fstream>
#include <iostream>


namespace revolve {
namespace brain {


ExtNNController::ExtNNController(std::string modelName,
					     ExtNNConfig Config,
					     EvaluatorPtr evaluator,
					     const std::vector< ActuatorPtr > & actuators,
					     const std::vector< SensorPtr > & sensors)
{
	modelName_ = modelName;
	
	evaluator_ = evaluator;
	inputs_ = Config.inputs_;
	outputs_ = Config.outputs_;
	allNeurons_ = Config.allNeurons_;
	inputNeurons_ = Config.inputNeurons_;
	outputNeurons_ = Config.outputNeurons_;
	hiddenNeurons_ = Config.hiddenNeurons_;
	outputPositionMap_ = Config.outputPositionMap_;
	inputPositionMap_ = Config.inputPositionMap_;
	idToNeuron_ = Config.idToNeuron_;
	connections_ = Config.connections_;
	numInputNeurons_ = Config.numInputNeurons_;
	numOutputNeurons_ = Config.numOutputNeurons_;
	numHiddenNeurons_ = Config.numHiddenNeurons_;
}


ExtNNController::~ExtNNController()
{
	delete [] inputs_;
	delete [] outputs_;
}




void ExtNNController::update(const std::vector<ActuatorPtr>& actuators,
				   const std::vector<SensorPtr>& sensors,
				   double t,
				   double step) 
{
//   	std::cout << "yay! \n";
	//boost::mutex::scoped_lock lock(networkMutex_);

	// Read sensor data into the input buffer
	unsigned int p = 0;
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
		actuator->update(&outputs_[p], step);
		p += actuator->outputs();
	}
}

std::vector< double > ExtNNController::getGenome()
{
	std::vector<double> ret(connections_.size(), 0);
	for(unsigned int i = 0; i < connections_.size(); i++) {
		ret[i] = connections_[i]->GetWeight();
	}
	return ret;
}

void ExtNNController::setGenome(std::vector< double > weights)
{
	if(weights.size() != connections_.size()) {
		std::cerr << "incorrect amount of weights (" << weights.size() << ") delivered. expected " << connections_.size() << std::endl;
		throw std::runtime_error("Weight size error");
	}
	for(unsigned int i = 0; i < weights.size(); i++) {
		connections_[i]->SetWeight(weights[i]);
	}
}
}
}
