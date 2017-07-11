#ifndef REVOLVE_BRAIN_SUPGBRAIN_H
#define REVOLVE_BRAIN_SUPGBRAIN_H

#include "Brain.h"
#include "supg/SUPGNeuron.h"
#include "neat/AsyncNEAT.h"
#include "Evaluator.h"

#include <vector>
#include <memory>
#include <iostream>
#include <cstdlib>

namespace revolve {
namespace brain {

class SUPGBrain : public Brain
{
//METHODS
public:
    SUPGBrain(const std::string &robot_name,
              EvaluatorPtr evaluator,
              const std::vector< std::vector< float > > &neuron_coordinates,
              const std::vector< ActuatorPtr >& actuators,
              const std::vector< SensorPtr >& sensors);
    ~SUPGBrain() {}

    using Brain::update;
    virtual void update(const std::vector< ActuatorPtr >& actuators,
                        const std::vector< SensorPtr >& sensors,
                        double t, double step) override;

protected:
    SUPGBrain(EvaluatorPtr evaluator);

//// Templates ---------------------------------------------------------

    template<typename ActuatorContainer, typename SensorContainer>
    void controller(const ActuatorContainer &actuators,
                    const SensorContainer &sensors,
                    double t,
                    double step)
    {
        assert(n_outputs == actuators.size());

        // Read sensor data and feed the neural network
        double *inputs = new double[n_inputs];
        size_t p = 0;
        for (auto sensor : sensors) {
            sensor->read(&inputs[p]);
            p += sensor->inputs();
        }
        assert(p == n_inputs);

        // load sensors
        for (size_t i = 0; i < n_inputs; i++) {
            neurons[0]->load_sensor(i, inputs[i]);
        }

        // Activate network and save results
        double *outputs = new double[n_outputs];
        for (size_t i = 0; i < neurons.size(); i++) {
            neurons[i]->activate(t);
            outputs[i] = neurons[i]->get_outputs()[0] * 2 - 1;
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

    template<typename ActuatorContainer, typename SensorContainer>
    void update(const ActuatorContainer &actuators,
                const SensorContainer &sensors,
                double t,
                double step)
    {
        this->learner(t); // CAREFUL, virtual function
        controller<ActuatorContainer, SensorContainer>(actuators, sensors, t, step);
    }

    void init_async_neat();

protected:
    virtual void learner(double t);
    virtual double getFitness();
    void nextBrain();

    static long GetMAX_EVALUATIONSenv();
    static double GetFREQUENCY_RATEenv();
    static double GetCYCLE_LENGTHenv();
    static const char* getVARenv(const char* var_name);

// DATA
protected:
    size_t n_inputs, n_outputs;
    std::vector< std::vector< float > > neuron_coordinates;

    std::unique_ptr<AsyncNeat> neat;
    EvaluatorPtr evaluator;
    const std::string robot_name;
    double start_eval_time;
    long generation_counter;
    std::shared_ptr< NeatEvaluation > current_evalaution;

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
};

}
}

#endif // REVOLVE_BRAIN_SUPGBRAIN_H
