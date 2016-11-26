#ifndef REVOLVE_BRAIN_CONVERTING_SPLIT_BRAIN_H_
#define REVOLVE_BRAIN_CONVERTING_SPLIT_BRAIN_H_

#include "split_brain.h"
#include "../evaluator.h"
#include <iostream>

namespace revolve {
namespace brain {


template <typename G, typename H>
class ConvSplitBrain : public SplitBrain<G,H> {
public:
    ConvSplitBrain(G (*convertForController)(H), H (*convertForLearner)(G)) : convertForController_(convertForController), convertForLearner_(convertForLearner) {};
    virtual ~ConvSplitBrain() {};
    
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
	if (start_eval_time_ == 0 || (t - start_eval_time_) > evaluation_rate_ && generation_counter_ < max_evaluations_) {
	    double fitness = evaluator_->fitness();
	    if(start_eval_time_ != 0) {
		std::cout << fitness << std::endl;
		this->learner->reportFitness("test", convertForLearner_(this->controller->getGenome()), fitness);
	    }
	    this->controller->setGenome(convertForController_(this->learner->getNewGenome("test")));
	    start_eval_time_ = t;
	    evaluator_->start();
	}
	this->controller->update(actuators, sensors, t, step);
    };

    
protected:
    G (*convertForController_)(H) = NULL;
    H (*convertForLearner_)(G) = NULL;
    EvaluatorPtr evaluator_ = NULL;
    double start_eval_time_ = 0;
    double evaluation_rate_ = 30;
    int generation_counter_ = 0;
    int max_evaluations_ = 1000;
};

}
}

#endif /* REVOLVE_BRAIN_CONVERTING_SPLIT_BRAIN_H_ */
