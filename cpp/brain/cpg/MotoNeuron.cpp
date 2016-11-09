//
// Created by matteo on 09/11/16.
//

#include "MotoNeuron.h"
#include <sstream>
#include <cmath>

using namespace revolve::brain::cpg;

MotoNeuron::MotoNeuron(real_t v_max)
    : v_max(v_max)
{}

MotoNeuron::~MotoNeuron()
{}

std::vector<real_t> MotoNeuron::update(std::vector<real_t> inputs, real_t delta_time)
{
    if (inputs.size() != 2) {
        std::stringstream ss;
        ss << "input size should be 2, instead is "
           << inputs.size();
        throw invalid_input_exception(ss.str());
    }

    return { output(inputs[0], inputs[1])};
}

real_t MotoNeuron::output(real_t pfe, real_t pff) const
{
    real_t result;

    result = -2 * (pfe - pff);
    result = 1 + std::exp(result/v_max);
    result = (2 / result) - 1;
    result *= v_max;

    return result;
}
