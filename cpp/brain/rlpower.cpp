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
        init_(true),
        source_y_size(5),
        start_eval_time_(0),
        generation_counter_(0),
        noise_sigma_(0.008),
        current_policy_(nActuators_, 0) {

    step_rate_ = RLPower::MAX_SPLINE_SAMPLES / source_y_size;

    //currentPosition_.Reset();
    //previousPosition_.Reset();
    std::random_device rd;
    std::mt19937 mt(rd());
    std::normal_distribution<double> dist(-1, 1);
    for (unsigned int i = 0; i < nActuators_; i++) {
        Spline *spline = new Spline(source_y_size);
        for (int j = 0; j < source_y_size; j++) {
            spline->at(j) = dist(mt);
        }
        current_policy_[i] = spline;
    }
}

RLPower::~RLPower() { }

double RLPower::getFitness()
{
        // Calculate fitness for current policy
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
    curr_fitness_ = this->getFitness();

    // Insert ranked policy in list
    std::cout << "Generation " << generation_counter_ << " fitness " << curr_fitness_ << std::endl;
    this->writeCurrent();
    ranked_policies_.insert({curr_fitness_, current_policy_});

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
    init_ = true;
    generation_counter_++;


    // Update steps if it is time
    if (generation_counter_ % RLPower::UPDATE_STEP == 0) {
        Policy tmp(current_policy_.size());
        for (int i=0; i<source_y_size; i++) {
            tmp[i] = new Spline(current_policy_[i]->begin(), current_policy_[i]->end());
        }

        source_y_size++;
        current_policy_.resize(source_y_size);
        interpolateCubic(tmp, current_policy_);

        for (int i=0; i<source_y_size; i++) {
            delete tmp[i];
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

    double total = 0;
    for (auto it = ranked_policies_.begin(); it != ranked_policies_.end(); it++) {
        // first is fitness
        total += it->first;
    }

    // for actuator
    for (unsigned int i = 0; i < nActuators_; i++) {
        Spline *spline = new Spline(source_y_size);
        // for column
        for (unsigned int j = 0; j < source_y_size; j++) {

            // modifier ...
            for (auto it = ranked_policies_.begin(); it != ranked_policies_.end(); it++) {
                // first → fitness
                // second → policy
                auto tmp = ((it->second[i]->at(j) - current_policy_[i]->at(j)) * it->first);
                spline->at(j) = tmp;
            }
            spline->at(j) /= total;

            // ... + noise + current
            spline->at(j) += dist(mt) + current_policy_[i]->at(j);
        }
        current_policy_[i] = spline;
    }
}


void RLPower::update(const std::vector< ActuatorPtr >& actuators, const std::vector< SensorPtr >& sensors, double t, double step) {
    boost::mutex::scoped_lock lock(networkMutex_);


    // Evaluate policy on certain time limit
    if ((t-start_eval_time_) > RLPower::FREQUENCY_RATE && generation_counter_ < RLPower::MAX_EVALUATIONS) {
        this->generatePolicy();
        start_eval_time_ = t;
    }

    // Initialise new policy
    if (init_) {
        //this->initPolicy(step_rate_);
        init_ = false;
    }

    this->step(t);
    double output_vector[nActuators_];

    // Send new signals to the actuators
    unsigned int p = 0;
    for (auto actuator: actuators) {
        actuator->update(&output_vector[p], step);
        //actuator->update(&currentPolicy_[p][stepIndex_], step);
        p += actuator->outputs();
    }
}

void RLPower::step(double time) {
}

void RLPower::initPolicy(const int steps) {
    // Generate new array of random splines
    std::random_device rd;
    std::mt19937 mt(rd());

    if (step_rate_ <= 0) step_rate_ = 1;


    if (ranked_policies_.size() == 0) {
        std::uniform_real_distribution<double> dist(-1, 1);
        for (unsigned int i = 0; i < nActuators_; i++) {
            Spline *spline = new Spline(RLPower::MAX_SPLINE_SAMPLES, 0);
            for (unsigned int j = 0; j < RLPower::MAX_SPLINE_SAMPLES; j = j + steps) {
                spline->at(j) = dist(mt);
            }
            current_policy_[i] = spline;
        }
    } else {
        double total = 0;
        for (auto it = ranked_policies_.begin(); it != ranked_policies_.end(); it++) {
            total += it->first;
        }
        std::uniform_real_distribution<double> dist(-0.1, 0.1);
        for (unsigned int i = 0; i < nActuators_; i++) {
            Spline *spline = new Spline(RLPower::MAX_SPLINE_SAMPLES, 0);
            for (unsigned int j = 0; j < RLPower::MAX_SPLINE_SAMPLES; j = j + steps) {
                spline->at(j) = dist(mt) + current_policy_.at(i)->at(j);
                for (auto it = ranked_policies_.begin(); it != ranked_policies_.end(); it++) {
                    spline->at(j) += ((it->second.at(i)->at(j) - current_policy_.at(i)->at(j)) * it->first) / total;
                }
            }
            current_policy_[i] = spline;
        }
    }

    this->interpolate(steps);
}

void RLPower::interpolate(const int steps) {
    for (unsigned int i = 0; i < nActuators_; i++) {
        unsigned int startingIndex, beginIndex, endIndex;
        double startingValue, beginValue = 0, endValue = 0, stepRate;
        for (unsigned int j = 0; j < RLPower::MAX_SPLINE_SAMPLES; j++) {
            if (current_policy_[i]->at(j) != 0 && beginValue == 0) {
                beginValue = current_policy_[i]->at(j);
                beginIndex = j;
                startingValue = beginValue;
                startingIndex = beginIndex;
            }
            if (current_policy_[i]->at(j) != 0 && beginValue != 0) {
                endValue = current_policy_[i]->at(j);
                endIndex = j;
            }

            if (beginValue != 0 && endValue != 0) {
                // Interpolate
                stepRate = (endValue - beginValue) / steps;

                for (unsigned int k = beginIndex + 1; k < endIndex; k++) {
                    current_policy_[i]->at(k) = current_policy_[i]->at(k - 1) + stepRate;
                }

                // Reset check for next value
                beginValue = endValue;
                beginIndex = endIndex;
                endValue = 0;
            }
        }

        // Interpolate end and start of spline
        stepRate = (startingValue - beginValue) / steps;
        for (unsigned int k = beginIndex + 1;
                k < (RLPower::MAX_SPLINE_SAMPLES * (i + 1)) + (startingIndex - (RLPower::MAX_SPLINE_SAMPLES * i)); k++) {
            current_policy_[i]->at(k % (RLPower::MAX_SPLINE_SAMPLES * (i + 1))) =
                    current_policy_[i]->at((k - 1) % (RLPower::MAX_SPLINE_SAMPLES * (i + 1))) + stepRate;
        }
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

void RLPower::writeCurrent() {
    std::ofstream outputFile;
    std::string uri = "~/output/spider_";
    outputFile.open(uri + ".csv", std::ios::app | std::ios::out | std::ios::ate);
    // std::to_string(currEval_) -------^
    // outputFile << "id,fitness,steps,policy" << std::endl;
    outputFile << generation_counter_ << "," << curr_fitness_ << "," << step_rate_ << ",";
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
