#ifndef REVOLVE_BRAIN_BASIC_NEAT_BRAIN_H
#define REVOLVE_BRAIN_BASIC_NEAT_BRAIN_H

#include "brain.h"
#include "neat/asyncneat.h"
#include "evaluator.h"
#include "neat/accneat/src/network/cpu/cpunetwork.h"

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
              const std::vector< ActuatorPtr >& actuators,
              const std::vector< SensorPtr >& sensors);
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
        if ((t-start_eval_time) > BasicBrain::FREQUENCY_RATE) {

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

        assert(n_outputs == actuators.size());

        // Read sensor data and feed the neural network
        std::vector<double> inputs(n_inputs,0);
        unsigned int p = 0;
        for (auto sensor : sensors) {
            sensor->read(&inputs[p]);
            p += sensor->inputs();
        }
        assert(p == n_inputs);
	std::vector<float> floatIns(n_inputs,0);
	for(unsigned int i = 0; i < n_inputs; i++) {
	    floatIns[i] = inputs[i];
// 	    std::cout << inputs[i] << " " << floatIns[i] << " ";
	}	
// 	std::cout << n_inputs << " " << n_outputs;
	NEAT::NetDims dims = cppn->get_dims();
	std::cout << dims.nlinks << " " << dims.nnodes.all;
// 	std::cout << std::endl;
	cppn->set_activations(floatIns);
	

        // Activate network and save results
	cppn->activate(1);
	std::vector<float> outputvect(n_outputs, 0);
        cppn->get_activations(outputvect).data();
	std::vector<double> outputDouble(n_outputs,0);
	for(unsigned int i = 0; i < n_outputs; i++) {
	    outputDouble[i] = outputvect[i];
// 	    std::cout << outputDouble[i] << " " << outputvect[i] << " ";
	}	
// 	std::cout << std::endl;
	double *outputs = outputDouble.data();
        // send signals to actuators
        p = 0;
        for (auto actuator: actuators) {
            actuator->update(&outputs[p], step);
            p += actuator->outputs();
        }
        assert(p == n_outputs);
    }

    void init_async_neat();

private:
    double getFitness();
    void nextBrain();
// DATA
protected:
    unsigned int n_inputs, n_outputs;

private:
    std::unique_ptr<AsyncNeat> neat;
    EvaluatorPtr evaluator;
    double start_eval_time;
    unsigned int generation_counter;
    std::shared_ptr< NeatEvaluation > current_evalaution;
    NEAT::CpuNetwork *cppn;

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
