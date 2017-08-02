#include "BiasNeuron.h"
#include <iostream>

namespace revolve {
namespace brain {


BiasNeuron::BiasNeuron(const std::string &id,
                       const std::map<std::string, double> &params) :
        Neuron(id)
{
  if (not params.count("rv:bias")) {
    std::cerr << "A `" << "Bias" << "` neuron requires `rv:bias` element." << std::endl;
    throw std::runtime_error("Robot brain error");
  }
  this->bias_ = params.find("rv:bias")
                      ->second;
}


double
BiasNeuron::CalculateOutput(double /*t*/)
{
  return this->bias_;
}

std::map<std::string, double>
BiasNeuron::getNeuronParameters()
{
  std::map<std::string, double> ret;
  ret["rv:bias"] = bias_;
  return ret;
}

void
BiasNeuron::setNeuronParameters(std::map<std::string, double> params)
{
  if (not params.count("rv:bias")) {
    std::cerr << "A `" << "Bias" << "` neuron requires `rv:bias` element." << std::endl;
    throw std::runtime_error("Robot brain error");
  }
  this->bias_ = params.find("rv:bias")
                      ->second;
}

std::string
BiasNeuron::getType()
{
  return "Bias";
}


}
}