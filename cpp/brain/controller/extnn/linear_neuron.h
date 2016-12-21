#ifndef REVOLVE_GAZEBO_BRAIN_LINEARNEURON_H_
#define REVOLVE_GAZEBO_BRAIN_LINEARNEURON_H_

#include "neuron.h"
#include "neural_connection.h"

namespace revolve {
namespace brain {

class LinearNeuron : public Neuron
{
public:
	/**
	* Constructor for a linear neuron
	* @param id: string to identify the neuron
	* @param params: parameters that specify the behavior of the neuron
	* @return pointer to the linear neuron
	*/
	LinearNeuron(const std::string &id,
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
	double gain_; //gain of the neuron
	double bias_; //bias of the neuron
};

}
}

#endif // REVOLVE_GAZEBO_BRAIN_LINEARNEURON_H_