/*
 *
 */

#include "NeuralNetwork.h"

#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <cstdlib>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <cmath>
#include <cstring>

using namespace revolve::brain;

NeuralNetwork::NeuralNetwork():
    flipState_(false),
    nInputs_(0),
    nOutputs_(0),
    nHidden_(0),
    nNonInputs_(0)
{
    // Initialize weights, input and states to zero by default
    memset(inputWeights_, 0, sizeof(inputWeights_));
    memset(outputWeights_, 0, sizeof(outputWeights_));
    memset(hiddenWeights_, 0, sizeof(hiddenWeights_));
    memset(state1_, 0, sizeof(state1_));
    memset(state2_, 0, sizeof(state2_));
    memset(input_, 0, sizeof(input_));

}

NeuralNetwork::NeuralNetwork(std::string modelName,
                             std::vector< ActuatorPtr > & actuators,
                             std::vector< SensorPtr > & sensors):
    flipState_(false),
    nInputs_(0),
    nOutputs_(0),
    nHidden_(0),
    nNonInputs_(0)
{
    // Initialize weights, input and states to zero by default
    memset(inputWeights_, 0, sizeof(inputWeights_));
    memset(outputWeights_, 0, sizeof(outputWeights_));
    memset(hiddenWeights_, 0, sizeof(hiddenWeights_));
    memset(state1_, 0, sizeof(state1_));
    memset(state2_, 0, sizeof(state2_));
    memset(input_, 0, sizeof(input_));

    // We now setup the neural network and its parameters. The end result
    // of this operation should be that we can iterate/update all sensors in
    // a straightforward manner, likewise for the motors. We therefore first
    // create a map of all neurons, telling us how many there are for each
    // type, and what their properties are. We then iterate all sensors and
    // motors, creating the adequate neurons in place as we do so.
    //TODO
}

NeuralNetwork::~NeuralNetwork()
{}

void NeuralNetwork::step(double time) {
    unsigned int i = 0;
    unsigned int j = 0;

    if (nOutputs_ == 0) {
        return;
    }

    double *curState, *nextState;
    if (flipState_) {
        curState = state2_;
        nextState = state1_;
    } else {
        curState = state1_;
        nextState = state2_;
    }


    unsigned int maxNonInputs = MAX_HIDDEN_NEURONS + MAX_OUTPUT_NEURONS;
    for (i = 0; i < nNonInputs_; ++i) {
        double curNeuronActivation = 0;

        // Add input neuron values
        for (j = 0; j < nInputs_; ++j) {
            curNeuronActivation += inputWeights_[maxNonInputs * j + i]
                    * input_[j];
        }

        // Add output neuron values
        for (j = 0; j < nOutputs_; ++j) {
            curNeuronActivation += outputWeights_[maxNonInputs * j + i] * curState[j];
        }

        // Add hidden neuron values
        for (j = 0; j < nHidden_; ++j) {
            curNeuronActivation += hiddenWeights_[maxNonInputs * j + i] * curState[nOutputs_ + j];
        }

        unsigned int base = MAX_NEURON_PARAMS * i;
        switch (types_[i]) {
        case SIGMOID:
            /* params are bias, gain */
            curNeuronActivation -= params_[base];
            nextState[i] = 1.0
                / (1.0 + exp(-params_[base + 1] *
                        curNeuronActivation));
            break;
        case SIMPLE:
            /* linear, params are bias, gain */
            curNeuronActivation -= params_[base];
            nextState[i] = params_[base + 1] *
                    curNeuronActivation;
            break;
        case OSCILLATOR: { // Use the block to prevent "crosses initialization" error
            /* params are period, phase offset, gain (amplitude) */
            double period = params_[base];
            double phaseOffset = params_[base + 1];
            double gain = params_[base + 2];

            /* Value in [0, 1] */
            nextState[i] = ((sin( (2.0*M_PI/period) *
                    (time - period * phaseOffset))) + 1.0) / 2.0;

            /* set output to be in [0.5 - gain/2, 0.5 + gain/2] */
            nextState[i] = (0.5 - (gain/2.0) +
                    nextState[i] * gain);
        } break;
        default:
            // Unsupported type should never happen
            std::cerr << "Invalid neuron type during processing, must be a bug." << std::endl;
            throw std::runtime_error("Robot brain error");
        }
    }

    flipState_ = !flipState_;
}

void NeuralNetwork::update(const std::vector< ActuatorPtr >& actuators,
                           const std::vector< SensorPtr >& sensors,
                           double t,
                           double step)
{
    boost::mutex::scoped_lock lock(networkMutex_);

    // Read sensor data and feed the neural network
    unsigned int p = 0;
    for (auto sensor : sensors) {
        sensor->read(&input_[p]);
        p += sensor->inputs();
    }

    this->step(t);

    // Since the output neurons are the first in the state
    // array we can just use it to update the motors directly.
    double * output = flipState_ ? &state2_[0] : &state1_[0];

    // Send new signals to the motors
    p = 0;
    for (auto actuator: actuators) {
        actuator->update(&output[p], step);
        p += actuator->outputs();
    }
}
