#ifndef REVOLVE_GAZEBO_BRAIN_INPUTNEURON_H_
#define REVOLVE_GAZEBO_BRAIN_INPUTNEURON_H_

#include "neuron.h"

namespace revolve {
namespace brain {

class InputNeuron : public Neuron
{
public:
	/**
	* Constructor for an input neuron 
	* @param id: string to identify the neuron
	* @param params: parameters that specify the behavior of the neuron
	* @return pointer to the input neuron
	*/
	InputNeuron(const std::string &id,
		    const std::map<std::string, double> &params);

	/**
	* Method to set the input of the neuron
	* @param value: new input value
	*/
	virtual void SetInput(double value) override;
	
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
	double input_; //input of the neuron

};

}
}

#endif // REVOLVE_GAZEBO_BRAIN_INPUTNEURON_H_