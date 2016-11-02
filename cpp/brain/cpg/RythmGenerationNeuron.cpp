//
// Created by matteo on 01/11/16.
//

#include "RythmGenerationNeuron.h"
#include <sstream>
#include <cmath>

using namespace revolve::brain::cpg;



RythmGenerationNeuron::RythmGenerationNeuron(real_t weight, real_t c, real_t amplitude, real_t offset)
        : phi(1)
{
    setWeight(weight);
    setC(c);
    setAmplitude(amplitude);
    setOffset(offset);
}

RythmGenerationNeuron::~RythmGenerationNeuron()
{

}

std::vector<real_t> RythmGenerationNeuron::update(std::vector<real_t> inputs, real_t delta_time) {
    // reading neuron inputs
    if (inputs.size() != 1) {
        std::stringstream ss;
        ss << "input size should be 1, instead is " << inputs.size();
        throw invalid_input_exception(ss.str());
    }
    real_t otherPhi = inputs[0];

    // creating output value from previous phi
    real_t _output = output();

    // updating phi to the new value
    // NOTE we update phi after getting the output value because this way both
    // phi and other phi are of the same cycle.
    phi = nextPhi(otherPhi, delta_time);

    return {_output, phi};
}

real_t RythmGenerationNeuron::nextPhi(const real_t otherPhi, real_t delta_time) const {
    static const real_t PI = std::acos(-1); // define PI

    const real_t thisPhi = this->phi;

    // (2 * pi * c') + w * sin(otherPhi-thisPhi)
    const real_t deltaPhi =
            2 * PI * this->c +
            this->weight * std::sin(otherPhi-thisPhi);

    real_t new_phi = thisPhi + deltaPhi;

    //TODO compensate for phi error
//    if (new_phi > PI*2)
//        new_phi -= PI*2;
//    else if (new_phi < -PI*2)
//        new_phi += PI*2;

    //TODO consider delta_time
    return new_phi;
}

real_t RythmGenerationNeuron::output() const {
    // A * cos(phi) + o
    return (this->amplitude * std::cos(this->phi))
           + this->offset;
}

// GETTERS AND SETTERS --------------------------------------------------------

real_t RythmGenerationNeuron::getPhi() const {
    return phi;
}

real_t RythmGenerationNeuron::getWeight() const {
    return weight;
}

#define WEIGHT_MIN 0
#define WEIGHT_MAX 4.5
// the weight that determines the shape [0, 4.5]
void RythmGenerationNeuron::setWeight(real_t weight) {
    if (weight < WEIGHT_MIN || weight > WEIGHT_MAX)
        throw invalid_parameter("weight", weight, WEIGHT_MIN, WEIGHT_MAX);
    RythmGenerationNeuron::weight = weight;
}

real_t RythmGenerationNeuron::getC() const {
    return c;
}

#define C_MIN -2
#define C_MAX 2
// the phase difference [-2, 2]
void RythmGenerationNeuron::setC(real_t c) {
    if (c < C_MIN || c > C_MAX)
        throw invalid_parameter("c", c, C_MIN, C_MAX);
    RythmGenerationNeuron::c = c;
}

real_t RythmGenerationNeuron::getAmplitude() const {
    return amplitude;
}

#define AMPLITUDE_MIN -10
#define AMPLITUDE_MAX 10
//  the amplitude determines influence of a flexor/extensor on the final output signal [−10, 10]
void RythmGenerationNeuron::setAmplitude(real_t amplitude) {
    if (amplitude < AMPLITUDE_MIN || amplitude > AMPLITUDE_MAX)
        throw invalid_parameter("amplitude", amplitude, AMPLITUDE_MIN, AMPLITUDE_MAX);
    RythmGenerationNeuron::amplitude = amplitude;
}

real_t RythmGenerationNeuron::getOffset() const {
    return offset;
}

#define OFFSET_MIN -0.1
#define OFFSET_MAX 0.1
//  the offset dims shape of the final output signal to the starting position [-0.1, 0.1]
void RythmGenerationNeuron::setOffset(real_t offset) {
    if (offset < OFFSET_MIN || offset > OFFSET_MAX)
        throw invalid_parameter("offset", offset, OFFSET_MIN, OFFSET_MAX);
    RythmGenerationNeuron::offset = offset;
}

real_t percentage_of_range(real_t percentage, real_t range_start, real_t range_end)
{
    if (percentage > 100)
        return range_end;
    else if (percentage < 0)
        return range_start;

    real_t range = range_end - range_start;
    real_t distance = (percentage/100) * range;
    return range_start + distance;
}

real_t RythmGenerationNeuron::setWeightPercentage(real_t weight) {
    this->weight = percentage_of_range(weight, WEIGHT_MIN, WEIGHT_MAX);
    return this->weight;
}

real_t RythmGenerationNeuron::setCPercentage(real_t c) {
    this->c = percentage_of_range(c, C_MIN, C_MAX);
    return this->c;
}

real_t RythmGenerationNeuron::setAmplitudePercentage(real_t amplitude) {
    this->amplitude = percentage_of_range(amplitude, AMPLITUDE_MIN, AMPLITUDE_MAX);
    return this->amplitude;
}

real_t RythmGenerationNeuron::setOffsetPercentage(real_t offset) {
    this->offset = percentage_of_range(offset, OFFSET_MIN, OFFSET_MAX);
    return this->offset;
}

