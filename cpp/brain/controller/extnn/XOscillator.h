#ifndef REVOLVE_GAZEBO_BRAIN_XOSCILLATOR_H_
#define REVOLVE_GAZEBO_BRAIN_XOSCILLATOR_H_

#include "ENeuron.h"
#include "NeuralConnection.h"

namespace revolve {
namespace brain {

class XOscillator
        : public Neuron
{
public:
    /**
    * Constructor for a Xoscilator neuron
    * @param id: string to identify the neuron
    * @param params: parameters that specify the behavior of the neuron
    * @return pointer to the Xoscilator neuron
    */
    XOscillator(const std::string &id,
                const std::map<std::string, double> &params);

    /**
    * Method to calculate the output of the neuron
    * @param t: current time
    * @return the output of the neuron at time t
    */
    virtual double
    CalculateOutput(double t) override;

    virtual std::map<std::string, double>
    getNeuronParameters() override;

    virtual void
    setNeuronParameters(std::map<std::string, double> params) override;

    virtual std::string
    getType() override;


protected:
    double tau_; //TODO:: find out what these parameters mean

    double lastTime_; //last time the output was calculated
    double stateDeriv_;
};

}
}

#endif // REVOLVE_GAZEBO_BRAIN_XOSCILLATOR_H_