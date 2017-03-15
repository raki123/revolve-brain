//
// Created by matteo on 3/15/17.
//

#include "GenericLearnerBrain.h"

using namespace revolve::brain;

GenericLearnerBrain::GenericLearnerBrain(std::unique_ptr<BaseLearner> learner)
    : learner(std::move(learner))
{

}

GenericLearnerBrain::GenericLearnerBrain(BaseLearner *learner)
    : learner(learner)
{

}

void GenericLearnerBrain::update(const std::vector<ActuatorPtr> &actuators,
                                 const std::vector<SensorPtr> &sensors,
                                 double t, double step)
{
  BaseController *controller = learner->update(sensors, t, step);
  controller->update(actuators, sensors, t, step);
}
