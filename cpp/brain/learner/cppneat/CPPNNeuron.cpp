#include "CPPNNeuron.h"

namespace CPPNEAT {

Neuron::Neuron(Neuron &copy_of)
        :
        neuron_id(copy_of.neuron_id)
        , layer(copy_of.layer)
        , neuron_type(copy_of.neuron_type)
        , neuron_params(copy_of.neuron_params)
{

}

Neuron::Neuron(std::string neuron_id,
               Layer layer,
               Ntype neuron_type,
               std::map<std::string, double> neuron_params)
        :
        neuron_id(neuron_id)
        , layer(layer)
        , neuron_type(neuron_type)
        , neuron_params(neuron_params)
{

}

void
Neuron::set_neuron_param(double value,
                         ParamSpec param_spec)
{
// 	if(value > param_spec.max_value) {
// 		if(param_spec.max_inclusive) {
// 			value = param_spec.max_value;
// 		}
// 		else {
// 			value = param_spec.max_value - param_spec.epsilon;
// 		}
// 	}
// 	if(value < param_spec.min_value) {
// 		if(param_spec.min_inclusive) {
// 			value = param_spec.min_value;
// 		}
// 		else {
// 			value = param_spec.min_value + param_spec.epsilon;
// 		}
// 	}
  neuron_params[param_spec.name] = value;
}

}