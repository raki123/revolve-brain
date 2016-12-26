#include "conversion.h"
#include "learner/cppneat/neuron_gene.h"
#include "learner/cppneat/connection_gene.h"

#include <map>
#include <iostream>
#include <fstream>
#include <vector>

namespace revolve {
namespace brain {

std::map<boost::shared_ptr<ExtNNConfig>, CPPNEAT::GeneticEncodingPtr> known;

CPPNEAT::Learner::LearningConfiguration learning_configuration; 
std::map<CPPNEAT::Neuron::Ntype, CPPNEAT::Neuron::NeuronTypeSpec> brain_spec;

std::map<int, unsigned int> input_map;
std::map<int, unsigned int> output_map;


void set_learning_conf()
{
	learning_configuration.asexual = false;
	learning_configuration.pop_size = 50;
	learning_configuration.tournament_size = 40;
	learning_configuration.num_children = 45;
	learning_configuration.weight_mutation_probability = 0.8;
	learning_configuration.weight_mutation_sigma = 5.0; //1.0 produced good results on ccpg without structural augmentation
	learning_configuration.param_mutation_probability = 0.8;
	learning_configuration.param_mutation_sigma = 0.25;
	learning_configuration.structural_augmentation_probability = 0.8;
	learning_configuration.structural_removal_probability = 0;
	learning_configuration.max_generations = 40;
	learning_configuration.speciation_threshold = 0.03;
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

void set_brain_spec(bool hyperneat)
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
	bias.possible_layers.push_back(CPPNEAT::Neuron::INPUT_LAYER);
	
		
	brain_spec[CPPNEAT::Neuron::BIAS] = bias;

	if(!hyperneat) {
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
		oscillator.possible_layers.push_back(CPPNEAT::Neuron::INPUT_LAYER);
		
		brain_spec[CPPNEAT::Neuron::OSCILLATOR] = oscillator;
	} else {
		CPPNEAT::Neuron::NeuronTypeSpec input_oscillator;
		CPPNEAT::Neuron::ParamSpec period_spec;
		set_param_spec(period_spec, "rv:period", eps, 0, 10, max_inclusive, min_inclusive);
		CPPNEAT::Neuron::ParamSpec phase_offset_spec;
		set_param_spec(phase_offset_spec, "rv:phase_offset", eps, 0, 3.14, max_inclusive, min_inclusive);
		CPPNEAT::Neuron::ParamSpec amplitude_spec;
		set_param_spec(amplitude_spec, "rv:amplitude", eps, 0, 10000, max_inclusive, min_inclusive);
		input_oscillator.param_specs.push_back(period_spec);
		input_oscillator.param_specs.push_back(phase_offset_spec);
		input_oscillator.param_specs.push_back(amplitude_spec);
		input_oscillator.possible_layers.push_back(CPPNEAT::Neuron::HIDDEN_LAYER);
		input_oscillator.possible_layers.push_back(CPPNEAT::Neuron::OUTPUT_LAYER);

		
		brain_spec[CPPNEAT::Neuron::INPUT_OSCILLATOR] = input_oscillator;
	}



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
	if(!hyperneat) {
		CPPNEAT::Neuron::NeuronTypeSpec diff;
		diff.param_specs.push_back(bias_spec);
		diff.possible_layers.push_back(CPPNEAT::Neuron::HIDDEN_LAYER);
		diff.possible_layers.push_back(CPPNEAT::Neuron::OUTPUT_LAYER);
		
			
		brain_spec[CPPNEAT::Neuron::DIFFERENTIAL_CPG] = diff;
	}
}




boost::shared_ptr<ExtNNConfig> convertForController(CPPNEAT::GeneticEncodingPtr genotype) {
	assert(genotype->layered == false);
	std::vector<CPPNEAT::NeuronGenePtr> neuron_genes = genotype->neuron_genes;
	std::vector<CPPNEAT::ConnectionGenePtr> connection_genes = genotype->connection_genes;
	
	std::map<int,NeuronPtr> innov_number_to_neuron;
	
	boost::shared_ptr<ExtNNConfig> config(new ExtNNConfig());
	for(CPPNEAT::NeuronGenePtr neuron_gene : neuron_genes) {
		NeuronPtr newNeuron;
		std::string neuronId = neuron_gene->neuron->neuron_id;
		std::map<std::string, double> neuron_params = neuron_gene->neuron->neuron_params;
		
		switch(neuron_gene->neuron->layer) {
			case CPPNEAT::Neuron::INPUT_LAYER: {
				newNeuron.reset(new InputNeuron(neuronId, neuron_params));
				config->inputNeurons_.push_back(newNeuron);
				config->inputPositionMap_[newNeuron] = input_map[neuron_gene->getInnovNumber()];
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
					case CPPNEAT::Neuron::OSCILLATOR: {
						newNeuron.reset(new OscillatorNeuron(neuronId, neuron_params));
						break;
					}
					case CPPNEAT::Neuron::INPUT_OSCILLATOR: {
						newNeuron.reset(new InputDependentOscillatorNeuron(neuronId, neuron_params));
						break;
					}
					default: {
						throw std::runtime_error("Unkown neuron type to be converted");
						break;
					}
						
				}
				config->hiddenNeurons_.push_back(newNeuron);
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
					case CPPNEAT::Neuron::OSCILLATOR: {
						newNeuron.reset(new OscillatorNeuron(neuronId, neuron_params));
						break;
					}
					case CPPNEAT::Neuron::INPUT_OSCILLATOR: {
						newNeuron.reset(new InputDependentOscillatorNeuron(neuronId, neuron_params));
						break;
					}
					default: {
						throw std::runtime_error("Unknown neuron type to be converted");
						break;
					}
						
				}
				config->outputNeurons_.push_back(newNeuron);
				config->outputPositionMap_[newNeuron] = output_map[neuron_gene->getInnovNumber()];
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
CPPNEAT::GeneticEncodingPtr convertForLearner(boost::shared_ptr<ExtNNConfig> config) 
{
	return known[config];
}
std::vector<double> forController(std::vector<std::vector<double>> toConvert)
{
	return toConvert[0];
}

std::vector<std::vector<double>> forLearner(std::vector<double> toConvert) 
{
	return std::vector<std::vector<double>>(1,toConvert);
}



boost::shared_ptr<ExtNNConfig> cpg_network;
std::map<std::string, std::tuple<int,int,int>> neuron_coordinates;
CPPNEAT::GeneticEncodingPtr last;

std::string getHyper() 
{
	learning_configuration.layered_network = true;
	return "https://www.youtube.com/watch?v=NxuJ3RosR80";
}

boost::shared_ptr< LayeredExtNNConfig > convertForLayeredExtNN(CPPNEAT::GeneticEncodingPtr genotype)
{
	assert(genotype->layered == true);
	std::vector<std::vector<CPPNEAT::NeuronGenePtr>> layers = genotype->layers;
	std::vector<CPPNEAT::ConnectionGenePtr> connection_genes = genotype->connection_genes;
	
	std::map<int,NeuronPtr> innov_number_to_neuron;
	
	boost::shared_ptr<LayeredExtNNConfig> config(new LayeredExtNNConfig());
	config->layers_ = std::vector<std::vector<NeuronPtr>>(layers.size(), std::vector<NeuronPtr>());
	for(unsigned int i = 0; i < layers.size(); i++) {
		for(CPPNEAT::NeuronGenePtr neuron_gene : layers[i]) {
			NeuronPtr newNeuron;
			std::string neuronId = neuron_gene->neuron->neuron_id;
			std::map<std::string, double> neuron_params = neuron_gene->neuron->neuron_params;
			
			switch(neuron_gene->neuron->layer) {
				case CPPNEAT::Neuron::INPUT_LAYER: {
					newNeuron.reset(new InputNeuron(neuronId, neuron_params));
					config->layers_[i].push_back(newNeuron);
					config->inputPositionMap_[newNeuron] = input_map[neuron_gene->getInnovNumber()];
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
						case CPPNEAT::Neuron::OSCILLATOR: {
							newNeuron.reset(new OscillatorNeuron(neuronId, neuron_params));
							break;
						}
						case CPPNEAT::Neuron::INPUT_OSCILLATOR: {
							newNeuron.reset(new InputDependentOscillatorNeuron(neuronId, neuron_params));
							break;
						}
						default: {
							throw std::runtime_error("Unkown neuron type to be converted");
							break;
						}
							
					}
					config->layers_[i].push_back(newNeuron);
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
						case CPPNEAT::Neuron::OSCILLATOR: {
							newNeuron.reset(new OscillatorNeuron(neuronId, neuron_params));
							break;
						}
						case CPPNEAT::Neuron::INPUT_OSCILLATOR: {
							newNeuron.reset(new InputDependentOscillatorNeuron(neuronId, neuron_params));
							break;
						}
						default: {
							throw std::runtime_error("Unknown neuron type to be converted");
							break;
						}
							
					}
					config->layers_[i].push_back(newNeuron);
					config->outputPositionMap_[newNeuron] = output_map[neuron_gene->getInnovNumber()];
					break;
				}
				default: {
					throw std::runtime_error("Robot brain error");
					break;
				}
			}
			config->idToNeuron_[neuronId] = newNeuron;
			innov_number_to_neuron[neuron_gene->getInnovNumber()] = newNeuron;
		}
	}
	for(CPPNEAT::ConnectionGenePtr connection_gene : connection_genes) {
		NeuronPtr dst = innov_number_to_neuron[connection_gene->mark_to];
		NeuralConnectionPtr newConnection(new NeuralConnection(innov_number_to_neuron[connection_gene->mark_from],
								       dst,
								       connection_gene->weight));
		dst->AddIncomingConnection(dst->GetSocketId(), newConnection);
		config->connections_.push_back(newConnection);
	}
	return config;
}

boost::shared_ptr<ExtNNConfig> convertForExtNNFromHyper(CPPNEAT::GeneticEncodingPtr genotype)
{
	boost::shared_ptr<LayeredExtNNConfig> hyper_config = convertForLayeredExtNN(genotype);
	LayeredExtNNController print_net("debug", hyper_config, std::vector<ActuatorPtr>(), std::vector<SensorPtr>());
	std::ofstream debug_out("debug.dot");
	print_net.writeNetwork(debug_out);
	for(NeuralConnectionPtr connection : cpg_network->connections_) 
	{		
		NeuronPtr src = connection->GetInputNeuron();
		NeuronPtr dst = connection->GetOutputNeuron();
		std::tuple<int,int,int> coord_src = neuron_coordinates[src->Id()];
		std::tuple<int,int,int> coord_dst = neuron_coordinates[dst->Id()];
		for(NeuronPtr neuron : hyper_config->layers_[0]) 
		{
			//could be faster by neuron->Id()[6] but less easy to read
			if(neuron->Id() == "Input-0") { 
				neuron->SetInput(std::get<0>(coord_src));
			} else if(neuron->Id() == "Input-1") { 
				neuron->SetInput(std::get<1>(coord_src));
			} else if(neuron->Id() == "Input-2") { 
				neuron->SetInput(std::get<2>(coord_src));
			} else if(neuron->Id() == "Input-3") { 
				neuron->SetInput(std::get<0>(coord_dst));
			} else if(neuron->Id() == "Input-4") { 
				neuron->SetInput(std::get<1>(coord_dst));
			} else if(neuron->Id() == "Input-5") { 
				neuron->SetInput(std::get<2>(coord_dst));
			} 
		}

		for(std::vector<NeuronPtr> layer : hyper_config->layers_) {
			for(NeuronPtr neuron : layer) {
				neuron->Update(0);
			}
			for(NeuronPtr neuron : layer) {
				neuron->FlipState();
			}
		}

		std::map<std::string, double> params;
		for (auto it = hyper_config->layers_[hyper_config->layers_.size()-1].begin(); it != hyper_config->layers_[hyper_config->layers_.size()-1].end(); ++it) 
		{
			auto outNeuron = *it;
			if(outNeuron->Id() == "weight") 
			{
				connection->SetWeight(outNeuron->GetOutput());
				break;
			}
		}
	}
	for(NeuronPtr neuron : cpg_network->allNeurons_) 
	{		
		std::tuple<int,int,int> coord_src = neuron_coordinates[neuron->Id()];
		std::tuple<int,int,int> coord_dst = std::make_tuple(0,0,0);
		for(NeuronPtr neuron : hyper_config->layers_[0]) 
		{
			//could be faster by neuron->Id()[6] but less easy to read
			if(neuron->Id() == "Input-0") { 
				neuron->SetInput(std::get<0>(coord_src));
			} else if(neuron->Id() == "Input-1") { 
				neuron->SetInput(std::get<1>(coord_src));
			} else if(neuron->Id() == "Input-2") { 
				neuron->SetInput(std::get<2>(coord_src));
			} else if(neuron->Id() == "Input-3") { 
				neuron->SetInput(std::get<0>(coord_dst));
			} else if(neuron->Id() == "Input-4") { 
				neuron->SetInput(std::get<1>(coord_dst));
			} else if(neuron->Id() == "Input-5") { 
				neuron->SetInput(std::get<2>(coord_dst));
			} 
		}

		for(std::vector<NeuronPtr> layer : hyper_config->layers_) {
			for(NeuronPtr neuron : layer) {
				neuron->Update(0);
			}
			for(NeuronPtr neuron : layer) {
				neuron->FlipState();
			}
		}

		std::map<std::string, double> params;
		for (auto it = hyper_config->layers_[hyper_config->layers_.size()-1].begin(); it != hyper_config->layers_[hyper_config->layers_.size()-1].end(); ++it) 
		{
			auto outNeuron = *it;
			params[outNeuron->Id()] = outNeuron->GetOutput();
		}
		neuron->setNeuronParameters(params);
	}
	last = genotype;
	return cpg_network;
}

CPPNEAT::GeneticEncodingPtr convertForHyperFromExtNN(boost::shared_ptr<ExtNNConfig> config)
{
	return last;
}

}	
}