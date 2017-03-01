#include "BaseLearner.h"

using namespace revolve::brain;

base_learner::base_learner(std::unique_ptr<base_controller> controller)
    : active_controller(std::move(controller))
{
}

revolve::brain::base_learner::base_learner()
    : active_controller()
{
}



base_learner::~base_learner()
{
}


base_controller * base_learner::update(const std::vector<SensorPtr>& sensors,
                                              double t, double step)
{
    base_controller *controller_p = this->active_controller.get();
    assert(controller_p != nullptr);
    return controller_p;
}

