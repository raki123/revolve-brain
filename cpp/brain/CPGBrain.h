//
// Created by matteo on 09/11/16.
//

#ifndef REVOLVE_BRAIN_CPGBRAIN_H
#define REVOLVE_BRAIN_CPGBRAIN_H

#include "brain/cpg/CPGNetwork.h"
#include "brain/cpg/RythmGenerationNeuron.h"
#include "brain/cpg/PatternFormationNeuron.h"
#include "brain/cpg/MotoNeuron.h"
#include "brain.h"
#include "evaluator.h"

#include <iostream>
#include <map>

namespace revolve {
namespace brain {

class CPGBrain : public revolve::brain::Brain {
public:
    CPGBrain(std::string robot_name,
             EvaluatorPtr evaluator,
             unsigned int n_actuators,
             unsigned int n_sensors);

    virtual ~CPGBrain();

    virtual void update(const std::vector<ActuatorPtr> &actuators,
                        const std::vector<SensorPtr> &sensors,
                        double t, double step) override;

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
        unsigned int p = 0;
        for (auto sensor : sensors) {
            sensor->read(&inputs[p]);
            p += sensor->inputs();
        }
        assert(p == n_inputs);

        std::vector<cpg::real_t> inputs_readings(n_inputs, 0);
        for (int i=0; i<n_inputs; i++)
            inputs_readings[i] = (cpg::real_t) inputs[i];
        delete[] inputs;

        double *outputs = new double[cpgs.size()];
        for(int i=0; i<cpgs.size(); i++) {
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

protected:
    const std::string robot_name;

    // controller data
    unsigned int n_inputs;
    unsigned int n_actuators;
    std::vector<cpg::CPGNetwork*> cpgs;

    // learner data
    const EvaluatorPtr evaluator;
    double start_eval_time_;
    unsigned int generation_counter_; // Number of current generation

    // learner parameters
    const double evaluation_rate_;
    const unsigned int max_evaluations_; // Maximal number of evaluations
    bool evaluator_started = false;

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
    PolicyPtr current_policy_ = nullptr; // Pointer to the current policy
    std::map<double, PolicyPtr, std::greater<cpg::real_t>> ranked_policies_; // Container

    static const double SIGMA_DECAY_SQUARED;// = 0.98; // sigma decay

    char algorithm_type_ = 'B'; // Type of the used algorithm
    unsigned int max_ranked_policies_; // 10 - Maximal number of stored ranked policies
    double noise_sigma_; // 0.008 - Noise in generatePolicy() function
    double sigma_tau_correction_ = 0.2; // Tau deviation for self-adaptive sigma

};

}
}


#endif //REVOLVE_BRAIN_CPGBRAIN_H
