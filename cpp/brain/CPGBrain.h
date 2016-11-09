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
    std::string robot_name;
    EvaluatorPtr evaluator;
    unsigned int n_inputs;

    std::vector<cpg::CPGNetwork*> cpgs;
};

}
}


#endif //REVOLVE_BRAIN_CPGBRAIN_H
