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
    // Read sensor data and feed the neural network
    double *inputs = new double[n_inputs];
    unsigned int p = 0;
    for (auto sensor : sensors) {
        sensor->read(&inputs[p]);
        p += sensor->inputs();
    }
    assert(p == n_inputs);

    std::vector<cpg::real_t> inputs_readings(sensors.size(), 0);
    for (int i=0; i<n_inputs; i++)
        inputs_readings[i] = (cpg::real_t) inputs[i];
    delete[] inputs;

    double *outputs = new double[cpgs.size()];
    for(cpg::CPGNetwork* cpg_network : cpgs) {
        cpg_network->update(inputs_readings, step);
    }

    p = 0;
    for (auto actuator: actuators) {
        actuator->update(&outputs[p], step);
        p += actuator->outputs();
    }
    assert(p == cpgs.size());

    delete[] outputs;

}
