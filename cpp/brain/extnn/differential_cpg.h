#ifndef REVOLVE_GAZEBO_BRAIN_DIFFERENTIALCPG_H_
#define REVOLVE_GAZEBO_BRAIN_DIFFERENTIALCPG_H_

#include "neuron.h"
#include "neural_connection.h"

namespace revolve {
namespace brain {

class DifferentialCPG : public Neuron
{
public:
	/**
	* Constructor for differential cpg 
	* @param id: string to identify the neuron
	* @param params: parameters that specify the behavior of the neuron
	* @return pointer to the differential cpg
	*/
	DifferentialCPG(const std::string &id,
			const std::map<std::string, double> &params);
	
	/**
	* Method to calculate the output of the neuron
	* @param t: current time
	* @return the output of the neuron at time t
// 	*/
	virtual double CalculateOutput(double t);
	
	virtual std::map<std::string, double> getNeuronParameters();
	
	virtual std::string getType();


protected:
	double bias_; //bias of the neuron
	double lastTime_; //last time the output was calculated
};

}
}

#endif // REVOLVE_GAZEBO_BRAIN_DIFFERENTIALCPG_H_