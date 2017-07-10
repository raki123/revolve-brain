//
// Created by Milan Jelisavcic on 28/03/16.
//

#include "RLPowerLearner.h"

#include <random>
#include <fstream>

#include <gsl/gsl_spline.h>
#include <yaml-cpp/yaml.h>

using namespace revolve::brain;

RLPowerLearner::RLPowerLearner(std::string modelName,
                               Config brain,
                               size_t n_weight_vectors)
        : generation_counter_(0)
        , n_weight_vectors_(n_weight_vectors)
        , robot_name_(modelName)
{


  // Read out brain configuration attributes
  algorithm_type_ = brain.algorithm_type;
  std::cout << std::endl << "Initialising RLPowerLearner, type " << algorithm_type_ << std::endl << std::endl;
  interpolation_spline_size_ = brain.interpolation_spline_size;
  max_evaluations_ = brain.max_evaluations;
  max_ranked_policies_ = brain.max_ranked_policies;
  noise_sigma_ = brain.noise_sigma;
  sigma_tau_correction_ = brain.sigma_tau_correction;
  source_y_size_ = brain.source_y_size;
  update_step_ = brain.update_step;
  policy_load_path_ = brain.policy_load_path;

  step_rate_ = interpolation_spline_size_ / source_y_size_;

  if (policy_load_path_ == "") {
    // Generate first random policy
    this->generateInitPolicy();
  } else {
    this->loadPolicy(policy_load_path_);
  }
}

RLPowerLearner::~RLPowerLearner()
{
  // `boost::shared_ptr< Policy >` should take care of memory management for us
}


void
RLPowerLearner::generateInitPolicy()
{
  std::random_device rd;
  std::mt19937 mt(rd());
  std::normal_distribution<double> dist(0,
                                        this->noise_sigma_);

  // Init first random controller
  if (!current_policy_)
    current_policy_ = std::make_shared<Policy>(n_weight_vectors_);

  for (size_t i = 0; i < n_weight_vectors_; i++) {
    Spline spline(source_y_size_);
    for (size_t j = 0; j < source_y_size_; j++) {
      spline[j] = dist(mt);
    }
    current_policy_->at(i) = spline;
  }
}

void
RLPowerLearner::loadPolicy(std::string const policy_path)
{
  YAML::Node policy_file = YAML::LoadFile(policy_path);
  if (policy_file.IsNull()) {
    std::cout << "Failed to load the policy file." << std::endl;
    return;
  }

  // Init first random controller
  if (!current_policy_)
    current_policy_ = std::make_shared<Policy>(n_weight_vectors_);

  std::cout << "evaluation: " << policy_file[0]["evaluation"] << std::endl;
  std::cout << "steps: " << policy_file[0]["steps"] << std::endl;
  std::cout << "velocity: " << policy_file[0]["population"][0]["velocity"] << std::endl;

  size_t k = 0;
  source_y_size_ = policy_file[0]["steps"].as<uint>();
  YAML::Node policy = policy_file[0]["population"][0]["policy"];

  if (source_y_size_ * n_weight_vectors_ != policy.size()) {
    std::cout << "Number of (n_spline_points) is not equal to (n_actuators * n_steps)!" << std::endl;
    return;
  }

  for (size_t i = 0; i < n_weight_vectors_; i++) {
    Spline spline(source_y_size_);
    for (size_t j = 0; j < source_y_size_; j++) {
      spline[j] = policy[k++].as<double>();
    }
    current_policy_->at(i) = spline;
  }
}

