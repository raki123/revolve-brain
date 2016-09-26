#ifndef REVOLVE_BRAIN_SUPGBRAIN_H
#define REVOLVE_BRAIN_SUPGBRAIN_H

#include "brain.h"
#include "supg/supgneuron.h"
#include "neat/asyncneat.h"
#include "evaluator.h"

#include <vector>
#include <memory>
#include <iostream>

namespace revolve {
namespace brain {

class SUPGBrain : public Brain
{
public:
    SUPGBrain(EvaluatorPtr evaluator,
              const std::vector< std::vector< float > > &neuron_coordinates,
              const std::vector< ActuatorPtr >& actuators,
              const std::vector< SensorPtr >& sensors);
    ~SUPGBrain();

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
        if ((t-start_eval_time) > SUPGBrain::FREQUENCY_RATE) {

            // check if to stop the experiment. Negative value for MAX_EVALUATIONS will never stop the experiment
            if (SUPGBrain::MAX_EVALUATIONS > 0 && generation_counter > SUPGBrain::MAX_EVALUATIONS) {
                std::cout << "Max Evaluations (" << SUPGBrain::MAX_EVALUATIONS << ") reached. stopping now." << std::endl;
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
        double *inputs = new double[n_inputs];
        unsigned int p = 0;
        for (auto sensor : sensors) {
            sensor->read(&inputs[p]);
            p += sensor->inputs();
        }
        assert(p == n_inputs);

        // load sensors
        for (unsigned int i = 0; i < n_inputs; i++) {
            neurons[0]->load_sensor(i, inputs[i]);
        }

        // Activate network and save results
        double *outputs = new double[n_outputs];
        for (unsigned int i = 0; i < neurons.size(); i++) {
            neurons[i]->activate(t);
            outputs[i] = neurons[i]->get_outputs()[0];
        }

        // send signals to actuators
        p = 0;
        for (auto actuator: actuators) {
            actuator->update(&outputs[p], step);
            p += actuator->outputs();
        }
        assert(p == n_outputs);

        delete[] inputs;
    }

private:
    double getFitness();
    void nextBrain();

    std::unique_ptr<AsyncNeat> neat;
    EvaluatorPtr evaluator;
    double start_eval_time;
    unsigned int generation_counter;
    std::shared_ptr< NeatEvaluation > current_evalaution;

    unsigned int n_inputs, n_outputs;
    std::vector< std::vector< float > > neuron_coordinates;
    std::vector< std::unique_ptr< SUPGNeuron > > neurons;

    /**
     * Number of evaluations before the program quits. Usefull to do long run
     * tests. If negative (default value), it will never stop.
     *
     * Takes value from env variable SUPG_MAX_EVALUATIONS.
     * Default value -1
     */
    const long MAX_EVALUATIONS; //= -1; // max number of evaluations
    /**
     * How long should an evaluation lasts (in seconds)
     *
     * Takes value from env variable SUPG_FREQUENCY_RATE
     * Default value 30 seconds
     */
    const double FREQUENCY_RATE; //= 30; // seconds
    /**
     * How long should the supg timer cicle be (in seconds)
     *
     * Takes value from env variable SUPG_CYCLE_LENGTH
     * Default value 5 seconds
     */
    const double CYCLE_LENGTH; // = 5; // seconds

    static long GetMAX_EVALUATIONSenv();
    static double GetFREQUENCY_RATEenv();
    static double GetCYCLE_LENGTHenv();
    static const char* getVARenv(const char* var_name);
};

}
}

#endif // REVOLVE_BRAIN_SUPGBRAIN_H
