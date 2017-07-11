#include "PolicyController.h"

using namespace revolve::brain;

#include <gsl/gsl_spline.h>
#include <random>

const double PolicyController::CYCLE_LENGTH = 5; // seconds
const size_t PolicyController::INTERPOLATION_CACHE_SIZE = 100; // number of data

PolicyController::PolicyController(size_t n_actuators,
                                   size_t interpolation_cache_size)
        : n_actuators_(n_actuators)
        , interpolation_cache_size_(interpolation_cache_size)
        , interpolation_cache_(nullptr)
        , cycle_start_time_(-1)
{
  // Init of empty cache
  interpolation_cache_ = std::make_shared<Policy>(n_actuators);

  for (size_t i = 0; i < n_actuators; i++) {
    interpolation_cache_->at(i).resize(interpolation_cache_size_, 0);
  }
}

PolicyController::PolicyController(size_t n_actuators)
        : PolicyController(n_actuators, INTERPOLATION_CACHE_SIZE)
{}

PolicyController::~PolicyController()
{}

void PolicyController::update(const std::vector<ActuatorPtr> &actuators,
                              const std::vector<SensorPtr> &sensors,
                              double t,
                              double step)
{
  // generate outputs
  double *output_vector = new double[n_actuators_];
  this->generateOutput(t, output_vector);

  // Send new signals to the actuators
  size_t p = 0;
  for (auto actuator: actuators) {
    actuator->update(&output_vector[p], step);
    p += actuator->outputs();
  }

  delete[] output_vector;
}

void PolicyController::generateOutput(const double time,
                                      double *output_vector)
{
  if (cycle_start_time_ < 0) {
    cycle_start_time_ = time;
  }

  // get correct X value (between 0 and CYCLE_LENGTH)
  double x = time - cycle_start_time_;
  while (x >= PolicyController::CYCLE_LENGTH) {
    cycle_start_time_ += PolicyController::CYCLE_LENGTH;
    x = time - cycle_start_time_;
  }

  // adjust X on the cache coordinate space
  x = (x / CYCLE_LENGTH) * interpolation_cache_size_;
  // generate previous and next values
  int x_a = ((int)x) % interpolation_cache_size_;
  int x_b = (x_a + 1) % interpolation_cache_size_;

  // linear interpolation for every actuator
  for (size_t i = 0; i < n_actuators_; i++) {
    double y_a = interpolation_cache_->at(i)[x_a];
    double y_b = interpolation_cache_->at(i)[x_b];

    output_vector[i] = y_a +
                       ((y_b - y_a) * (x - x_a) / (x_b - x_a));
  }
}

PolicyPtr PolicyController::getPhenotype()
{
  return policy_;
}

void PolicyController::setPhenotype(PolicyPtr policy)
{
  policy_ = policy;
  update_cache();
  cycle_start_time_ = -1;
  //TODO:: make sure the current time in cycle is correct.
}

void PolicyController::update_cache()
{
  this->InterpolateCubic(policy_.get(),
                         interpolation_cache_.get());
}

void PolicyController::InterpolateCubic(Policy *const source_y,
                                        Policy *destination_y)
{
  const size_t source_y_internal_size = (*source_y)[0].size();
  const size_t destination_y_internal_size = (*destination_y)[0].size();
  const size_t n_actuators = source_y->size();
  assert(n_actuators == destination_y->size());

  const size_t N = source_y_internal_size + 1;
  double *x = new double[N];
  double *y = new double[N];
  double *x_new = new double[destination_y_internal_size];

  gsl_interp_accel *acc = gsl_interp_accel_alloc();
  const gsl_interp_type *t = gsl_interp_cspline_periodic;
  gsl_spline *spline = gsl_spline_alloc(t,
                                        N);

  // init x
  double step_size = CYCLE_LENGTH / source_y_internal_size;
  for (size_t i = 0; i < N; i++) {
    x[i] = step_size * i;
  }

  // init x_new
  step_size = CYCLE_LENGTH / destination_y_internal_size;
  for (size_t i = 0; i < destination_y_internal_size; i++) {
    x_new[i] = step_size * i;
  }

  for (size_t j = 0; j < n_actuators; j++) {
    Spline &source_y_line = source_y->at(j);
    Spline &destination_y_line = destination_y->at(j);

    // init y
    // TODO use memcpy
    for (size_t i = 0; i < source_y_internal_size; i++) {
      y[i] = source_y_line[i];
    }

    // make last equal to first
    y[N - 1] = y[0];

    gsl_spline_init(spline,
                    x,
                    y,
                    N);

    for (size_t i = 0; i < destination_y_internal_size; i++) {
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

PolicyController *
PolicyController::GenerateRandomController(double noise_sigma,
                                           size_t n_actuators,
                                           size_t n_spline_points,
                                           size_t interpolation_cache_size)
{
  std::random_device rd;
  std::mt19937 mt(rd());
  std::normal_distribution<double> dist(0,
                                        noise_sigma);

  PolicyController *controller = new PolicyController(n_actuators,
                                                      interpolation_cache_size);

  // Init first random controller
  if (!controller->policy_)
    controller->policy_ = std::make_shared<Policy>(n_actuators);

  for (size_t i = 0; i < n_actuators; i++) {
    Spline spline(n_spline_points);
    for (size_t j = 0; j < n_spline_points; j++) {
      spline[j] = dist(mt);
    }
    controller->policy_
              ->at(i) = spline;
  }

  // Init of empty cache
  if (!controller->interpolation_cache_)
    controller->interpolation_cache_ = std::make_shared<Policy>(n_actuators);

  for (size_t i = 0; i < n_actuators; i++) {
    controller->interpolation_cache_
              ->at(i)
              .resize(controller->interpolation_cache_size_,
                      0);
  }

  controller->update_cache();
  return controller;
}