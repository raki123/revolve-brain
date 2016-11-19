#include "simple_split_brain.h"

namespace revolve {
namespace brain {

template <typename G>
void SimpleSplitBrain<G>::update(const std::vector< ActuatorPtr > & actuators,
				      const std::vector< SensorPtr > & sensors,
				      double t,
				      double step)
{
    if ((t - start_eval_time_) > evaluation_rate_ && generation_counter_ < max_evaluations_) {
	this->learner->reportFitness("test", this->controller->getGenome(), 1);
	this->controller->setGenome(this->learner->newGenome("test"));
	start_eval_time_ = t;
    }
    this->controller->update(actuators, sensors, t, step);
}
    
}
}

