#include "linear_neuron.h"
#include <sstream>
#include <iostream>

namespace revolve {
namespace brain {


LinearNeuron::LinearNeuron(const std::string &id,
			   const std::map<std::string, double> &params):
Neuron(id)
{
	if (!params.count("rv:bias") || !params.count("rv:gain")) {
		std::cerr << "A `" << "Simple" << "` neuron requires `rv:bias` and `rv:gain` elements." << std::endl;
		throw std::runtime_error("Robot brain error");
	}
	double min_value = -1, max_value = 1;
	this->bias_ = min_value + (max_value-min_value)*params.find("rv:bias")->second;
	min_value = 0, max_value = 1;
	this->gain_ = min_value + (max_value-min_value)*params.find("rv:gain")->second;
}


double LinearNeuron::CalculateOutput(double t)
{
	double inputValue = 0;

	for (auto it = this->incomingConnections_.begin(); it != this->incomingConnections_.end(); ++it)
	{
		auto inConnection = it->second;
		inputValue += inConnection->GetInputNeuron()->GetOutput() * inConnection->GetWeight();
	}

	double result = this->gain_ * (inputValue - this->bias_);

	return result;
}


}
}