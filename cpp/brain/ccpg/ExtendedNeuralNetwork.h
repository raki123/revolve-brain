#ifndef REVOLVE_GAZEBO_BRAIN_EXTENDEDNEURALNETWORK_H_
#define REVOLVE_GAZEBO_BRAIN_EXTENDEDNEURALNETWORK_H_

#include "brain/brain.h"
#include "brain/evaluator.h"
#include "Neuron.h"
#include "NeuralConnection.h"
#include <map>
#include <vector>
#include <string>


#include "LinearNeuron.h"
#include "SigmoidNeuron.h"
#include "OscillatorNeuron.h"
#include "VOscillator.h"
#include "XOscillator.h"
#include "LeakyIntegrator.h"
#include "BiasNeuron.h"
#include "DifferentialCPG.h"

#include "InputNeuron.h"


namespace revolve {
namespace brain {
  

class ExtendedNeuralNetwork : public Brain
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
	ExtendedNeuralNetwork(std::string modelName,
			      ExtNNConfig Config,
			      EvaluatorPtr evaluator,
			      const std::vector< ActuatorPtr > &actuators ,
			      const std::vector< SensorPtr > &sensors);

	virtual ~ExtendedNeuralNetwork();

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

	struct ExtNNConfig;
protected:

	// Mutex for updating the network
	//boost::mutex networkMutex_;


	/**
	* This function creates neurons and adds them to appropriate lists
	* @param neuronId: id of the neuron
	* @param neuronType: type of the neuron
	* @param neuronLayer: layer the neuron should be added to, can be 'hidden', 'input' or 'output'
	* @param params: parameters of the new neuron
	* @return pointer to the new neuron that was added
	*/
	NeuronPtr addNeuron(const std::string &neuronId,
			    const std::string &neuronType, 
			    const std::string &neuronLayer, 
			    const std::map<std::string, double> &params);
	
	/**
	* Method to add a new connection to the network
	* @param src: the beginning of the connetion
	* @param dst: the end of the connection
	* @param weight: weighting factor of the connection
	* @param idToNeuron: mapping between the neurons and their ids
	* @return pointer to the neural connection
	*/
	void connectionHelper(const std::string &src,
			      const std::string &dst,
			      const std::string &socket,
			      double weight,
			      const std::map<std::string, NeuronPtr> &idToNeuron);

	/**
	 * Gets the weight of all the connections
	 * @return weights of all neural connections
	 */
	std::vector<double> GetWeights();
	
	/**
	 * Changes the weights of the neural connections
	 * @param weights: new weights to be assigned
	 */
	void LoadWeights(std::vector<double> weights);
	
	/**
	 * Delete all hidden neurons and all connections
	 */
	void flush();



	std::string modelName_; //name of the robot
	
	EvaluatorPtr evaluator_ = NULL; //pointer to the evaluator that is used

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

	int numInputNeurons_; //number of input neurons
	int numOutputNeurons_; // number of output neurons
	int numHiddenNeurons_; // number of hidden neurons
public:
	struct ExtNNConfig {
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

		int numInputNeurons_; //number of input neurons
		int numOutputNeurons_; // number of output neurons
		int numHiddenNeurons_; // number of hidden neurons
	};

};


}
}

#endif // REVOLVE_GAZEBO_BRAIN_EXTENDEDNEURALNETWORK_H_