#ifndef REVOLVE_GAZEBO_BRAIN_SIGMOIDNEURON_H_
#define REVOLVE_GAZEBO_BRAIN_SIGMOIDNEURON_H_

#include "Neuron.h"
#include "NeuralConnection.h"

namespace revolve {
namespace brain {

class SigmoidNeuron : public Neuron
{
public:
	/**
	* Constructor for a sigmoid neuron
	* @param id: string to identify the neuron
	* @param params: parameters that specify the behavior of the neuron
	* @return pointer to the sigmoid neuron
	*/
	SigmoidNeuron(const std::string &id,
		      const std::map<std::string, double> &params);
	
	/**
	* Method to calculate the output of the neuron
	* @param t: current time
	* @return the output of the neuron at time t
	*/
	virtual double CalculateOutput(double t);


protected:
	double gain_; //gain of the neuron
	double bias_; //bias of the neuron
};

}
}

#endif // REVOLVE_GAZEBO_BRAIN_SIGMOIDNEURON_H_