//
// Created by matteo on 10/11/16.
//

#ifndef REVOLVE_BRAIN_TESTCPGBRAIN_H
#define REVOLVE_BRAIN_TESTCPGBRAIN_H

#include "brain/CPGBrain.h"
#include <vector>
#include <string>

class TestCPGBrain : public revolve::brain::CPGBrain
{
public:
    TestCPGBrain(std::string robot_name,
                 revolve::brain::EvaluatorPtr evaluator,
                 unsigned int n_actuators,
                 unsigned int n_sensors);

    void test_update(const std::vector<revolve::brain::ActuatorPtr> &actuators,
                     const std::vector<revolve::brain::SensorPtr> &sensors,
                     double t, double step);
};


#endif //REVOLVE_BRAIN_TESTCPGBRAIN_H
