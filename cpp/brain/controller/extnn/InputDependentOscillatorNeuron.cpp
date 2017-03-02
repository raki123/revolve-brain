#include "InputDependentOscillatorNeuron.h"
#include <cmath>
#include <iostream>

namespace revolve {
namespace brain {


InputDependentOscillatorNeuron::InputDependentOscillatorNeuron(const std::string &id,
                                                               const std::map<std::string, double> &params) :
        Neuron(id)
{
  if (!params.count("rv:period") || !params.count("rv:phase_offset") || !params.count("rv:amplitude")) {
    std::cerr << "An `" << "Oscillator" <<
              "` neuron requires `rv:period`, `rv:phase_offset` and `rv:amplitude` elements." << std::endl;
    throw std::runtime_error("Robot brain error");
  }

  this->period_ = params.find("rv:period")
                        ->second;
  this->phaseOffset_ = params.find("rv:phase_offset")
                             ->second;
  this->gain_ = params.find("rv:amplitude")
                      ->second;
}


double
InputDependentOscillatorNeuron::CalculateOutput(double /*t*/)
{
  double inputValue = 0;

  for (auto it = this->incomingConnections_
                     .begin(); it != this->incomingConnections_
                                         .end(); ++it) {
    auto inConnection = it->second;
    inputValue += inConnection->GetInputNeuron()
                              ->GetOutput() * inConnection->GetWeight();
  }
  return 0.5 * (1.0 + this->gain_ *
                      sin(2.0 * M_PI / (this->period_) * (inputValue - this->period_ * this->phaseOffset_)));
}

std::map<std::string, double>
InputDependentOscillatorNeuron::getNeuronParameters()
{
  std::map<std::string, double> ret;
  ret["rv:period"] = period_;
  ret["rv:phase_offset"] = phaseOffset_;
  ret["rv:amplitude"] = gain_;
  return ret;
}

void
InputDependentOscillatorNeuron::setNeuronParameters(std::map<std::string, double> params)
{
  if (!params.count("rv:period") || !params.count("rv:phase_offset") || !params.count("rv:amplitude")) {
    std::cerr << "An `" << "Oscillator" <<
              "` neuron requires `rv:period`, `rv:phase_offset` and `rv:amplitude` elements." << std::endl;
    throw std::runtime_error("Robot brain error");
  }

  this->period_ = params.find("rv:period")
                        ->second;
  this->phaseOffset_ = params.find("rv:phase_offset")
                             ->second;
  this->gain_ = params.find("rv:amplitude")
                      ->second;
}


std::string
InputDependentOscillatorNeuron::getType()
{
  return "InputDependentOscillator";
}


}
}