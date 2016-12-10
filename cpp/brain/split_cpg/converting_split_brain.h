#ifndef REVOLVE_BRAIN_CONVERTING_SPLIT_BRAIN_H_
#define REVOLVE_BRAIN_CONVERTING_SPLIT_BRAIN_H_

#include "split_brain.h"
#include "../evaluator.h"
#include <iostream>
#include <fstream>

namespace revolve {
namespace brain {


template <typename G, typename H>
class ConvSplitBrain : public SplitBrain<G,H> {
public:
    ConvSplitBrain(G (*convertForController)(H), H (*convertForLearner)(G), std::string model_name) 
        : first_run(true)
	, run_count(0)
        , convertForController_(convertForController)
	, convertForLearner_(convertForLearner) {
		int index = model_name.find("-");
		this->model_name = model_name.substr(0, index);
	};
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
	if ((t - start_eval_time_) > evaluation_rate_) { //&& generation_counter_ < max_evaluations_) {
	    double fitness = evaluator_->fitness();
	    writeCurrent(fitness);
	    this->learner->reportFitness("test", convertForLearner_(this->controller->getGenome()), fitness);
	    H genome = this->learner->getNewGenome("test");
	    G controllerGenome = convertForController_(genome);
	    this->controller->setGenome(controllerGenome);
	    start_eval_time_ = t;
	    evaluator_->start();
	    generation_counter_++;
	}
	this->controller->update(actuators, sensors, t, step);
    }
    
void writeCurrent(double fitness) 
{
    std::ofstream outputFile;
    if(generation_counter_ == 0) {
        std::ifstream infile(model_name + "-" + std::to_string(run_count) + ".log");
        while(infile.good()) {
	    run_count++;
	    infile = std::ifstream(model_name + "-" + std::to_string(run_count) + ".log");
	}
    }
    outputFile.open(model_name + "-" + std::to_string(run_count) + ".log", std::ios::app | std::ios::out | std::ios::ate);
    outputFile << "- generation: " << generation_counter_ << std::endl;
    outputFile << "  velocities:" << std::endl;
    outputFile << "  - " << fitness << std::endl;
    outputFile.close();
}
    
protected:
    std::string model_name;
    bool first_run;
    int run_count;
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