void RLPowerLearner::reportFitness(std::string id,
                                   PolicyPtr genotype,
                                   double curr_fitness)
{
  // Insert ranked policy in list
  PolicyPtr policy_copy = std::make_shared<Policy>(n_weight_vectors_);
  for (size_t i = 0; i < n_weight_vectors_; i++) {
    Spline &spline = current_policy_->at(i);
    policy_copy->at(i) = Spline(spline.begin(),
                                spline.end());

    spline.resize(source_y_size_);
  }
  ranked_policies_.insert({curr_fitness, policy_copy});

  // Remove worst policies
  while (ranked_policies_.size() > max_ranked_policies_) {
    auto last = std::prev(ranked_policies_.end());
    ranked_policies_.erase(last);
  }

  // Print-out current status to the terminal
  std::cout << robot_name_ << ":" << generation_counter_ << " ranked_policies_:";
  for (auto const &it : ranked_policies_) {
    double fitness = it.first;
    std::cout << " " << fitness;
  }
  std::cout << std::endl;

  // Write fitness and genomes log to output files
//    this->writeCurrent();
  this->writeElite();

  // Update generation counter and check is it finished
  generation_counter_++;
  if (generation_counter_ == max_evaluations_) {
    std::exit(0);
  }

  // Increase spline points if it is a time
  if (update_step_ > 0 && generation_counter_ % update_step_ == 0)
    this->increaseSplinePoints();

  /// Actual policy generation

  /// Determine which mutation operator to use
  /// Default, for algorithms A and B, is used standard normal distribution with decaying sigma
  /// For algorithms C and D, is used normal distribution with self-adaptive sigma
  std::random_device rd;
  std::mt19937 mt(rd());

  if (algorithm_type_ == "C" || algorithm_type_ == "D") {
    // uncorrelated mutation with one step size
    std::mt19937 sigma_mt(rd());
    std::normal_distribution<double> sigma_dist(0,
                                                1);
    noise_sigma_ = noise_sigma_ * std::exp(sigma_tau_correction_ * sigma_dist(sigma_mt));
  } else {
    // Default is decaying sigma
    if (ranked_policies_.size() >= max_ranked_policies_)
      noise_sigma_ *= SIGMA_DECAY_SQUARED;
  }
  std::normal_distribution<double> dist(0,
                                        noise_sigma_);

  /// Determine which selection operator to use
  /// Default, for algorithms A and C, is used ten parent crossover
  /// For algorithms B and D, is used two parent crossover with binary tournament selection
  if (ranked_policies_.size() < max_ranked_policies_) {
    // Generate random policy if number of stored policies is less then 'max_ranked_policies_'
    for (size_t i = 0; i < n_weight_vectors_; i++) {
      for (size_t j = 0; j < source_y_size_; j++) {
        (*current_policy_)[i][j] = dist(mt);
      }
    }
  } else {
    // Generate new policy using weighted crossover operator
    double total_fitness = 0;
    if (algorithm_type_ == "B" || algorithm_type_ == "D") {
      // k-selection tournament
      auto parent1 = binarySelection();
      auto parent2 = parent1;
      while (parent2 == parent1) {
        parent2 = binarySelection();
      }

      double fitness1 = parent1->first;
      double fitness2 = parent2->first;

      PolicyPtr policy1 = parent1->second;
      PolicyPtr policy2 = parent2->second;

      // TODO: Verify what should be total fitness in binary
      total_fitness = fitness1 + fitness2;

      // For each spline
      for (size_t i = 0; i < n_weight_vectors_; i++) {
        // And for each control point
        for (size_t j = 0; j < source_y_size_; j++) {
          // Apply modifier
          double spline_point = 0;
          spline_point += ((policy1->at(i)[j] - (*current_policy_)[i][j])) * (fitness1 / total_fitness);
          spline_point += ((policy2->at(i)[j] - (*current_policy_)[i][j])) * (fitness2 / total_fitness);

          // Add a mutation + current
          // TODO: Verify do we use current in this case
          spline_point += dist(mt) + (*current_policy_)[i][j];

          // Set a newly generated point as current
          (*current_policy_)[i][j] = spline_point;
        }
      }
    } else {
      // Default is all parents selection

      // Calculate first total sum of fitnesses
      for (auto const &it : ranked_policies_) {
        double fitness = it.first;
        total_fitness += fitness;
      }

      // For each spline
      // TODO: Verify that this should is correct formula
      for (size_t i = 0; i < n_weight_vectors_; i++) {
        // And for each control point
        for (size_t j = 0; j < source_y_size_; j++) {

          // Apply modifier
          double spline_point = 0;
          for (auto const &it : ranked_policies_) {
            double fitness = it.first;
            PolicyPtr policy = it.second;

            spline_point += ((policy->at(i)[j] - (*current_policy_)[i][j])) * (fitness / total_fitness);
          }

          // Add a mutation + current
          // TODO: Verify do we use 'current_policy_' in this case
          spline_point += dist(mt) + (*current_policy_)[i][j];

          // Set a newly generated point as current
          (*current_policy_)[i][j] = spline_point;
        }
      }
    }
  }
}

PolicyPtr
RLPowerLearner::currentGenotype()
{
  return current_policy_;
}


void
RLPowerLearner::interpolateCubic(Policy *const source_y,
                                 Policy *destination_y)
{
  const size_t source_y_size_ = (*source_y)[0].size();
  const size_t destination_y_size = (*destination_y)[0].size();

  const size_t N = source_y_size_ + 1;
  double *x = new double[N];
  double *y = new double[N];
  double *x_new = new double[destination_y_size];

  gsl_interp_accel *acc = gsl_interp_accel_alloc();
  const gsl_interp_type *t = gsl_interp_cspline_periodic;
  gsl_spline *spline = gsl_spline_alloc(t,
                                        N);

  // init x
  double step_size = CYCLE_LENGTH / source_y_size_;
  for (size_t i = 0; i < N; i++) {
    x[i] = step_size * i;
  }

  // init x_new
  step_size = CYCLE_LENGTH / destination_y_size;
  for (size_t i = 0; i < destination_y_size; i++) {
    x_new[i] = step_size * i;
  }

  for (size_t j = 0; j < n_weight_vectors_; j++) {
    Spline &source_y_line = source_y->at(j);
    Spline &destination_y_line = destination_y->at(j);

    // init y
    // TODO use memcpy
    for (size_t i = 0; i < source_y_size_; i++) {
      y[i] = source_y_line[i];
    }

    // make last equal to first
    y[N - 1] = y[0];

    gsl_spline_init(spline,
                    x,
                    y,
                    N);

    for (size_t i = 0; i < destination_y_size; i++) {
      destination_y_line[i] = gsl_spline_eval(spline,
                                              x_new[i],
                                              acc);
    }
  }

  gsl_spline_free(spline);
  gsl_interp_accel_free(acc);

  delete[] x_new;
  delete[] y;
  delete[] x;
}

