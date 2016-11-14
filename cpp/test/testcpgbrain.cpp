//
// Created by matteo on 10/11/16.
//

#include "testcpgbrain.h"

#include "testevaluator.h"
#include "testactuator.h"
#include "testsensor.h"

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

int main(int argc, char *argv[]) {
    std::cout << "testing supgbrain" << std::endl;

    revolve::brain::EvaluatorPtr testEvaluator = boost::make_shared<TestEvaluator>();

    std::vector<revolve::brain::SensorPtr> sensors;
    for (int i=0; i<10; i++)
        sensors.push_back(
            boost::make_shared<TestSensor>()
        );

    std::vector<revolve::brain::ActuatorPtr> actuators;
    for (int i=0; i<10; i++)
        actuators.push_back(
            boost::make_shared<TestActuator>()
        );

    TestCPGBrain testBrain(
        "robot_test",
        testEvaluator,
        actuators.size(),
        sensors.size()
    );

    float delta_time = 0.1;

    for (float time = 0; time < 100; time += delta_time) {
        testBrain.test_update(actuators,
                              sensors,
                              time,delta_time);
    }


}

TestCPGBrain::TestCPGBrain(std::string robot_name,
                           revolve::brain::EvaluatorPtr evaluator,
                           unsigned int n_actuators,
                           unsigned int n_sensors)
    : revolve::brain::CPGBrain(
        robot_name,
        evaluator,
        n_actuators,
        n_sensors
    )
{

}

void TestCPGBrain::test_update(const std::vector<revolve::brain::ActuatorPtr>& actuators,
                               const std::vector<revolve::brain::SensorPtr>& sensors,
                               double t, double step)
{
    this->update(actuators,sensors,t,step);
}
