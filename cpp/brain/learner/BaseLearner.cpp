#include "BaseLearner.h"

using namespace revolve::brain;

BaseLearner::BaseLearner(std::unique_ptr<BaseController> controller)
        :
        active_controller(std::move(controller))
{
}

revolve::brain::BaseLearner::BaseLearner()
        :
        active_controller()
{
}


BaseLearner::~BaseLearner()
{
}


BaseController *
BaseLearner::update(const std::vector<SensorPtr> &sensors,
                     double t,
                     double step)
{
  BaseController *controller_p = this->active_controller.get();
  assert(controller_p != nullptr);
  return controller_p;
}

