//
// Created by Milan Jelisavcic on 28/03/16.
//

#ifndef REVOLVEBRAIN_BRAIN_RLPOWER_H_
#define REVOLVEBRAIN_BRAIN_RLPOWER_H_

#include <cmath>
#include <string>

#include <boost/thread/mutex.hpp>

#include "Brain.h"
#include "Evaluator.h"

namespace revolve
{
  namespace brain
  {

//    typedef std::vector<double> Spline;
//    typedef std::vector<Spline *> Policy;

    class RLPower
            : public revolve::brain::Brain
    {

      protected:
      struct Config;
      public:
      typedef std::vector< double > Spline;
      typedef std::vector< Spline > Policy;
      typedef std::shared_ptr< Policy > PolicyPtr;

//        typedef const std::shared_ptr<revolve::msgs::ModifyNeuralNetwork const> ConstModifyNeuralNetworkPtr;

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
      RLPower(
              std::string modelName,
              Config brain,
              EvaluatorPtr evaluator,
              size_t n_actuators,
              size_t n_sensors);

      virtual ~RLPower() override;

      using revolve::brain::Brain::update;

      /**
       * Method for updating sensors readings, actuators positions, ranked list of policies and generating new policy
       * @param actuators: vector list of robot's actuators
       * @param sensors: vector list of robot's sensors
       * @param t:
       * @param step:
       */
      virtual void update(
              const std::vector< ActuatorPtr > &actuators,
              const std::vector< SensorPtr > &sensors,
              double t,
              double step) override;

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
      class PolicySave
      {
        public:
        PolicyPtr policy_;
        double fitness_;

        PolicySave(
                double fitness,
                PolicyPtr &p)
                :
                policy_(p), fitness_(fitness)
        {}

        bool
        operator>(const PolicySave &ps) const
        {
          return this->fitness_ > ps.fitness_;
        }
      };

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

      template < typename ActuatorContainer, typename SensorContainer >
      void update(
              const ActuatorContainer &actuators,
              const SensorContainer &sensors,
              double t,
              double step)
      {
        //        boost::mutex::scoped_lock lock(networkMutex_);
        if (policy_load_path_ == "")
        {
          if (start_eval_time_ < 0)
          {
            start_eval_time_ = t;
          }

          // Evaluate policy on certain time limit
          if ((t - start_eval_time_) > evaluation_rate_ && generation_counter_ < max_evaluations_)
          {
            this->updatePolicy();
            start_eval_time_ = t;
            evaluator_->start();
          }
        }

        // generate outputs
        double *output_vector = new double[n_actuators_];
        this->generateOutput(t,
                             output_vector);

        // Send new signals to the actuators
        size_t p = 0;
        for (auto actuator: actuators)
        {
          actuator->update(&output_vector[p],
                           step);
          p += actuator->outputs();
        }

        delete[] output_vector;
      }

      struct Config
      {
        std::string algorithm_type;
        size_t evaluation_rate;
        size_t interpolation_spline_size;
        size_t max_evaluations;
        size_t max_ranked_policies;
        double noise_sigma;
        double sigma_tau_correction;
        size_t source_y_size;
        size_t update_step;
        std::string policy_load_path;
      };

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
      void generateInitPolicy();

      /**
       * Generate cache policy
       */
      void generateCache();

      /**
       * Evaluate the current policy and generate new
       */
      void updatePolicy();

      /**
       * Load saved policy from JSON file
       */
      void loadPolicy(std::string const policy_path);

      /**
       * Generate interpolated spline based on number of sampled control points in 'source_y'
       * @param source_y: set of control points over which interpolation is generated
       * @param destination_y: set of interpolated control points (default 100 points)
       */
      void interpolateCubic(
              Policy *const source_y,
              Policy *destination_y);

      /**
       * Increment number of sampling points for policy
       */
      void increaseSplinePoints();

      /**
       * Randomly select two policies and return the one with higher fitness
       * @return an iterator from 'ranked_policies_' map
       */
      std::map< double, RLPower::PolicyPtr >::iterator binarySelection();

      /**
       * Extracts the value of the current_policy in x=time using linear
       * interpolation
       *
       * Writes the output in output_vector
       */
      void generateOutput(
              const double time,
              double *output_vector);

      /**
       * Retrieves fitness for the current policy
       * @return
       */
      double getFitness();

      /**
       * Writes current spline to file
       */
      void writeCurrent();

      /**
       * Writes best 10 splines to file
       */
      void writeElite();

      PolicyPtr current_policy_ = NULL; // Pointer to the current policy
      PolicyPtr interpolation_cache_ =
              NULL; // Pointer to the interpolated current_policy_ (default 100 points)
      EvaluatorPtr evaluator_ = NULL; // Pointer to the fitness evaluator

      size_t generation_counter_; // Number of current generation
      size_t interpolation_spline_size_; // Number of 'interpolation_cache_' sample points
      size_t max_ranked_policies_; // Maximal number of stored ranked policies
      size_t max_evaluations_; // Maximal number of evaluations
      size_t n_actuators_; // Number of actuators
      size_t n_sensors_; // Number of sensors
      size_t source_y_size_; //
      size_t step_rate_; //
      size_t update_step_; // Number of evaluations after which sampling size increases

      double cycle_start_time_;
      double evaluation_rate_;
      double noise_sigma_; // Noise in generatePolicy() function
      double sigma_tau_correction_; // Tau deviation for self-adaptive sigma
      double start_eval_time_;

      std::string robot_name_; // Name of the robot
      std::string algorithm_type_; // Type of the used algorithm
      std::string policy_load_path_; // Load path for previously saved policies
      std::map< double, PolicyPtr, std::greater< double>>
              ranked_policies_; // Container for best ranked policies
    };

  }
}

#endif // REVOLVEBRAIN_BRAIN_RLPOWER_H_