void
RLPowerLearner::increaseSplinePoints()
{
  source_y_size_++;

  // LOG code
  step_rate_ = interpolation_spline_size_ / source_y_size_;
  std::cout << "New samplingSize_=" << source_y_size_ << ", and stepRate_=" << step_rate_ << std::endl;

  // Copy current policy for resizing
  Policy policy_copy(current_policy_->size());
  for (size_t i = 0; i < n_weight_vectors_; i++) {
    Spline &spline = current_policy_->at(i);
    policy_copy[i] = Spline(spline.begin(),
                            spline.end());

    spline.resize(source_y_size_);
  }

  this->interpolateCubic(&policy_copy,
                         current_policy_.get());

  //for every ranked policy
  for (auto &it : ranked_policies_) {
    PolicyPtr policy = it.second;

    for (size_t j = 0; j < n_weight_vectors_; j++) {
      Spline &spline = policy->at(j);
      policy_copy[j] = Spline(spline.begin(),
                              spline.end());
      spline.resize(source_y_size_);
    }
    this->interpolateCubic(&policy_copy,
                           policy.get());
  }
}

std::map<double, PolicyPtr>::iterator
RLPowerLearner::binarySelection()
{
  std::random_device rd;
  std::mt19937 umt(rd());
  std::uniform_int_distribution<size_t> udist(0,
                                                    max_ranked_policies_ - 1);

  // Select two different numbers from uniform distribution U(0, max_ranked_policies_ - 1)
  size_t pindex1, pindex2;
  pindex1 = udist(umt);
  do {
    pindex2 = udist(umt);
  } while (pindex1 == pindex2);

  // Set iterators to begin of the 'ranked_policies_' map
  auto individual1 = ranked_policies_.begin();
  auto individual2 = ranked_policies_.begin();

  // Move iterators to indices positions
  std::advance(individual1,
               pindex1);
  std::advance(individual2,
               pindex2);

  double fitness1 = individual1->first;
  double fitness2 = individual2->first;

  return fitness1 > fitness2 ? individual1 : individual2;
}

void
RLPowerLearner::writeCurrent()
{
  std::ofstream outputFile;
  outputFile.open(robot_name_ + ".log",
                  std::ios::app | std::ios::out | std::ios::ate);
  outputFile << "- generation: " << generation_counter_ << std::endl;
  outputFile << "  velocities:" << std::endl;
  for (auto const &it : ranked_policies_) {
    double fitness = it.first;
    outputFile << "  - " << fitness << std::endl;
  }
  outputFile.close();
}

void
RLPowerLearner::writeElite()
{
  std::ofstream outputFile;
  outputFile.open(robot_name_ + ".policy",
                  std::ios::app | std::ios::out | std::ios::ate);
  outputFile << "- evaluation: " << generation_counter_ << std::endl;
  outputFile << "  steps: " << source_y_size_ << std::endl;
  outputFile << "  population:" << std::endl;
  for (auto const &it : ranked_policies_) {
    double fitness = it.first;
    PolicyPtr policy = it.second;
    outputFile << "   - velocity: " << fitness << std::endl;
    outputFile << "     policy:" << std::endl;
    for (size_t i = 0; i < policy->size(); i++) {
      Spline &spline = policy->at(i);
      for (size_t j = 0; j < spline.size(); j++) {
        outputFile << "      - " << spline.at(j) << std::endl;
      }
    }
  }
  outputFile.close();
}

const size_t RLPowerLearner::MAX_EVALUATIONS = 1000; // max number of evaluations
const size_t RLPowerLearner::MAX_RANKED_POLICIES = 10; // max length of policies vector
const size_t RLPowerLearner::INTERPOLATION_CACHE_SIZE = 100; // number of data points for the interpolation cache
const size_t RLPowerLearner::INITIAL_SPLINE_SIZE = 3; // number of initially sampled spline points
const size_t RLPowerLearner::UPDATE_STEP = 100; // after # generations, it increases the number of spline points
const double RLPowerLearner::EVALUATION_RATE = 30.0; // evaluation time for each policy
const double RLPowerLearner::SIGMA_START_VALUE = 0.8; // starting value for sigma
const double RLPowerLearner::SIGMA_TAU_CORRECTION = 0.2;

const double RLPowerLearner::CYCLE_LENGTH = 5; // seconds
const double RLPowerLearner::SIGMA_DECAY_SQUARED = 0.98; // sigma decay
