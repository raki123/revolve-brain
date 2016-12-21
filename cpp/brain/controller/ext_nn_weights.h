#ifndef REVOLVE_BRAIN_EXTNNCONTROLLER_H_
#define REVOLVE_BRAIN_EXTNNCONTROLLER_H_

#include "controller.h"
#include "../evaluator.h"
#include "extnn/neuron.h"
#include "extnn/neural_connection.h"
#include "ext_nn_net.h"
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


namespace revolve {
namespace brain {
  

class ExtNNController : public Controller<std::vector<double>>
{
public:
  
	/**
	 * Constructor for a neural network including neurons that are of a different type than the usual ones.
	 * @param modelName: name of the model
	 * @param Config: configuration file
	 * @param actuators: vector list of robot's actuators
	 * @param sensors: vector list of robot's sensors
	 * @return pointer to the neural network
	 */
	ExtNNController(std::string modelName,
			      boost::shared_ptr<ExtNNConfig> Config,
			      const std::vector< ActuatorPtr > &actuators ,
			      const std::vector< SensorPtr > &sensors);

	virtual ~ExtNNController();

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
	 * Gets the weight of all the connections and all parameters of all neurons
	 * @return weights of all neural connections and parameters for all neurons
	 */
	virtual std::vector<double> getGenome();
	
	/**
	 * Changes the weights of the neural connections
	 * @param weights: new weights to be assigned
	 */
	virtual void setGenome(std::vector<double> weights);
	
	void writeNetwork(std::ofstream &write_to);
	
protected:

	std::string modelName_; //name of the robot
	
	double * inputs_;    // buffer of input values from the sensors
	double * outputs_;     // buffer of output values for the actuators

	std::vector<NeuronPtr> allNeurons_; //vector containing all neurons
	std::vector<NeuronPtr> inputNeurons_; //vector containing the input neurons
	std::vector<NeuronPtr> outputNeurons_; //vector containing the output neurons
	std::vector<NeuronPtr> hiddenNeurons_; //vector containing the hidden neurons

	std::map<NeuronPtr, int> outputPositionMap_; 	// positions for indexing into the outputs_ buffer for each output neuron
	std::map<NeuronPtr, int> inputPositionMap_;	// positions for indexing into the inputs_ buffer for each input neuron

	std::map<std::string, NeuronPtr> idToNeuron_;	// Map neuron id strings to Neuron objects

	std::vector<NeuralConnectionPtr> connections_; //vector of all the neural connections

public:



};


}
}

#endif // REVOLVE_BRAIN_EXTNNCONTROLLER_H_