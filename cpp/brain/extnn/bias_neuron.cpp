#include "bias_neuron.h"
#include <iostream>

namespace revolve {
namespace brain {


BiasNeuron::BiasNeuron(const std::string &id,
		       const std::map<std::string, double> &params):
Neuron(id)
{
	if (!params.count("rv:bias")) {
		std::cerr << "A `" << "Bias" << "` neuron requires `rv:bias` element." << std::endl;
		throw std::runtime_error("Robot brain error");
	}
	double min_value=-1.0, max_value=1.0;
	this->bias_ = min_value + (max_value-min_value)*params.find("rv:bias")->second;
}


double BiasNeuron::CalculateOutput(double /*t*/)
{
	return this->bias_;
}


}
}