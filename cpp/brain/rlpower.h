//
// Created by Milan Jelisavcic on 28/03/16.
//

#ifndef REVOLVE_GAZEBO_BRAIN_REINFORCEDLEARNING_H
#define REVOLVE_GAZEBO_BRAIN_REINFORCEDLEARNING_H

#include "brain.h"

#include <cmath>
#include <boost/thread/mutex.hpp>

typedef std::vector<double> Spline;
typedef std::vector<Spline*> Policy;

class RLPower : public Brain {
public:
    RLPower() { }

    RLPower(std::string modelName, std::vector< ActuatorPtr >& actuators, std::vector< SensorPtr >& sensors);

    virtual ~RLPower();

    /**
     * @param Motor list
     * @param Sensor list
     */
    virtual void update(const std::vector<ActuatorPtr> &actuators, const std::vector<SensorPtr> &sensors,
                        double t, double step);

protected:

    double getFitness();
    void generatePolicy();

    // Mutex for stepping / updating the network
    boost::mutex networkMutex_;

    /**
     * Holder of current policy
     */
    Policy current_policy_;

    /**
     * Ranked list of used splines
     */
    std::map<double, Policy, std::greater<double>> ranked_policies_;

    unsigned int nActuators_;
    unsigned int nSensors_;

    double start_eval_time_;

    unsigned int generation_counter_;
    //ignition::math::Pose3d currentPosition_;
    //xignition::math::Pose3d previousPosition_;

    /**
     * Rank and matrix of currently used splines.
     * Same as @currentSpline_
     */
    unsigned int source_y_size;
    unsigned int step_rate_;

    /**
     * Noise in the generatePolicy function
     */
    double noise_sigma_;

private:
    void interpolateCubic(Policy& source_y, Policy& destination_y);
    void generateCache();

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
    void writeLast();

    /**
     * Extracts the value of the current_policy in x=time using linear
     * interpolation
     *
     * Writes the output in output_vector
     */
    void generateOutput(const double time, double *output_vector);

    Policy interpolation_cache_;

    const int MAX_EVALUATIONS = 2500; // max number of evaluations
    const int MAX_RANKED_POLICIES = 10; // max length of policies vector
    const int MAX_SPLINE_SAMPLES = 100; // interpolation cache size
    const int UPDATE_STEP = 100; // after # generations, it increases the number of spline points
    const int FREQUENCY_RATE = 30; // seconds
    const double CICLE_LENGTH = 5; // seconds
    const double SIGMA_DECAY_SQUARED =  0.98; // sigma decay
    const int INTERPOLATION_CACHE_SIZE = 100; // number of data points for the interpolation cache
    double cycle_start_time_;
};

#endif //REVOLVE_GAZEBO_BRAIN_REINFORCEDLEARNING_H
