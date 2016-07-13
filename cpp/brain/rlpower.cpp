//
// Created by Milan Jelisavcic on 28/03/16.
//

#include "rlpower.h"
#include "sensor.h"
#include "actuator.h"

#include <random>
#include <iostream>
#include <fstream>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>


RLPower::RLPower(std::string modelName, std::vector< ActuatorPtr >& actuators, std::vector< SensorPtr >& sensors) :
        nActuators_(actuators.size()),
        nSensors_(sensors.size()),
        source_y_size(3),
        start_eval_time_(0),
        generation_counter_(0),
        noise_sigma_(0.008),
        cycle_start_time_(-1),
        current_policy_(nActuators_, 0) {

    step_rate_ = RLPower::MAX_SPLINE_SAMPLES / source_y_size;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::normal_distribution<double> dist(-1, 1);

    // Init first random controller
    for (unsigned int i = 0; i < nActuators_; i++) {
        Spline *spline = new Spline(source_y_size);
        for (int j = 0; j < source_y_size; j++) {
            spline->at(j) = dist(mt);
        }
        current_policy_[i] = spline;
    }

    // Init of empty cache
    for (unsigned int i = 0; i < nActuators_; i++) {
        interpolation_cache_[i] = new Spline(INTERPOLATION_CACHE_SIZE);
    }
}

RLPower::~RLPower() {
    // Delete ranking policy
    for (auto it = ranked_policies_.begin(); it != ranked_policies_.end(); it++) {
        // first → fitness
        // second → policy
        for (int i=0; i<it->second.size(); i++) {
            delete it->second[i];
            it->second[i] = nullptr;
        }
    }

    // Current policy
    for (unsigned int i = 0; i < nActuators_; i++) {
        delete current_policy_[i];
        current_policy_[i] = nullptr;
    }

    // Cache
    for (unsigned int i = 0; i < nActuators_; i++) {
        delete interpolation_cache_[i];
        interpolation_cache_[i] = nullptr;
    }
}

double RLPower::getFitness()
{
        //TODO Calculate fitness for current policy
        /*double dS = sqrt(
                pow(previousPosition_.Pos().X() - currentPosition_.Pos().X(), 2) +
                pow(previousPosition_.Pos().Y() - currentPosition_.Pos().Y(), 2));
        previousPosition_ = currentPosition_;
        return dS / start_eval_time_;
        */
        return 0;
}

void RLPower::generatePolicy()
{
    // Calculate fitness for current policy
    double curr_fitness = this->getFitness();

    // Insert ranked policy in list
    std::cout << "Generation " << generation_counter_ << " fitness " << curr_fitness << std::endl;
    this->writeCurrent(curr_fitness);
    ranked_policies_.insert({curr_fitness, current_policy_});

    // Remove worst policies
    while (ranked_policies_.size() > RLPower::MAX_RANKED_POLICIES) {
        auto last = std::prev(ranked_policies_.end());
        for (int i=0; i<last->second.size(); i++) {
            delete last->second[i];
        }
        ranked_policies_.erase(last);
    }

    if (generation_counter_ == (RLPower::MAX_EVALUATIONS - 1))
        this->writeLast();

    // Set variables for new policy
    generation_counter_++;


    // increase spline points if it is time
    if (generation_counter_ % RLPower::UPDATE_STEP == 0) {
        source_y_size++;

        // current policy
        Policy policy_copy(current_policy_.size());
        for (int i=0; i<source_y_size; i++) {
            policy_copy[i] = new Spline(current_policy_[i]->begin(), current_policy_[i]->end());
        }

        current_policy_.resize(source_y_size);
        interpolateCubic(policy_copy, current_policy_);


        //for every ranked policy
        for(int i=0; i < ranked_policies_.size(); i++) {

            for (int j=0; j<source_y_size; j++) {
                policy_copy[j] = new Spline(ranked_policies_[i][j]->begin(), ranked_policies_[i][j]->end());
            }

            ranked_policies_[i].resize(source_y_size);
            interpolateCubic(policy_copy, ranked_policies_[i]);


            for (int j=0; j<source_y_size; j++) {
                delete policy_copy[j];
            }

        }

        // LOG code
        step_rate_ = RLPower::MAX_SPLINE_SAMPLES / source_y_size;
        std::cout << "New samplingSize_=" << source_y_size << ", and stepRate_=" << step_rate_ << std::endl;
        if (generation_counter_ == RLPower::MAX_EVALUATIONS - 1)
            std::cout << "Finish!!!" << std::endl;

    }


    // Actual policy generation
    std::random_device rd;
    std::mt19937 mt(rd());
    std::normal_distribution<double> dist(0, noise_sigma_);
    noise_sigma_ *= SIGMA_DECAY_SQUARED;

    double total_fitness = 0;
    for (auto it = ranked_policies_.begin(); it != ranked_policies_.end(); it++) {
        // first is fitness
        total_fitness += it->first;
    }

    // for actuator
    for (unsigned int i = 0; i < nActuators_; i++) {
        Spline *spline = new Spline(source_y_size);
        // for column
        for (unsigned int j = 0; j < source_y_size; j++) {

            // modifier ...
            double spline_point = 0;
            for (auto it = ranked_policies_.begin(); it != ranked_policies_.end(); it++) {
                // first → fitness
                // second → policy
                spline_point += ((it->second[i]->at(j) - current_policy_[i]->at(j)) * it->first);
            }
            spline_point /= total_fitness;

            // ... + noise + current
            spline_point += dist(mt) + current_policy_[i]->at(j);

            spline->at(j) = spline_point;
        }
        current_policy_[i] = spline;
    }

    // cache update
    this->generateCache();
}

