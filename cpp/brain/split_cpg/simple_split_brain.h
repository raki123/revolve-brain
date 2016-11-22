#ifndef REVOLVE_BRAIN_SIMPLE_SPLIT_BRAIN_H_
#define REVOLVE_BRAIN_SIMPLE_SPLIT_BRAIN_H_

#include "split_brain.h"
#include "../evaluator.h"
#include <iostream>

namespace revolve {
namespace brain {


template <typename G>
class SimpleSplitBrain : public SplitBrain<G> {
public:
    SimpleSplitBrain() {};
    virtual ~SimpleSplitBrain() {};
    
    /**
     * Update step called for the brain.
     * @param actuators List of actuators
     * @param sensors List of sensors
     * @param t Current simulation time
     * @param step Actuation step size in seconds
     */
    virtual void update(const std::vector< ActuatorPtr > & actuators,
                        const std::vector< SensorPtr > & sensors,
                        double t,
                        double step) 
    {
	if ((t - start_eval_time_) > evaluation_rate_ && generation_counter_ < max_evaluations_) {
	    double fitness = evaluator_->fitness();
	    std::cout << fitness << std::endl;
	    this->learner->reportFitness("test", this->controller->getGenome(), fitness);
	    this->controller->setGenome(this->learner->getNewGenome("test"));
	    start_eval_time_ = t;
	    evaluator_->start();
	}
	this->controller->update(actuators, sensors, t, step);
    };

    
protected:
    EvaluatorPtr evaluator_ = NULL;
    double start_eval_time_ = 0;
    double evaluation_rate_ = 30;
    int generation_counter_ = 0;
    int max_evaluations_ = 1000;
};

}
}

#endif /* REVOLVE_BRAIN_SIMPLE_SPLIT_BRAIN_H_ */
