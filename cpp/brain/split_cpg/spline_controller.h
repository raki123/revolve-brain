//
// Created by Milan Jelisavcic on 28/03/16.
//

#ifndef REVOLVE_BRAIN_SPLINE_CONTROLLER_H
#define REVOLVE_BRAIN_SPLINE_CONTROLLER_H

#include "controller.h"

#include <cmath>
#include <string>


namespace revolve {
    namespace brain {

//    typedef std::vector<double> Spline;
//    typedef std::vector<Spline *> Policy;

        class SplineController : public revolve::brain::Controller<std::vector<std::vector<double>>> {
	protected:
	    struct Config;
        public:
            typedef std::vector<double> Spline;
            typedef std::vector<Spline> Policy;
            typedef std::shared_ptr<Policy> PolicyPtr;
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
            SplineController(std::string modelName,
                    Config brain,
                    EvaluatorPtr evaluator,
                    unsigned int n_actuators,
                    unsigned int n_sensors);

            virtual ~SplineController();

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
	    /**
	    * Get method for the genome
	    * @return current genome
	    */
	    virtual G getGenome();
	    
	    /**
	    * Set method for the genome
	    * @param newGenome: new genome to use instead of the old one
	    */
	    virtual void setGenome(G newGenome);
      
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

            static const unsigned int MAX_EVALUATIONS;// = 1000; // max number of evaluations
            static const unsigned int MAX_RANKED_POLICIES;// = 10; // max length of policies vector
            static const unsigned int INTERPOLATION_CACHE_SIZE;// = 100; // number of data points for the interpolation cache
            static const unsigned int INITIAL_SPLINE_SIZE;// = 3; // number of initially sampled spline points
            static const unsigned int UPDATE_STEP;// = 100; // after # generations, it increases the number of spline points
            static const double EVALUATION_RATE;// = 30.0; // evaluation time for each policy
            static const double SIGMA_START_VALUE;// = 0.8; // starting value for sigma
            static const double SIGMA_TAU_CORRECTION;// = 0.2;

            static const double CYCLE_LENGTH;// = 5; // seconds
            static const double SIGMA_DECAY_SQUARED;// = 0.98; // sigma decay

            template<typename ActuatorContainer, typename SensorContainer>
            void update(const ActuatorContainer &actuators,
                        const SensorContainer &sensors,
                        double t,
                        double step) {

                // generate outputs
                double *output_vector = new double[n_actuators_];
                this->generateOutput(t, output_vector);

                // Send new signals to the actuators
                unsigned int p = 0;
                for (auto actuator: actuators) {
                    actuator->update(&output_vector[p], step);
                    p += actuator->outputs();
                }

                delete[] output_vector;
            }

            struct Config {
                std::string algorithm_type;
                unsigned int evaluation_rate;
                unsigned int interpolation_spline_size;
                unsigned int max_evaluations;
                unsigned int max_ranked_policies;
                double noise_sigma;
                double sigma_tau_correction;
                unsigned int source_y_size;
                unsigned int update_step;
                std::string policy_load_path;
            };

        private:

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
             * Increment number of sampling points for policy
             */
            void increaseSplinePoints();

            /**
             * Randomly select two policies and return the one with higher fitness
             * @return an iterator from 'ranked_policies_' map
             */
            std::map<double, RLPower::PolicyPtr>::iterator binarySelection();

            /**
             * Extracts the value of the current_policy in x=time using linear
             * interpolation
             *
             * Writes the output in output_vector
             */
            void generateOutput(const double time,
                                double *output_vector);

            PolicyPtr current_policy_ = NULL; // Pointer to the current policy
            PolicyPtr interpolation_cache_ = NULL; // Pointer to the interpolated current_policy_ (default 100 points)
            EvaluatorPtr evaluator_ = NULL; // Pointer to the fitness evaluator

            unsigned int generation_counter_; // Number of current generation
            unsigned int interpolation_spline_size_; // Number of 'interpolation_cache_' sample points
            unsigned int max_ranked_policies_; // Maximal number of stored ranked policies
            unsigned int max_evaluations_; // Maximal number of evaluations
            unsigned int n_actuators_; // Number of actuators
            unsigned int n_sensors_; // Number of sensors
            unsigned int source_y_size_; //
            unsigned int step_rate_; //
            unsigned int update_step_; // Number of evaluations after which sampling size increases

            double cycle_start_time_;
            double evaluation_rate_;
            double noise_sigma_; // Noise in generatePolicy() function
            double sigma_tau_correction_; // Tau deviation for self-adaptive sigma
            double start_eval_time_;

            std::string robot_name_; // Name of the robot
            std::string algorithm_type_; // Type of the used algorithm
            std::string policy_load_path_; // Load path for previously saved policies
            std::map<double, PolicyPtr, std::greater<double>> ranked_policies_; // Container for best ranked policies
        };

    }
}

#endif //REVOLVE_BRAIN_SPLINE_CONTROLLER_H