void RLPower::generateCache()
{
    this->interpolateCubic(current_policy_, interpolation_cache_);
}

void RLPower::generateOutput(const double time, double* output_vector)
{
    if (cycle_start_time_ < 0) {
        cycle_start_time_ = time;
    }

    // get correct X value (between 0 and CICLE_LENGTH)
    double x = time - cycle_start_time_;
    while (x >= RLPower::CICLE_LENGTH) {
        cycle_start_time_ += RLPower::CICLE_LENGTH;
        x = time - cycle_start_time_;
    }

    // adjust X on the cache coordinate space
    x = (x/CICLE_LENGTH) * INTERPOLATION_CACHE_SIZE;
    // generate previous and next values
    int x_a = ((int)x) % INTERPOLATION_CACHE_SIZE;
    int x_b = (x_a+1) % INTERPOLATION_CACHE_SIZE;

    // linear interpolation for every actuator
    for (int i=0; i<nActuators_; i++) {
        double y_a = interpolation_cache_[i]->at(x_a);
        double y_b = interpolation_cache_[i]->at(x_b);

        output_vector[i] = y_a +
            ((y_b - y_a) * (x - x_a) / (x_b - x_a));
    }
}


void RLPower::update(const std::vector< ActuatorPtr >& actuators, const std::vector< SensorPtr >& sensors, double t, double step) {
    boost::mutex::scoped_lock lock(networkMutex_);

    // Evaluate policy on certain time limit
    if ((t-start_eval_time_) > RLPower::FREQUENCY_RATE && generation_counter_ < RLPower::MAX_EVALUATIONS) {
        this->generatePolicy();
        start_eval_time_ = t;
    }

    // generate outputs
    double output_vector[nActuators_];
    this->generateOutput(t, output_vector);

    // Send new signals to the actuators
    unsigned int p = 0;
    for (auto actuator: actuators) {
        actuator->update(&output_vector[p], step);
        p += actuator->outputs();
    }
}

void RLPower::interpolateCubic(Policy &source_y, Policy &destination_y)
{
    const unsigned int source_y_size = source_y.size();
    const unsigned int destination_y_size = destination_y.size();

    const unsigned int N = source_y_size;
    double *x = new double[N+1];
    double *y = new double[N+1];
    double *x_new = new double[destination_y_size];

    gsl_interp_accel *acc = gsl_interp_accel_alloc ();
    const gsl_interp_type *t = gsl_interp_cspline_periodic;
    gsl_spline *spline = gsl_spline_alloc (t, N);

    // init x
    double step_size = CICLE_LENGTH / source_y_size;
    double x_ = 0;
    for (int i = 0; i < N+1; i++) {
        x[i] = x_;
        x_ += step_size;
    }

    // init x_new
    step_size = CICLE_LENGTH / destination_y_size;
    x_ = 0;
    for (int i = 0; i <= destination_y_size; i++) {
        x_new[i] = x_;
        x_ += step_size;
    }


    for (int j = 0; j< source_y.size(); j++) {
        Spline *source_y_line = source_y[j];
        Spline *destination_y_line = destination_y[j];

        // init y
        // TODO use memcpy
        for (int i = 0; i < N; i++) {
            y[i] = source_y_line->at(i);
        }

        // make last equal to first
        y[N] = y[0];

        gsl_spline_init(spline, x, y, N);

        for (int i = 0; i <= destination_y_size; i++) {
            destination_y_line->at(i) = gsl_spline_eval (spline, x_new[i], acc);
        }
    }

    gsl_spline_free (spline);
    gsl_interp_accel_free (acc);

    delete x_new;
    delete y;
    delete x;
}


void RLPower::printCurrent() {
    for (unsigned int i = 0; i < RLPower::MAX_SPLINE_SAMPLES; i++) {
        for (unsigned int j = 0; j < nActuators_; j++) {
            std::cout << current_policy_[j]->at(i) << " ";
        }
        std::cout << std::endl;
    }
}

void RLPower::writeCurrent(double current_fitness) {
    std::ofstream outputFile;
    std::string uri = "~/output/spider_";
    outputFile.open(uri + ".csv", std::ios::app | std::ios::out | std::ios::ate);
    // std::to_string(currEval_) -------^
    // outputFile << "id,fitness,steps,policy" << std::endl;
    outputFile << generation_counter_ << "," << current_fitness << "," << step_rate_ << ",";
    for (unsigned int i = 0; i < RLPower::MAX_SPLINE_SAMPLES; i++) {
        for (unsigned int j = 0; j < nActuators_; j = j + step_rate_) {
            outputFile << current_policy_[j]->at(i) << ":";
        }
    }
    outputFile << std::endl;
    outputFile.close();
}

void RLPower::writeLast() {
    std::ofstream outputFile;
    std::string uri = "~/output/spider_last_";
    outputFile.open(uri + ".csv", std::ios::app);
    // std::to_string(currEval_) -------^
    outputFile << "id,fitness,steps,policy" << std::endl;
    for (auto iterator = ranked_policies_.begin(); iterator != ranked_policies_.end(); iterator++) {
        outputFile << iterator->first << "," << step_rate_ << ",";
        for (unsigned int i = 0; i < RLPower::MAX_SPLINE_SAMPLES; i++) {
            for (unsigned int j = 0; j < nActuators_; j++) {
                outputFile << iterator->second[j]->at(i) << ":";
            }
        }
        outputFile << std::endl;
    }
    outputFile << std::endl;
    outputFile.close();
}
