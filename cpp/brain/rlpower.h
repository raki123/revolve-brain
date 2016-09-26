//
// Created by Milan Jelisavcic on 28/03/16.
//

#ifndef REVOLVE_BRAIN_RLPOWER_H
#define REVOLVE_BRAIN_RLPOWER_H

#include "brain.h"
#include "evaluator.h"

#include <cmath>
#include <boost/thread/mutex.hpp>


namespace revolve {
namespace brain {

    typedef std::vector<double> Spline;
    typedef std::vector<Spline *> Policy;

    class RLPower : public revolve::brain::Brain {

    public:
        typedef std::vector<double> Spline;
        typedef std::vector<Spline> Policy;
        typedef std::shared_ptr<Policy> PolicyPtr;

        /**
         * The RLPower constructor reads out configuration file, deretmines which algorithm type to apply and
         * initialises new policy.
         * @param evaluator: pointer to the fitness evaluatior
         * @param n_actuators: number of actuators
         * @param n_sensors: number of sensors
         * @return pointer to the RLPower class object
         */
        RLPower(EvaluatorPtr evaluator,
                unsigned int n_actuators,
                unsigned int n_sensors);

        virtual ~RLPower();

        /**
         * Method for updating sensors readings, actuators positions, ranked list of policies and generating new policy
         * @param actuators: vector list of robot's actuators
         * @param sensors: vector list of robot's sensors
         * @param t:
         * @param step:
         */
        virtual void update(const std::vector<ActuatorPtr> &actuators,
                            const std::vector<SensorPtr> &sensors,
                            double t,
                            double step);

    protected:
//        /**
//         * Request handler to modify the neural network
//         */
//        void modify(ConstModifyNeuralNetworkPtr &req);
//
//        boost::mutex networkMutex_; // Mutex for stepping / updating the network

        /**
         * Ranked list of used splines
         */
        class PolicySave {
        public:
            PolicyPtr policy_;
            double fitness_;

            PolicySave(double fitness, PolicyPtr &p) :
                    policy_(p),
                    fitness_(fitness) {}

            bool operator>(const PolicySave &ps) const {
                return this->fitness_ > ps.fitness_;
            }
        };

        const unsigned int MAX_EVALUATIONS = 1000; // max number of evaluations
        const unsigned int MAX_RANKED_POLICIES = 10; // max length of policies vector
        const unsigned int INTERPOLATION_CACHE_SIZE = 100; // number of data points for the interpolation cache
        const unsigned int MAX_SPLINE_SAMPLES = 100; // interpolation cache size
        const unsigned int INITIAL_SPLINE_SIZE = 3; // number of initially sampled spline points
        const double SIGMA_START_VALUE = 0.8; // starting value for sigma

        const unsigned int UPDATE_STEP = 100; // after # generations, it increases the number of spline points
        const unsigned int FREQUENCY_RATE = 30; // seconds
        const double CYCLE_LENGTH = 5; // seconds
        const double SIGMA_DECAY_SQUARED = 0.98; // sigma decay


    protected:
        template<typename ActuatorContainer, typename SensorContainer>
        void update(const ActuatorContainer &actuators,
                    const SensorContainer &sensors,
                    double t,
                    double step) {
            //boost::mutex::scoped_lock lock(networkMutex_);

            // Evaluate policy on certain time limit
            if ((t - start_eval_time_) > RLPower::FREQUENCY_RATE &&
                generation_counter_ < RLPower::MAX_EVALUATIONS) {
                this->generatePolicy();
                start_eval_time_ = t;
                evaluator_->start();
            }

            // generate outputs
            double *output_vector = new double[nActuators_];
            this->generateOutput(t, output_vector);

            // Send new signals to the actuators
            unsigned int p = 0;
            for (auto actuator: actuators) {
                actuator->update(output_vector + p, step);
                p += actuator->outputs();
            }

            delete[] output_vector;
        }

    private:
//        /**
//         * Transport node
//         */
//        ::gazebo::transport::NodePtr node_;
//
//        /**
//         * Network modification subscriber
//         */
//        ::gazebo::transport::SubscriberPtr alterSub_;

        /**
         * Generate new policy
         */
        void generatePolicy();

        /**
         * Generate cache policy
         */
        void generateCache();

        /**
         * Generate interpolated spline based on number of sampled control points in 'source_y'
         * @param source_y: set of control points over which interpolation is generated
         * @param destination_y: set of interpolated control points (default 100 points)
         */
        void interpolateCubic(Policy *const source_y,
                              Policy *destination_y);

        /**
         * Extracts the value of the current_policy in x=time using linear
         * interpolation
         *
         * Writes the output in output_vector
         */
        void generateOutput(const double time,
                            double *output_vector);

        /**
         * Retrieves fitness for the current policy
         * @return
         */
        double getFitness();

        /**
         * Writes all current splines to file
         */
        void printCurrent();

        /**
         * Writes current spline to file
         */
        void writeCurrent(double current_fitness);

        /**
         * Writes best 10 splines to file
         */
        void writeLast(double fitness);


        PolicyPtr current_policy_; // Pointer to the current policy
        PolicyPtr interpolation_cache_; // Pointer to the interpolated current_policy_ (default 100 points)
        EvaluatorPtr evaluator_; // Pointer to the fitness evaluator

        unsigned int nActuators_; // Number of actuators
        unsigned int nSensors_; // Number of sensors
        unsigned int generation_counter_; // Number of current generation
        unsigned int source_y_size; //
        unsigned int step_rate_; //
        unsigned int intepolation_spline_size_; // Number of 'interpolation_cache_' sample points
        unsigned int max_ranked_policies_; // Maximal number of stored ranked policies
        unsigned int max_evaluations_; // Maximal number of evaluations

        double cycle_start_time_;
        double start_eval_time_;
        double noise_sigma_; // Noise in the generatePolicy function

        std::string robot_name_; // Name of the robot
        std::string algorithm_type_; // Type of the used algorithm
        std::map<double, PolicyPtr, std::greater<double>> ranked_policies_; // Container for best ranked policies
    };

}
}

#endif //REVOLVE_BRAIN_RLPOWER_H
