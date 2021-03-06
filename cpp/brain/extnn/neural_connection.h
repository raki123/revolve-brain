#ifndef REVOLVE_GAZEBO_BRAIN_NEURALCONNECTION_H_
#define REVOLVE_GAZEBO_BRAIN_NEURALCONNECTION_H_

#include "neuron.h"

namespace revolve {
namespace brain {

class NeuralConnection
{
public:
	/**
	 * Consturctor for a neural connetion
	 * @param src: the beginning of the connetion
	 * @param dst: the end of the connection
	 * @param weight: weighting factor of the connection
	 * @return pointer to the neural connection
	 */
	NeuralConnection(const NeuronPtr &src,
			 const NeuronPtr &dst,
			 double weight);
	
	/**
	 * @return the weight of the neural connection
	 */
	double GetWeight() const;
	
	/**
	 * @param weight: new weight of the neural connection
	 */
	void SetWeight(double weigth);
	
	/**
	 * @return the neuron in the beginning of the connection
	 */
	NeuronPtr GetInputNeuron() const;

protected:
	double weight_; //weight of the connection
	NeuronPtr src_; //source neuron of the connection
	NeuronPtr dst_; //destination neuron of the connection
};


}
}

#endif // REVOLVE_GAZEBO_BRAIN_NEURALCONNECTION_H_
