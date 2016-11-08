//
// Created by matteo on 01/11/16.
//

#ifndef REVOLVE_BRAIN_RYTHMGENERATIONNEURON_H
#define REVOLVE_BRAIN_RYTHMGENERATIONNEURON_H

#include "CPGNeuron.h"

namespace revolve {
namespace brain {
namespace cpg {

class RythmGenerationNeuron : public revolve::brain::cpg::Neuron {
public:
    /**
     * RythmGenerationNeuron constructor. Starting value of phi is 1
     * @param weight the weight that determines the shape [0, 4.5]
     * @param c the phase difference [-2, 2]
     * @param amplitude the amplitude determines influence of a flexor/extensor on the final output signal [−10, 10]
     * @param offset the offset dims shape of the final output signal to the starting position [-0.1, 0.1]
     * @throws invalid_parameter if one of the given parameters is out of range
     */
    RythmGenerationNeuron(real_t weight, real_t c, real_t amplitude, real_t offset);

    virtual ~RythmGenerationNeuron();

    /**
     * Update the rythm generator to the step (t+1)
     * @param inputs std::vector<real> of size 1 contaning the phi of the coupled
     * RythmGenerationNeuron
     * @param delta_time time passed since the last update. Value in seconds expected to be
     * positive and relativly small. Big time steps could have weird effects on the network.
     * @return std::vector<real> of size 2. First element has the output of the
     * @throws invalid_input_exception if input vector is not of the correct size (1)
     * RythmGenerationNeuron, second element has the update phi value
     */
    virtual std::vector<real_t> update(std::vector<real_t> inputs, real_t delta_time) override;

protected:
    /**
     * Calculates the next phi value and returns it. It is NOT updating this->phi.
     * @param otherPhi the phi of the coupled RythmGenerationNeuron
     * @param delta_time time passed since the last update. Value in seconds expected to be
     * positive and relativly small. Big time steps could have weird effects on the network.
     * @return new phi value
     */
    real_t nextPhi(const real_t otherPhi, real_t delta_time) const;

    /**
     * Calculates the output value of the neuron using the current phi value
     * @return the output value of the RythmGenerationNeuron
     */
    real_t output() const;

// GETTERS AND SETTERS
public:
    /**
     * Returns current phi value
     * @return current phi value
     */
    real_t getPhi() const;

    real_t getWeight() const;
    void setWeight(real_t weight);
    // value from 0 to 1, does not throw exceptions if value is outside the allowed domain
    real_t setWeightPercentage(real_t weight);
    real_t calculateWeightPercentage(real_t weight) const;
    real_t calculateWeightFromPercentage(real_t weight) const;

    real_t getC() const;
    void setC(real_t c);
    // value from 0 to 1, does not throw exceptions if value is outside the allowed domain
    real_t setCPercentage(real_t c);
    real_t calculateCPercentage(real_t c) const;
    real_t calculateCFromPercentage(real_t c) const;

    real_t getAmplitude() const;
    void setAmplitude(real_t amplitude);
    // value from 0 to 1, does not throw exceptions if value is outside the allowed domain
    real_t setAmplitudePercentage(real_t amplitude);
    real_t calculateAmplitudePercentage(real_t amplitude) const;
    real_t calculateAmplitudeFromPercentage(real_t amplitude) const;

    real_t getOffset() const;
    void setOffset(real_t offset);
    // value from 0 to 1, does not throw exceptions if value is outside the allowed domain
    real_t setOffsetPercentage(real_t offset);
    real_t calculateOffsetPercentage(real_t offset) const;
    real_t calculateOffsetFromPercentage(real_t offset) const;

public:
    const real_t WEIGHT_MIN = 0;
    const real_t WEIGHT_MAX = 4.5;
    const real_t C_MIN = -2;
    const real_t C_MAX =  2;
    const real_t AMPLITUDE_MIN = -10;
    const real_t AMPLITUDE_MAX =  10;
    const real_t OFFSET_MIN = -0.1;
    const real_t OFFSET_MAX =  0.1;

protected:
    // current value
    real_t phi;

private:
    // RythmGenerationNeuron parameters
    // the weight that determines the shape [0, 4.5]
    real_t weight;
    // the phase difference [-2, 2]
    real_t c;
    //  the amplitude determines influence of a flexor/extensor on the final output signal [−10, 10]
    real_t amplitude;
    //  the offset dims shape of the final output signal to the starting position [-0.1, 0.1]
    real_t offset;
};

}
}
}


#endif //REVOLVE_BRAIN_RYTHMGENERATIONNEURON_H
