//
// Created by matteo on 09/11/16.
//

#ifndef REVOLVE_BRAIN_CPGBRAIN_H
#define REVOLVE_BRAIN_CPGBRAIN_H

#include "brain/cpg/CPGNetwork.h"
#include "brain/cpg/RythmGenerationNeuron.h"
#include "brain/cpg/PatternFormationNeuron.h"
#include "brain/cpg/MotoNeuron.h"
#include "Brain.h"
#include "Evaluator.h"

#include <iostream>
#include <map>

namespace revolve {
namespace brain {

class CPGBrain : public revolve::brain::Brain {
public:
    /**
     * @brief CPGBrain constructor
     *
     * @param robot_name the robot name (for logs)
     * @param evaluator pointer to the evaluator to evoluate the brain
     * @param n_actuators number of actuators
     * @param n_sensors number of sensors
     */
    CPGBrain(std::string robot_name,
             EvaluatorPtr evaluator,
             size_t n_actuators,
             size_t n_sensors);

    virtual ~CPGBrain();

    virtual void update(const std::vector<ActuatorPtr> &actuators,
                        const std::vector<SensorPtr> &sensors,
                        double t, double step) override;

    void setConnections(std::vector<std::vector<cpg::CPGNetwork::Weights>> connections);

protected:
    template<typename ActuatorContainer, typename SensorContainer>
    void update(const ActuatorContainer &actuators,
                const SensorContainer &sensors,
                double t,
                double step)
    {
        learner(t);
        controller(actuators, sensors, step);
    }

    virtual void learner(double t);

    template<typename ActuatorContainer, typename SensorContainer>
    void controller(const ActuatorContainer &actuators,
                            const SensorContainer &sensors,
                            double step)
    {
        // Read sensor data and feed the neural network
        double *inputs = new double[n_inputs];
        size_t p = 0;
        for (auto sensor : sensors) {
            sensor->read(&inputs[p]);
            p += sensor->inputs();
        }
        assert(p == n_inputs);

        std::vector<cpg::real_t> inputs_readings(n_inputs, 0);
        for (size_t i = 0; i < n_inputs; i++)
            inputs_readings[i] = (cpg::real_t) inputs[i];
        delete[] inputs;

        double *outputs = new double[cpgs.size()];
        for (size_t i = 0; i < cpgs.size(); i++) {
            cpg::CPGNetwork* cpg_network = cpgs[i];
            outputs[i] = cpg_network->update(inputs_readings, step) * 100;
        }

        p = 0;
        for (auto actuator: actuators) {
            actuator->update(&outputs[p], step);
            p += actuator->outputs();
        }
        assert(p == cpgs.size());

        delete[] outputs;
    }

    void connectionsToGenotype();

protected:
    // robot name
    const std::string robot_name;


    // -- controller data --

    // number of sensory inputs expected
    size_t n_inputs;
    // number of actuators
    size_t n_actuators;
    //list of cpgs
    std::vector<cpg::CPGNetwork*> cpgs;
    /** Connection matrix between the different servos
     * First is start of the connections, second is end.
     * Example: connections[0][1].we is the connection starting from servo 0
     * and reacing servo 1 for the RythmGenerationNeurons E
     */
    std::vector<std::vector<cpg::CPGNetwork::Weights>> connections;


    // -- learner data --

    // Evaluator for the brain
    const EvaluatorPtr evaluator;
    // last start of the evaluations. Needed to check duration of current evaluation.
    double start_eval_time_;
    // id of the current generation
    size_t generation_counter_;
    // needs to implement the grace period in starting the controller
    bool evaluator_started = false;


    // -- learner parameters --

    // How many seconds should every evaluation last
    const double evaluation_rate_;
    // Maximal number of evaluations
    const size_t max_evaluations_;

// RLPOWER SECTION ------------------------------------------------------------
protected:
    typedef std::vector<cpg::real_t> Genome;
    typedef std::shared_ptr<Genome> GenomePtr;
    typedef std::vector<GenomePtr> Policy;
    typedef std::shared_ptr<Policy> PolicyPtr;

    /**
     * @brief Evaluate the current policy and generate new
     *
     * @param fitness fitness of the current evaluation
     */
    void updatePolicy(double fitness);

    /**
     * Randomly select two policies and return the one with higher fitness
     * @return an iterator from 'ranked_policies_' map
     */
    std::map<double, PolicyPtr>::iterator binarySelection();

    /**
     * @brief update the new parameters in the cpgs
     */
    void genomeToPhenotype();

private:
    // Pointer to the current policy
    PolicyPtr current_policy_ = nullptr;
    // Container
    std::map<double, PolicyPtr, std::greater<cpg::real_t>> ranked_policies_;

    // sigma decay
    static const double SIGMA_DECAY_SQUARED;// = 0.98;

    // Type of the used algorithm
    char algorithm_type_ = 'B';
    // Maximal number of stored ranked policies
    size_t max_ranked_policies_; // = 10
    // Noise in generatePolicy() function
    double noise_sigma_; // = 0.008
    // Tau deviation for self-adaptive sigma
    double sigma_tau_correction_ = 0.2;

};

}
}


#endif //REVOLVE_BRAIN_CPGBRAIN_H
