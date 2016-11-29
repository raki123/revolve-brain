#ifndef REVOLVE_BRAIN_BASIC_NEAT_BRAIN_H
#define REVOLVE_BRAIN_BASIC_NEAT_BRAIN_H

#include "brain.h"
#include "neat/asyncneat.h"
#include "evaluator.h"
#include "split_cpg/extended_neural_network_controller.h"

#include <vector>
#include <memory>
#include <iostream>

namespace revolve {
namespace brain {

class BasicBrain : public Brain
{
//METHODS
public:
    BasicBrain(EvaluatorPtr evaluator,
              unsigned int n_actuators,
              unsigned int n_sensors);
    ~BasicBrain() {}

    virtual void update(const std::vector< ActuatorPtr >& actuators,
                        const std::vector< SensorPtr >& sensors,
                        double t, double step) override;

protected:

    template<typename ActuatorContainer, typename SensorContainer>
    void update(const ActuatorContainer &actuators,
                const SensorContainer &sensors,
                double t,
                double step)
    {
        // Evaluate policy on certain time limit
        if ((t-start_eval_time) > BasicBrain::FREQUENCY_RATE || firstcall) {
	    firstcall = false;
            // check if to stop the experiment. Negative value for MAX_EVALUATIONS will never stop the experiment
            if (BasicBrain::MAX_EVALUATIONS > 0 && generation_counter > BasicBrain::MAX_EVALUATIONS) {
                std::cout << "Max Evaluations (" << BasicBrain::MAX_EVALUATIONS << ") reached. stopping now." << std::endl;
                std::exit(0);
            }
            generation_counter++;
            std::cout << "################# EVALUATING NEW BRAIN !!!!!!!!!!!!!!!!!!!!!!!!! (generation " << generation_counter << " )" << std::endl;
            this->nextBrain();
            start_eval_time = t;
            evaluator->start();
        }
	current_evalaution->getOrganism()->net->update(actuators, sensors, t, step);
    }

    void init_async_neat();

private:
    double getFitness();
    void nextBrain();
private:
    unsigned int n_inputs;
    unsigned int n_outputs;
    std::unique_ptr<AsyncNeat> neat;
    EvaluatorPtr evaluator;
    double start_eval_time;
    unsigned int generation_counter;
    std::shared_ptr< NeatEvaluation > current_evalaution;
    //ExtNNController *cppn;
    bool firstcall;
    /**
     * Number of evaluations before the program quits. Usefull to do long run
     * tests. If negative (default value), it will never stop.
     *
     * Takes value from env variable SUPG_MAX_EVALUATIONS.
     * Default value -1
     */
    const long MAX_EVALUATIONS= -1; // max number of evaluations
    /**
     * How long should an evaluation lasts (in seconds)
     *
     * Takes value from env variable SUPG_FREQUENCY_RATE
     * Default value 30 seconds
     */
    const double FREQUENCY_RATE= 30; // seconds
    /**
     * How long should the supg timer cicle be (in seconds)
     *
     * Takes value from env variable SUPG_CYCLE_LENGTH
     * Default value 5 seconds
     */
    const double CYCLE_LENGTH = 5; // seconds
};

}
}

#endif // REVOLVE_BRAIN_BASIC_NEAT_BRAIN_H
