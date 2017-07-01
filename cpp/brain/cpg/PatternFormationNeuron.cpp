//
// Created by matteo on 08/11/16.
//

#include "PatternFormationNeuron.h"
#include <cmath>

using namespace revolve::brain::cpg;

PatternFormationNeuron::PatternFormationNeuron(real_t alpha,
                                               real_t theta,
                                               std::vector<real_t> weights)
{
    setAlpha(alpha);
    setTheta(theta);
    setWeights(weights);
}

PatternFormationNeuron::~PatternFormationNeuron()
{}

std::vector<real_t> PatternFormationNeuron::update(std::vector<real_t> inputs,
                                                   real_t delta_time)
{
    real_t combined_inputs = generateInput(inputs);
    real_t result = output(combined_inputs);

    return {result};
}

real_t PatternFormationNeuron::generateInput(std::vector<real_t> inputs) const {
    if (inputs.size() != weights.size()) {
        std::stringstream ss;
        ss << "input size should be the same as weight("
           << weights.size()
           << "), instead is "
           << inputs.size();
        throw invalid_input_exception(ss.str());
    }

    real_t combined_inputs = 0;
    size_t input_size = weights.size();
    for (size_t i = 0; i < input_size; ++i) {
        combined_inputs += weights[i] * inputs[i];
    }

    return combined_inputs / input_size;
}

real_t PatternFormationNeuron::output(real_t combined_inputs) const {
    real_t result;

    result = (theta * combined_inputs) - combined_inputs;
    result = 1 + alpha * std::exp(result);
    result = 1 / result;

    return result;
}


real_t PatternFormationNeuron::getAlpha() const {
    return alpha;
}

void PatternFormationNeuron::setAlpha(real_t alpha) {
    PatternFormationNeuron::alpha = alpha;
}

real_t PatternFormationNeuron::getTheta() const {
    return theta;
}

void PatternFormationNeuron::setTheta(real_t theta) {
    PatternFormationNeuron::theta = theta;
}

const std::vector<real_t> &PatternFormationNeuron::getWeights() const {
    return weights;
}

void PatternFormationNeuron::setWeights(const std::vector<real_t> &weights) {
    PatternFormationNeuron::weights = std::vector<real_t>(weights);

    // Weight normalization
    real_t weight_sum = 0;
    size_t weights_size = this->weights.size();
    for (size_t i = 0; i < weights_size; ++i) {
        weight_sum += this->weights[i];
    }
    for (size_t i = 0; i < weights_size; ++i) {
        this->weights[i] /= weight_sum;
    }
}

// FROM PERCENTAGE ------------------------------------------------------------
real_t PatternFormationNeuron::calculateAlphaFromPercentage(real_t alpha) const {
    return percentage_from_range(alpha, ALPHA_MIN, ALPHA_MAX);
}

real_t PatternFormationNeuron::calculateThetaFromPercentage(real_t theta) const {
    return percentage_from_range(theta, THETA_MIN, THETA_MAX);
}

// TO PERCENTAGE --------------------------------------------------------------
real_t PatternFormationNeuron::calculateAlphaPercentage(real_t alpha) const {
    return percentage_of_range(alpha, ALPHA_MIN, ALPHA_MAX);
}

real_t PatternFormationNeuron::calculateThetaPercentage(real_t theta) const {
    return percentage_of_range(theta, THETA_MIN, THETA_MAX);
}


real_t PatternFormationNeuron::setAlphaPercentage(real_t alpha) {
    this->alpha = calculateAlphaFromPercentage(alpha);
    return this->alpha;
}

real_t PatternFormationNeuron::setThetaPercentage(real_t theta) {
    this->theta = calculateThetaFromPercentage(theta);
    return this->theta;
}

