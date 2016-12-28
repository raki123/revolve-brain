#ifndef REVOLVE_GAZEBO_BRAIN_INPUTDEPENDENTOSCILLATORNEURON_H_
#define REVOLVE_GAZEBO_BRAIN_INPUTDEPENDENTOSCILLATORNEURON_H_

#include "neuron.h"
#include "neural_connection.h"

namespace revolve {
namespace brain {

class InputDependentOscillatorNeuron : public Neuron
{
public:
	/**
	* Constructor for an oscilator neuron
	* @param id: string to identify the neuron
	* @param params: parameters that specify the behavior of the neuron
	* @return pointer to the oscilator neuron
	*/
	InputDependentOscillatorNeuron(const std::string &id,
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
	double period_; //period of the oscillation of the neuron
	double phaseOffset_; //current phase offset of the oscillation of the neuron
	double gain_; //gain of the neuron
};

}
}

#endif // REVOLVE_GAZEBO_BRAIN_INPUTDEPENDENTOSCILLATORNEURON_H_