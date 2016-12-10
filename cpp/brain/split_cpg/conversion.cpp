#include "conversion.h"
#include "../cppneat/neuron_gene.h"
#include "../cppneat/connection_gene.h"

#include <map>
#include <iostream>

namespace revolve {
namespace brain {

std::map<boost::shared_ptr<ExtNNConfig>, CPPNEAT::GeneticEncodingPtr> known;

CPPNEAT::Learner::LearningConfiguration learning_configuration; 
std::map<CPPNEAT::Neuron::Ntype, CPPNEAT::Neuron::NeuronTypeSpec> brain_spec;

void set_learning_conf()
{
	learning_configuration.asexual = false;
	learning_configuration.pop_size = 10;
	learning_configuration.tournament_size = 5;
	learning_configuration.num_children = 9;
	learning_configuration.weight_mutation_probability = 0.8;
	learning_configuration.weight_mutation_sigma = 1.0;
	learning_configuration.param_mutation_probability = 0.8;
	learning_configuration.param_mutation_sigma = 0.25;
	learning_configuration.structural_augmentation_probability = 0;
	learning_configuration.structural_removal_probability = 0;
	learning_configuration.max_generations = 100;
	learning_configuration.speciation_threshold = 0;
	learning_configuration.repeat_evaluations = 1;
}

void set_param_spec(CPPNEAT::Neuron::ParamSpec &spec, std::string name, double eps, double min_value, double max_value, bool min_inclusive, bool max_inclusive) {
	spec.name = name;
	spec.epsilon = eps;
	spec.max_inclusive = max_inclusive;
	spec.min_inclusive = min_inclusive;
	spec.min_value = min_value;
	spec.max_value = max_value;
}

void set_brain_spec()
{
	double eps = 1e-9;
	bool max_inclusive = false;
	bool min_inclusive = false;
	CPPNEAT::Neuron::NeuronTypeSpec input;
	input.possible_layers.push_back(CPPNEAT::Neuron::INPUT_LAYER);
	
	brain_spec[CPPNEAT::Neuron::INPUT] = input;
	
	
	CPPNEAT::Neuron::NeuronTypeSpec sigmoid;
	CPPNEAT::Neuron::ParamSpec bias_spec;
	set_param_spec(bias_spec, "rv:bias", eps, -1, 1, max_inclusive, min_inclusive);
	CPPNEAT::Neuron::ParamSpec gain_spec;
	set_param_spec(gain_spec, "rv:gain", eps, 0, 1, max_inclusive, min_inclusive);
	sigmoid.param_specs.push_back(bias_spec);
	sigmoid.param_specs.push_back(gain_spec);
	sigmoid.possible_layers.push_back(CPPNEAT::Neuron::HIDDEN_LAYER);
	sigmoid.possible_layers.push_back(CPPNEAT::Neuron::OUTPUT_LAYER);
	
	brain_spec[CPPNEAT::Neuron::SIGMOID] = sigmoid;

	
	CPPNEAT::Neuron::NeuronTypeSpec simple;
	simple.param_specs.push_back(bias_spec);
	simple.param_specs.push_back(gain_spec);
	simple.possible_layers.push_back(CPPNEAT::Neuron::HIDDEN_LAYER);
	simple.possible_layers.push_back(CPPNEAT::Neuron::OUTPUT_LAYER);
		
	brain_spec[CPPNEAT::Neuron::SIMPLE] = simple;


	CPPNEAT::Neuron::NeuronTypeSpec bias;
	bias.param_specs.push_back(bias_spec);
	bias.possible_layers.push_back(CPPNEAT::Neuron::HIDDEN_LAYER);
	bias.possible_layers.push_back(CPPNEAT::Neuron::OUTPUT_LAYER);
	
		
	brain_spec[CPPNEAT::Neuron::BIAS] = bias;


	CPPNEAT::Neuron::NeuronTypeSpec oscillator;
	CPPNEAT::Neuron::ParamSpec period_spec;
	set_param_spec(period_spec, "rv:period", eps, 0, 10, max_inclusive, min_inclusive);
	CPPNEAT::Neuron::ParamSpec phase_offset_spec;
	set_param_spec(phase_offset_spec, "rv:phase_offset", eps, 0, 3.14, max_inclusive, min_inclusive);
	CPPNEAT::Neuron::ParamSpec amplitude_spec;
	set_param_spec(amplitude_spec, "rv:amplitude", eps, 0, 10000, max_inclusive, min_inclusive);
	oscillator.param_specs.push_back(period_spec);
	oscillator.param_specs.push_back(phase_offset_spec);
	oscillator.param_specs.push_back(amplitude_spec);
	oscillator.possible_layers.push_back(CPPNEAT::Neuron::HIDDEN_LAYER);
	oscillator.possible_layers.push_back(CPPNEAT::Neuron::OUTPUT_LAYER);
	//ALERT:: not added to brain_spec



//         # these neurons are for the nonlinear oscillator CPG model found in Ijspeert (2005):
//         "V-Neuron": NeuronSpec(
//             params=[
//                 ParamSpec("alpha", min_value = 0.05, max_value = 10.0, epsilon = epsilon),
//                 ParamSpec("tau", min_value = 1.0, max_value = 50.0, epsilon = epsilon),
//                 ParamSpec("energy", min_value = 0.0, max_value = 25.0, epsilon = epsilon)
//             ],
//             layers = ["output", "hidden"]
//         ),
// 
//         "X-Neuron": NeuronSpec(
//             params=[
//                 ParamSpec("tau", min_value = 0.01, max_value = 5.0, epsilon = epsilon),
//             ],
//             layers = ["output", "hidden"]
//         ),

	CPPNEAT::Neuron::NeuronTypeSpec diff;
	diff.param_specs.push_back(bias_spec);
	diff.possible_layers.push_back(CPPNEAT::Neuron::HIDDEN_LAYER);
	diff.possible_layers.push_back(CPPNEAT::Neuron::OUTPUT_LAYER);
	
		
	brain_spec[CPPNEAT::Neuron::DIFFERENTIAL_CPG] = diff;

}




boost::shared_ptr<ExtNNConfig> convertForController(CPPNEAT::GeneticEncodingPtr genotype) {
	std::vector<CPPNEAT::NeuronGenePtr> neuron_genes = genotype->neuron_genes;
	std::vector<CPPNEAT::ConnectionGenePtr> connection_genes = genotype->connection_genes;
	
	std::map<int,NeuronPtr> innov_number_to_neuron;
	
	boost::shared_ptr<ExtNNConfig> config(new ExtNNConfig());
	config->numHiddenNeurons_ = 0;
	config->numInputNeurons_ = 0;
	config->numOutputNeurons_ = 0;
	for(CPPNEAT::NeuronGenePtr neuron_gene : neuron_genes) {
		NeuronPtr newNeuron;
		std::string neuronId = neuron_gene->neuron->neuron_id;
		std::map<std::string, double> neuron_params = neuron_gene->neuron->neuron_params;
		
		switch(neuron_gene->neuron->layer) {
			case CPPNEAT::Neuron::INPUT_LAYER: {
				newNeuron.reset(new InputNeuron(neuronId, neuron_params));
				config->inputNeurons_.push_back(newNeuron);
				config->inputPositionMap_[newNeuron] = config->numInputNeurons_;
				config->numInputNeurons_++;
				break;
			}
			case CPPNEAT::Neuron::HIDDEN_LAYER: {
				switch(neuron_gene->neuron->neuron_type) {
					case CPPNEAT::Neuron::SIMPLE: {
						newNeuron.reset(new LinearNeuron(neuronId, neuron_params));
						break;
					}
					case CPPNEAT::Neuron::SIGMOID: {
						newNeuron.reset(new SigmoidNeuron(neuronId, neuron_params));
						break;
					}
					case CPPNEAT::Neuron::DIFFERENTIAL_CPG: {
						newNeuron.reset(new DifferentialCPG(neuronId, neuron_params));
						break;
					}
					case CPPNEAT::Neuron::BIAS: {
						newNeuron.reset(new BiasNeuron(neuronId, neuron_params));
						break;
					}
					default: {
						throw std::runtime_error("Robot brain error");
						break;
					}
						
				}
				config->hiddenNeurons_.push_back(newNeuron);
				config->numHiddenNeurons_++;
				break;
			}
			case CPPNEAT::Neuron::OUTPUT_LAYER: {
				switch(neuron_gene->neuron->neuron_type) {
					case CPPNEAT::Neuron::SIMPLE: {
						newNeuron.reset(new LinearNeuron(neuronId, neuron_params));
						break;
					}
					case CPPNEAT::Neuron::SIGMOID: {
						newNeuron.reset(new SigmoidNeuron(neuronId, neuron_params));
						break;
					}
					case CPPNEAT::Neuron::DIFFERENTIAL_CPG: {
						newNeuron.reset(new DifferentialCPG(neuronId, neuron_params));
						break;
					}
					case CPPNEAT::Neuron::BIAS: {
						newNeuron.reset(new BiasNeuron(neuronId, neuron_params));
						break;
					}
					default: {
						throw std::runtime_error("Robot brain error");
						break;
					}
						
				}
				config->outputNeurons_.push_back(newNeuron);
				config->outputPositionMap_[newNeuron] = config->numOutputNeurons_;
				config->numOutputNeurons_++;
				break;
			}
			default: {
				throw std::runtime_error("Robot brain error");
				break;
			}
		}
		config->allNeurons_.push_back(newNeuron);
		config->idToNeuron_[neuronId] = newNeuron;
		innov_number_to_neuron[neuron_gene->getInnovNumber()] = newNeuron;
	}
	for(CPPNEAT::ConnectionGenePtr connection_gene : connection_genes) {
		NeuronPtr dst = innov_number_to_neuron[connection_gene->mark_to];
		NeuralConnectionPtr newConnection(new NeuralConnection(innov_number_to_neuron[connection_gene->mark_from],
								       dst,
								       connection_gene->weight));
		dst->AddIncomingConnection(dst->GetSocketId(), newConnection);
		config->connections_.push_back(newConnection);
	}
	known[config] = genotype;
	return config;
	
}
CPPNEAT::GeneticEncodingPtr convertForLearner(boost::shared_ptr<ExtNNConfig> config) {
	return known[config];
}

}	
}