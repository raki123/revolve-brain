//
// Created by Milan Jelisavcic on 28/03/16.
//

#ifndef REVOLVEBRAIN_BRAIN_LEARNER_RLPOWERLEARNER_H_
#define REVOLVEBRAIN_BRAIN_LEARNER_RLPOWERLEARNER_H_

#include "Learner.h"

#include <cmath>
#include <string>
#include <vector>

#include <boost/thread/mutex.hpp>


namespace revolve {
namespace brain {
typedef std::vector<double> Spline;

typedef std::vector<Spline> Policy;

typedef std::shared_ptr<Policy> PolicyPtr;

class RLPowerLearner
        : public Learner<PolicyPtr>
{

public:
    struct Config;

    /**
     * The RLPower constructor reads out configuration file, deretmines which algorithm type to apply and
     * initialises new policy.
     * @param modelName: name of a robot
     * @param brain: configuration file
     * @param evaluator: pointer to fitness evaluatior
     * @param n_actuators: number of actuators
     * @param n_sensors: number of sensors
     * @return pointer to the RLPower class object
     */
    RLPowerLearner(std::string modelName,
                   Config brain,
                   size_t n_weight_vectors);

    virtual ~RLPowerLearner();

    static const size_t MAX_EVALUATIONS;// = 1000; // max number of evaluations
    static const size_t MAX_RANKED_POLICIES;// = 10; // max length of policies vector
    static const size_t INTERPOLATION_CACHE_SIZE;// = 100; // number of data points for the interpolation cache
    static const size_t INITIAL_SPLINE_SIZE;// = 3; // number of initially sampled spline points
    static const size_t UPDATE_STEP;// = 100; // after # generations, it increases the number of spline points
    static const double EVALUATION_RATE;// = 30.0; // evaluation time for each policy
    static const double SIGMA_START_VALUE;// = 0.8; // starting value for sigma
    static const double SIGMA_TAU_CORRECTION;// = 0.2;

    static const double CYCLE_LENGTH;// = 5; // seconds
    static const double SIGMA_DECAY_SQUARED;// = 0.98; // sigma decay

    struct Config
    {
        std::string algorithm_type;
        size_t interpolation_spline_size;
        double evaluation_rate;
        size_t max_evaluations;
        size_t max_ranked_policies;
        double noise_sigma;
        double sigma_tau_correction;
        size_t source_y_size;
        size_t update_step;
        std::string policy_load_path;
    };

protected:
    /**
     * Ranked list of used splines
     */
    class PolicySave
    {
    public:
        PolicyPtr policy_;
        double fitness_;

        PolicySave(double fitness,
                   PolicyPtr &p) :
                policy_(p), fitness_(fitness)
        {}

        bool
        operator>(const PolicySave &ps) const
        {
          return this->fitness_ > ps.fitness_;
        }
    };


private:
    /**
     * Generate new policy
     */
    void generateInitPolicy();

    virtual void reportFitness(std::string id,
                               PolicyPtr genotype,
                               double fitness);

    virtual PolicyPtr currentGenotype();

    /**
     * Load saved policy from JSON file
     */
    void loadPolicy(std::string const policy_path);

    /**
         * Generate interpolated spline based on number of sampled control points in 'source_y'
         * @param source_y: set of control points over which interpolation is generated
         * @param destination_y: set of interpolated control points (default 100 points)
         */
    void
    interpolateCubic(Policy *const source_y,
                     Policy *destination_y);

    /**
         * Writes current spline to file
         */
    void
    writeCurrent();

    /**
     * Writes best 10 splines to file
     */
    void
    writeElite();

    /**
     * Increment number of sampling points for policy
     */
    void
    increaseSplinePoints();

    /**
     * Randomly select two policies and return the one with higher fitness
     * @return an iterator from 'ranked_policies_' map
     */
    std::map<double, PolicyPtr>::iterator
    binarySelection();


    PolicyPtr current_policy_ = NULL; // Pointer to the current policy

    size_t interpolation_spline_size_; // Number of 'interpolation_cache_' sample points
    size_t generation_counter_; // Number of current generation
    size_t max_ranked_policies_; // Maximal number of stored ranked policies
    size_t max_evaluations_; // Maximal number of evaluations
    size_t n_weight_vectors_; // Number of actuators
    size_t source_y_size_; //
    size_t step_rate_; //
    size_t update_step_; // Number of evaluations after which sampling size increases

    double noise_sigma_; // Noise in generatePolicy() function
    double sigma_tau_correction_; // Tau deviation for self-adaptive sigma

    std::string robot_name_; // Name of the robot
    std::string algorithm_type_; // Type of the used algorithm
    std::string policy_load_path_; // Load path for previously saved policies
    std::map<double, PolicyPtr, std::greater<double>> ranked_policies_; // Container for best ranked policies
};

}
}

#endif // REVOLVEBRAIN_BRAIN_LEARNER_RLPOWERLEARNER_H_
