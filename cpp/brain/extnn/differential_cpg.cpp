#include "differential_cpg.h"

#include <cmath>
#include <iostream>
#include <sstream>

namespace revolve {
namespace brain {



DifferentialCPG::DifferentialCPG(const std::string &id,
				 const std::map<std::string, double> &params):
Neuron(id)
{
	if (!params.count("rv:bias")) {
		std::cerr << "A `" << "Differential CPG" << "` neuron requires `rv:bias` element." << std::endl;
		throw std::runtime_error("Robot brain error");
	}
	double min_value=-1.0, max_value=1.0;
	this->bias_ = min_value + (max_value-min_value)*params.find("rv:bias")->second;
	lastTime_ = 0;
}


double DifferentialCPG::CalculateOutput(double t)
{
	double deltaT = t - lastTime_;
	lastTime_ = t;

	if (deltaT > 0.1) {
		deltaT = 0.1;

	}

	double inputValue = 0;

	for (auto it = this->incomingConnections_.begin(); it != this->incomingConnections_.end(); ++it)
	{
		auto inConnection = it->second;
		inputValue += inConnection->GetInputNeuron()->GetOutput() * inConnection->GetWeight();
	}

	double state_deriv = inputValue - this->bias_;
	double result = this->output_ + deltaT * state_deriv;

	double maxOut = 10000.0;

	// limit output:
	// if (result > maxOut) {
	// 	result = maxOut;
	// }
	// else if (result < -maxOut) {
	// 	result = -maxOut;
	// }

	// saturate output:
	double gain = 2.0 / maxOut;
	result = (2.0 / (1.0 + std::exp(-result * gain)) - 1.0) * maxOut;

	return result;
}


}
}