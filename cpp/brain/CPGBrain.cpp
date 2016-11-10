//
// Created by matteo on 09/11/16.
//

#include "CPGBrain.h"

using namespace revolve::brain;

CPGBrain::CPGBrain(std::string robot_name,
                   EvaluatorPtr evaluator,
                   unsigned int n_actuators,
                   unsigned int n_sensors)
    : Brain()
    , robot_name(robot_name)
    , evaluator(evaluator)
    , n_inputs(n_sensors)
    , cpgs(n_actuators, nullptr)
{
    for(int i=0; i<n_actuators; i++) {
        cpgs[i] = new cpg::CPGNetwork(n_sensors);
    }
}

CPGBrain::~CPGBrain()
{
    for(cpg::CPGNetwork* ptr : cpgs)
        delete ptr;
}

void CPGBrain::update(const std::vector<ActuatorPtr> &actuators,
                      const std::vector<SensorPtr> &sensors,
                      double t, double step)
{
    this->update<std::vector<ActuatorPtr>, std::vector<SensorPtr>>
            (actuators, sensors, t, step);
}
