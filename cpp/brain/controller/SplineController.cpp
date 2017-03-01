#include "SplineController.h"

using namespace revolve::brain;

#include <gsl/gsl_spline.h>
#include <random>

const double SplineController::CYCLE_LENGTH = 5; // seconds
const unsigned int SplineController::INTERPOLATION_CACHE_SIZE = 100; // number of data

SplineController * SplineController::GenerateRandomController(
    double noise_sigma,
    unsigned int n_actuators,
    unsigned int n_spline_points,
    unsigned int interpolation_cache_size
)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::normal_distribution<double> dist(0, noise_sigma);

    SplineController *controller = new SplineController(n_actuators,
            n_spline_points,
            interpolation_cache_size);

    // Init first random controller
    if (!controller->policy)
        controller->policy = std::make_shared<Policy>(n_actuators);

    for (unsigned int i = 0; i < n_actuators; i++) {
        Spline spline(n_spline_points);
        for (unsigned int j = 0; j < n_spline_points; j++) {
            spline[j] = dist(mt);
        }
        controller->policy->at(i) = spline;
    }

    // Init of empty cache
    if (!controller->interpolation_cache)
        controller->interpolation_cache = std::make_shared<Policy>(n_actuators);

    for (unsigned int i = 0; i < n_actuators; i++) {
        controller->interpolation_cache->at(i).resize(controller->interpolation_cache_size, 0);
    }

    controller->update_cache();
    return controller;
}



SplineController::SplineController(unsigned int n_actuators,
                                     unsigned int n_spline_points,
                                     unsigned int interpolation_cache_size)
    : n_actuators(n_actuators)
    , n_spline_points(n_spline_points)
    , interpolation_cache_size(interpolation_cache_size)
    , interpolation_cache(nullptr)
    , cycle_start_time(-1)
{
    this->policy = std::make_shared<Policy>(n_actuators);
    for (unsigned int i = 0; i < n_actuators; i++) {
        Spline spline(n_spline_points, 0);
        this->policy->at(i) = spline;
    }

    // Init of empty cache
    interpolation_cache = std::make_shared<Policy>(n_actuators);

    for (unsigned int i = 0; i < n_actuators; i++) {
        interpolation_cache->at(i).resize(interpolation_cache_size, 0);
    }
}


SplineController::SplineController(unsigned int n_actuators,
                                     unsigned int n_spline_points)
    : SplineController(n_actuators, n_spline_points, INTERPOLATION_CACHE_SIZE)
{}


SplineController::~SplineController()
{}

void SplineController::update(const std::vector<ActuatorPtr> &actuators,
                               const std::vector<SensorPtr> &sensors,
                               double t,
                               double step)
{
    // generate outputs
    double *output_vector = new double[n_actuators];
    this->generateOutput(t, output_vector);

    // Send new signals to the actuators
    unsigned int p = 0;
    for (auto actuator: actuators) {
        actuator->update(&output_vector[p], step);
        p += actuator->outputs();
    }

    delete[] output_vector;
}

void SplineController::generateOutput(const double time, double* output_vector)
{
    if (cycle_start_time < 0) {
        cycle_start_time = time;
    }

    // get correct X value (between 0 and CYCLE_LENGTH)
    double x = time - cycle_start_time;
    while (x >= SplineController::CYCLE_LENGTH) {
        cycle_start_time += SplineController::CYCLE_LENGTH;
        x = time - cycle_start_time;
    }

    // adjust X on the cache coordinate space
    x = (x / CYCLE_LENGTH) * interpolation_cache_size;
    // generate previous and next values
    int x_a = ((int) x) % interpolation_cache_size;
    int x_b = (x_a + 1) % interpolation_cache_size;

    // linear interpolation for every actuator
    for (unsigned int i = 0; i < n_actuators; i++) {
        double y_a = interpolation_cache->at(i)[x_a];
        double y_b = interpolation_cache->at(i)[x_b];

        output_vector[i] = y_a +
                           ((y_b - y_a) * (x - x_a) / (x_b - x_a));
    }
}

void SplineController::update_cache()
{
    this->Interpolate_cubic(policy.get(), interpolation_cache.get());
}

void SplineController::Interpolate_cubic(Policy *const source_y, Policy* destination_y)
{
    const unsigned int source_y_internal_size = (*source_y)[0].size();
    const unsigned int destination_y_internal_size = (*destination_y)[0].size();
    const unsigned int n_actuators = source_y->size();
    assert(n_actuators == destination_y->size());

    const unsigned int N = source_y_internal_size + 1;
    double *x = new double[N];
    double *y = new double[N];
    double *x_new = new double[destination_y_internal_size];

    gsl_interp_accel *acc = gsl_interp_accel_alloc();
    const gsl_interp_type *t = gsl_interp_cspline_periodic;
    gsl_spline *spline = gsl_spline_alloc(t, N);

    // init x
    double step_size = CYCLE_LENGTH / source_y_internal_size;
    for (unsigned int i = 0; i < N; i++) {
        x[i] = step_size * i;
    }

    // init x_new
    step_size = CYCLE_LENGTH / destination_y_internal_size;
    for (unsigned int i = 0; i < destination_y_internal_size; i++) {
        x_new[i] = step_size * i;
    }

    for (unsigned int j = 0; j < n_actuators; j++) {
        Spline &source_y_line = source_y->at(j);
        Spline &destination_y_line = destination_y->at(j);

        // init y
        // TODO use memcpy
        for (unsigned int i = 0; i < source_y_internal_size; i++) {
            y[i] = source_y_line[i];
        }

        // make last equal to first
        y[N - 1] = y[0];

        gsl_spline_init(spline, x, y, N);

        for (unsigned int i = 0; i < destination_y_internal_size; i++) {
            destination_y_line[i] = gsl_spline_eval(spline, x_new[i], acc);
        }
    }

    gsl_spline_free(spline);
    gsl_interp_accel_free(acc);

    delete[] x_new;
    delete[] y;
    delete[] x;
}

