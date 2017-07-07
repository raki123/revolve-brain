#ifndef REVOLVEBRAIN_BRAIN_CONTROLLER_EXTNN_LINEARNEURON_H_
#define REVOLVEBRAIN_BRAIN_CONTROLLER_EXTNN_LINEARNEURON_H_

#include "ENeuron.h"
#include "NeuralConnection.h"

namespace revolve {
namespace brain {

class LinearNeuron
        : public Neuron
{
public:
    /**
    * Constructor for a linear neuron
    * @param id: string to identify the neuron
    * @param params: parameters that specify the behavior of the neuron
    * @return pointer to the linear neuron
    */
    LinearNeuron(const std::string &id,
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
    double gain_; //gain of the neuron
    double bias_; //bias of the neuron
};

}
}

#endif // REVOLVEBRAIN_BRAIN_CONTROLLER_EXTNN_LINEARNEURON_H_