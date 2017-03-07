//
// Created by matteo on 09/11/16.
//

#ifndef REVOLVE_BRAIN_MOTONEURON_H
#define REVOLVE_BRAIN_MOTONEURON_H

#include "CPGNeuron.h"

namespace revolve {
namespace brain {
namespace cpg {

class MotoNeuron : revolve::brain::cpg::Neuron {
public:
    /**
     * MotoNeuron constructor
     * @param v_max maximum potential that can be produced as an output. Default to 1.
     */
    MotoNeuron(real_t v_max = 1);

    virtual ~MotoNeuron();

/**
 * Update the Moto Neuron to the step (t+1)
 * @param inputs std::vector<real> of size 2 contaning first the PFE output and second
 * the PFF output.
 * @param delta_time time passed since the last update. Value in seconds expected to be
 * positive and relativly small. Big time steps could have weird effects on the network.
 * @return std::vector<real> of size 1.
 * @throws invalid_input_exception if input vector is not of the correct size (dimension of internal weights)
 */
    virtual std::vector<real_t> update(std::vector<real_t> inputs, real_t delta_time) override;

private:
    /**
     * Calculates the output value of the neuron
     * @return neuron output
     */
    real_t output(real_t pfe, real_t pff) const;

private:
    real_t v_max;
};

}
}
}

#endif //REVOLVE_BRAIN_MOTONEURON_H
