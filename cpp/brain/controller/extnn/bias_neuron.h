#ifndef REVOLVE_GAZEBO_BRAIN_BIASNEURON_H_
#define REVOLVE_GAZEBO_BRAIN_BIASNEURON_H_

#include "neuron.h"
#include "neural_connection.h"

namespace revolve {
namespace brain {

class BiasNeuron : public Neuron
{
public:
	/**
	* Constructor for a bias neuron
	* @param id: string to identify the neuron
	* @param params: parameters that specify the behavior of the neuron
	* @return pointer to the bias neuron
	*/
	BiasNeuron(const std::string &id,
		   const std::map<std::string, double> &params);
	/**
	* Method to calculate the output of the neuron
	* @param t: current time
	* @return the output of the neuron at time t
	*/
	virtual double CalculateOutput(double t);
	
	virtual std::map<std::string, double> getNeuronParameters();
	
	virtual void setNeuronParameters(std::map<std::string, double> params);
	
	virtual std::string getType();


protected:
	double bias_; //bias of the neuron
};

}
}

#endif // REVOLVE_GAZEBO_BRAIN_BIASNEURON_H_