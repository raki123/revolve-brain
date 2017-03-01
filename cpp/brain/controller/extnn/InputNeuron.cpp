#include "InputNeuron.h"

namespace revolve
{
namespace brain
{


InputNeuron::InputNeuron(const std::string &id,
                         const std::map<std::string, double> &params) :
        Neuron(id)
{
  input_ = 0;
}

double
InputNeuron::CalculateOutput(double /*t*/)
{
  return input_;
}

void
InputNeuron::SetInput(double value)
{
  input_ = value;
}

std::map<std::string, double>
InputNeuron::getNeuronParameters()
{
  std::map<std::string, double> ret;
  return ret;
}

void
InputNeuron::setNeuronParameters(std::map<std::string, double> params)
{

}


std::string
InputNeuron::getType()
{
  return "Input";
}

}
}