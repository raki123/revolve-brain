#include "DifferentialCPG.h"

#include <cmath>
#include <iostream>
#include <sstream>

namespace revolve {
namespace brain {

DifferentialCPG::DifferentialCPG(const std::string &id,
                                 const std::map<std::string, double> &params) :
        Neuron(id)
{
  if (!params.count("rv:bias")) {
    std::cerr << "A `" << "Differential CPG" << "` neuron requires `rv:bias` element." << std::endl;
    throw std::runtime_error("Robot brain error");
  }
  this->bias_ = params.find("rv:bias")
                      ->second;
  lastTime_ = 0;
}


double
DifferentialCPG::CalculateOutput(double t)
{
  double deltaT = t - lastTime_;
  lastTime_ = t;

  if (deltaT > 0.1) {
    deltaT = 0.1;
  }

  double inputValue = 0;

  for (auto it = this->incomingConnections_.begin(); it != this->incomingConnections_.end(); ++it) {
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

std::map<std::string, double>
DifferentialCPG::getNeuronParameters()
{
  std::map<std::string, double> ret;
  ret["rv:bias"] = bias_;
  return ret;
}

void
DifferentialCPG::setNeuronParameters(std::map<std::string, double> params)
{
  if (!params.count("rv:bias")) {
    std::cerr << "A `" << "Differential CPG" << "` neuron requires `rv:bias` element." << std::endl;
    throw std::runtime_error("Robot brain error");
  }
  this->bias_ = params.find("rv:bias")
                      ->second;
}


std::string
DifferentialCPG::getType()
{
  return "DifferentialCPG";
}


}
}