#ifndef REVOLVE_GAZEBO_BRAIN_EXTENDEDNEURALNETWORK_H_
#define REVOLVE_GAZEBO_BRAIN_EXTENDEDNEURALNETWORK_H_

#include "split_brain.h"
#include "../evaluator.h"
#include "neuron.h"
#include "neural_connection.h"
#include <map>
#include <vector>
#include <string>


#include "linear_neuron.h"
#include "sigmoid_neuron.h"
#include "oscillator_neuron.h"
#include "v_oscillator.h"
#include "x_oscillator.h"
#include "leaky_integrator.h"
#include "bias_neuron.h"
#include "differential_cpg.h"
#include "input_neuron.h"


namespace revolve {
namespace brain {
  

class ExtNNController : public Controller<std::vector<double>>
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
	ExtNNController(std::string modelName);

	virtual ~ExtNNController();

	/**
	* Method for updating sensors readings, actuators positions
	* @param actuators: vector list of robot's actuators
	* @param sensors: vector list of robot's sensors
	* @param t: current time
	* @param step:
	*/
// 	virtual void update(const std::vector< ActuatorPtr > &actuators,
// 			    const std::vector< SensorPtr > &sensors,
// 			    double t,
// 			    double step) = 0;

protected:


};


}
}

#endif // REVOLVE_GAZEBO_BRAIN_EXTENDEDNEURALNETWORK_H_