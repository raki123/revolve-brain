#ifndef REVOLVEBRAIN_BRAIN_CONTROLLER_EXTNN_LEAKYINTEGRATOR_H_
#define REVOLVEBRAIN_BRAIN_CONTROLLER_EXTNN_LEAKYINTEGRATOR_H_

#include "ENeuron.h"
#include "NeuralConnection.h"


namespace revolve {
namespace brain {

class LeakyIntegrator
        : public Neuron
{
public:
    /**
  * Constructor for a leaky integrator neuron
  * @param id: string to identify the neuron
  * @param params: parameters that specify the behavior of the neuron
  * @return pointer to the leaky integrator neuron
  */
    LeakyIntegrator(const std::string &id,
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

    virtual void
    reset() override;

protected:
    double bias_; //bias of the neuron
    double tau_; //tau of the neuron

    double lastTime_; //last time the output was calculated

    double stateDeriv_; //current value of the derivation of the state
    double state_; //current state value
};

}
}

#endif // REVOLVEBRAIN_BRAIN_CONTROLLER_EXTNN_LEAKYINTEGRATOR_H_