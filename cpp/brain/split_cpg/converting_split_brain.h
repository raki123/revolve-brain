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
    ConvSplitBrain(G (*convertForController)(H), H (*convertForLearner)(G)) 
        : first_run(true)
        , convertForController_(convertForController)
	, convertForLearner_(convertForLearner) {};
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
	if(first_run) {
	    H genome = this->learner->getNewGenome("test");
	    G controllerGenome = convertForController_(genome);
	    this->controller->setGenome(controllerGenome);
	    start_eval_time_ = t;
	    evaluator_->start();
	    first_run = false;
	}
	if ((t - start_eval_time_) > evaluation_rate_ && generation_counter_ < max_evaluations_) {
	    double fitness = evaluator_->fitness();
	    this->learner->reportFitness("test", convertForLearner_(this->controller->getGenome()), fitness);
	    H genome = this->learner->getNewGenome("test");
	    G controllerGenome = convertForController_(genome);
	    this->controller->setGenome(controllerGenome);
	    start_eval_time_ = t;
	    evaluator_->start();
	    generation_counter_++;
	}
	this->controller->update(actuators, sensors, t, step);
    };

    
protected:
    bool first_run;
    G (*convertForController_)(H);
    H (*convertForLearner_)(G);
    EvaluatorPtr evaluator_;
    double start_eval_time_ = 0;
    double evaluation_rate_ = 30;
    int generation_counter_ = 0;
    int max_evaluations_ = 1000;
};

}
}

#endif /* REVOLVE_BRAIN_CONVERTING_SPLIT_BRAIN_H_ */
