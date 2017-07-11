//
// Created by matteo on 14/11/16.
//

#include "test_SUPGBrain.h"

#include "test_Evaluator.h"
#include "test_Actuator.h"
#include "test_Sensor.h"

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

const std::string test_name = "TestSUPGBrain";

int main() {
    std::cout << "testing supgbrain" << std::endl;
    AsyncNeat::Init(test_name);

    revolve::brain::EvaluatorPtr testEvaluator = boost::make_shared<TestEvaluator>();

    std::vector<revolve::brain::SensorPtr> sensors;
    for (int i=0; i<10; i++)
        sensors.push_back(
            boost::make_shared<TestSensor>()
        );

    std::vector< std::vector< float > > neuron_coordinates;
    std::vector<revolve::brain::ActuatorPtr> actuators;
    for (int i=0; i<10; i++) {
        actuators.push_back(
            boost::make_shared<TestActuator>()
        );
        neuron_coordinates.push_back(
            {-i*1.0f, i*1.0f}
        );
    }

    TestSUPGBrain testBrain(
        testEvaluator,
        neuron_coordinates,
        actuators,
        sensors
    );

    float delta_time = 0.1;

    for (float time = 0; time < 1000; time += delta_time) {
        testBrain.test_update(actuators,
                              sensors,
                              time,delta_time);
    }

    AsyncNeat::CleanUp();
}

TestSUPGBrain::TestSUPGBrain(revolve::brain::EvaluatorPtr evaluator,
                             const std::vector< std::vector< float > > &neuron_coordinates,
                             const std::vector<revolve::brain::ActuatorPtr> &actuators,
                             const std::vector<revolve::brain::SensorPtr> &sensors)
        : revolve::brain::SUPGBrain(test_name,
                                    evaluator,
                                    neuron_coordinates,
                                    actuators,
                                    sensors)
{

}

void TestSUPGBrain::test_update(const std::vector<revolve::brain::ActuatorPtr>& actuators,
                                const std::vector<revolve::brain::SensorPtr>& sensors,
                                double t, double step)
{
    this->update(actuators, sensors, t, step);
}
