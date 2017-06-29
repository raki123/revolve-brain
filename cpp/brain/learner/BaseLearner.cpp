#include "BaseLearner.h"

using namespace revolve::brain;

BaseLearner::BaseLearner(std::unique_ptr<BaseController> controller, const std::string &robot_name)
        :
        active_controller(std::move(controller)),
        robot_name(robot_name)
{
}

revolve::brain::BaseLearner::BaseLearner(const std::string &robot_name)
        :
        active_controller(),
        robot_name(robot_name)
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

