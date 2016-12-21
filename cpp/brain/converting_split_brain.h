#ifndef REVOLVE_BRAIN_CONVERTING_SPLIT_BRAIN_H_
#define REVOLVE_BRAIN_CONVERTING_SPLIT_BRAIN_H_

#include "split_brain.h"	
#include "evaluator.h"
#include <iostream>
#include <fstream>

namespace revolve {
namespace brain {


template <typename Phenotype, typename Genome>
class ConvSplitBrain : public SplitBrain<Phenotype,Genome> {
public:
    ConvSplitBrain(Phenotype (*convertForController)(Genome), Genome (*convertForLearner)(Phenotype), std::string model_name) 
        : eval_running(false)
	, reset(false)
	, reset_duration(3)
	, first_run(true)
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
	    this->controller->setGenome(convertForController_(this->learner->getNewGenome("test")));
	    start_eval_time_ = t;
	    evaluator_->start();
	    first_run = false;
	}
	if ((t - start_eval_time_) > (evaluation_rate_ + (reset?reset_duration:0))) { //&& generation_counter_ < max_evaluations_) {
	    double fitness = evaluator_->fitness();
	    writeCurrent(fitness);
	    this->learner->reportFitness("test", convertForLearner_(this->controller->getGenome()), fitness);
	    Phenotype controllerGenome = convertForController_(this->learner->getNewGenome("test"));
	    this->controller->setGenome(controllerGenome);
	    start_eval_time_ = t;
	    generation_counter_++;
	    eval_running = false;
	}
	if(reset && reset_duration > (t -start_eval_time_)) {
		//only works for spider
	    double outs[8] = {0.5,0,0.5,0,0.5,0,0.5,0};
	    double * out = &outs[0];
	    unsigned int p = 0;
	    for (auto actuator: actuators) {
		actuator->update(out + p, step);
		p += actuator->outputs();
	    }
	} 
	else {
	    if(!eval_running) {
	          evaluator_->start();
		  eval_running = true;
	    }
	    this->controller->update(actuators, sensors, t, step);
	}
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
    std::ofstream networkOutput(model_name + "-" + std::to_string(run_count) + "-" + std::to_string(generation_counter_) + ".dot");
}

protected:
    bool eval_running;
    bool reset;
    bool reset_duration;
    std::string model_name;
    bool first_run;
    int run_count;
    Phenotype (*convertForController_)(Genome);
    Genome (*convertForLearner_)(Phenotype);
    EvaluatorPtr evaluator_;
    double start_eval_time_ = 0;
    double evaluation_rate_ = 30;
    int generation_counter_ = 0;
};

}
}

#endif /* REVOLVE_BRAIN_CONVERTING_SPLIT_BRAIN_H_ */
