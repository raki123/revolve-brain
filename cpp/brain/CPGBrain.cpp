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
    , n_inputs(n_sensors)
    , cpgs(n_actuators, nullptr)
    , evaluator(evaluator)
    , start_eval_time_(-1)
    , generation_counter_(0)
        // TODO read this values from config file
    , evaluation_rate_(30)
    , max_evaluations_(1000)
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

void CPGBrain::learner(double t)
{
    if (offline)
        return;

    if (start_eval_time_ < 0)
        start_eval_time_ = t;

    if ((t - start_eval_time_) > evaluation_rate_ && generation_counter_ < max_evaluations_) {
        //TODO learner
        double fitness = evaluator->fitness();
        std::cout << "EVALUATION! fitness = " << fitness << std::endl;
        start_eval_time_ = t;
        evaluator->start();
    }
}

