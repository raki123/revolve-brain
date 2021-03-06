#ifndef REVOLVE_GAZEBO_BRAIN_VOSCILLATOR_H_
#define REVOLVE_GAZEBO_BRAIN_VOSCILLATOR_H_

#include "neuron.h"
#include "neural_connection.h"


namespace revolve {
namespace brain {

class VOscillator : public Neuron
{
public:
	/**
	* Constructor for a Voscilator neuron
	* @param id: string to identify the neuron
	* @param params: parameters that specify the behavior of the neuron
	* @return pointer to the Voscilator neuron
	*/	VOscillator(const std::string &id,
		    const std::map<std::string, double> &params);
	
	/**
	* Method to calculate the output of the neuron
	* @param t: current time
	* @return the output of the neuron at time t
	*/
	virtual double CalculateOutput(double t);


protected:
	double alpha_; //TODO:: find out what these parameters mean
	double tau_;
	double energy_;

	double lastTime_; //last time the output was calculated
	double stateDeriv_;
};

}
}

#endif // REVOLVE_GAZEBO_BRAIN_VOSCILLATOR_H_