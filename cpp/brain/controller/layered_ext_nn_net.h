#ifndef REVOLVE_BRAIN_LAYERED_NEAT_EXTNNCONTROLLER_H_
#define REVOLVE_BRAIN_LAYERED_NEAT_EXTNNCONTROLLER_H_

#include "controller.h"
#include "../evaluator.h"
#include "extnn/neuron.h"
#include "extnn/neural_connection.h"
#include <map>
#include <vector>
#include <string>


#include "extnn/linear_neuron.h"
#include "extnn/sigmoid_neuron.h"
#include "extnn/oscillator_neuron.h"
#include "extnn/v_oscillator.h"
#include "extnn/x_oscillator.h"
#include "extnn/leaky_integrator.h"
#include "extnn/bias_neuron.h"
#include "extnn/differential_cpg.h"
#include "extnn/input_neuron.h"
#include "extnn/input_dependent_oscillator_neuron.h"


namespace revolve {
namespace brain {
  
struct LayeredExtNNConfig {

	std::vector<std::vector<NeuronPtr>> layers_; //vector containing all neurons

	std::map<NeuronPtr, int> outputPositionMap_; 	// positions for indexing into the outputs_ buffer for each output neuron
	std::map<NeuronPtr, int> inputPositionMap_;	// positions for indexing into the inputs_ buffer for each input neuron

	std::map<std::string, NeuronPtr> idToNeuron_;	// Map neuron id strings to Neuron objects

	std::vector<NeuralConnectionPtr> connections_; //vector of all the neural connections
};

//extended neural network controller usable with standard neat or hyper neat (use different conversion methods)
class LayeredExtNNController : public Controller<boost::shared_ptr<LayeredExtNNConfig>>
{
public:
	/**
	 * Constructor for a neural network including neurons that are of a different type than the usual ones.
	 * @param modelName: name of the model
	 * @param Config: configuration file
	 * @param evaluator: pointer to the evaluator that is used
	 * @param actuators: vector list of robot's actuators
	 * @param sensors: vector list of robot's sensors
	 * @return pointer to the neural network
	 */
	LayeredExtNNController(std::string modelName,
			      boost::shared_ptr<LayeredExtNNConfig> Config,
			      const std::vector< ActuatorPtr > &actuators ,
			      const std::vector< SensorPtr > &sensors);

	virtual ~LayeredExtNNController();

	/**
	* Method for updating sensors readings, actuators positions
	* @param actuators: vector list of robot's actuators
	* @param sensors: vector list of robot's sensors
	* @param t: current time
	* @param step:
	*/
	virtual void update(const std::vector< ActuatorPtr > &actuators,
			    const std::vector< SensorPtr > &sensors,
			    double t,
			    double step);
			    
	/**
	 * Gets the weight of all the connections
	 * @return weights of all neural connections
	 */
	virtual boost::shared_ptr<LayeredExtNNConfig> getGenome();
	
	/**
	 * Changes the weights of the neural connections
	 * @param weights: new weights to be assigned
	 */
	virtual void setGenome(boost::shared_ptr<LayeredExtNNConfig> config);
	
	void writeNetwork(std::ofstream &write_to);

protected:



	std::string modelName_; //name of the robot
	

	double * inputs_;    // buffer of input values from the sensors
	double * outputs_;     // buffer of output values for the actuators

	std::vector<std::vector<NeuronPtr>> layers_; //vector containing all neurons


	std::map<NeuronPtr, int> outputPositionMap_; 	// positions for indexing into the outputs_ buffer for each output neuron
	std::map<NeuronPtr, int> inputPositionMap_;	// positions for indexing into the inputs_ buffer for each input neuron

	std::map<std::string, NeuronPtr> idToNeuron_;	// Map neuron id strings to Neuron objects

	std::vector<NeuralConnectionPtr> connections_; //vector of all the neural connections




};


}
}

#endif // REVOLVE_BRAIN_LAYERED_NEAT_EXTNNCONTROLLER_H_