//
// Created by matteo on 09/11/16.
//

#ifndef REVOLVE_BRAIN_CPGBRAIN_H
#define REVOLVE_BRAIN_CPGBRAIN_H

#include "brain/cpg/CPGNetwork.h"
#include "brain/cpg/RythmGenerationNeuron.h"
#include "brain/cpg/PatternFormationNeuron.h"
#include "brain/cpg/MotoNeuron.h"
#include "brain.h"
#include "evaluator.h"

#include <iostream>

namespace revolve {
namespace brain {

class CPGBrain : public revolve::brain::Brain {
public:
    CPGBrain(std::string robot_name,
             EvaluatorPtr evaluator,
             unsigned int n_actuators,
             unsigned int n_sensors);

    virtual ~CPGBrain();

    virtual void update(const std::vector<ActuatorPtr> &actuators,
                        const std::vector<SensorPtr> &sensors,
                        double t, double step) override;

protected:
    template<typename ActuatorContainer, typename SensorContainer>
    void update(const ActuatorContainer &actuators,
                const SensorContainer &sensors,
                double t,
                double step)
    {
        std::cout << "porca madonna1" << std::endl;
        // Read sensor data and feed the neural network
        double *inputs = new double[n_inputs];
        unsigned int p = 0;
        for (auto sensor : sensors) {
            sensor->read(&inputs[p]);
            p += sensor->inputs();
        }
        assert(p == n_inputs);
        std::cout << "porca madonna2" << std::endl;

        std::vector<cpg::real_t> inputs_readings(sensors.size(), 0);
        for (int i=0; i<n_inputs; i++)
            inputs_readings[i] = (cpg::real_t) inputs[i];
        delete[] inputs;

        std::cout << "porca madonna3" << std::endl;
        double *outputs = new double[cpgs.size()];
        for(int i=0; i<cpgs.size(); i++) {
            cpg::CPGNetwork* cpg_network = cpgs[i];
            outputs[i] = cpg_network->update(inputs_readings, step);
        }
        std::cout << "porca madonna4" << std::endl;

        p = 0;
        for (auto actuator: actuators) {
            actuator->update(&outputs[p], step);
            p += actuator->outputs();
        }
        assert(p == cpgs.size());

        delete[] outputs;
    }

protected:
    std::string robot_name;
    EvaluatorPtr evaluator;
    unsigned int n_inputs;

    std::vector<cpg::CPGNetwork*> cpgs;
};

}
}


#endif //REVOLVE_BRAIN_CPGBRAIN_H
