#ifndef TOL_REVOLVE_POLICY_CONTROLLER_H
#define TOL_REVOLVE_POLICY_CONTROLLER_H

#include "Controller.h"

namespace revolve {
namespace brain {

typedef std::vector<double> Spline;
typedef std::vector<Spline> Policy;
typedef std::shared_ptr<Policy> PolicyPtr;

class PolicyController
        : public Controller<PolicyPtr>
{
public:
    static const double CYCLE_LENGTH; // = 5 seconds
    static const unsigned int INTERPOLATION_CACHE_SIZE; // = 100 points

    explicit
    PolicyController(unsigned int n_actuators,
                     unsigned int interpolation_cache_size);

    explicit
    PolicyController(unsigned int n_actuators);

    virtual
    ~PolicyController();

    void
    update(const std::vector<ActuatorPtr> &actuators,
           const std::vector<SensorPtr> &sensors,
           double t,
           double step) override;

    void
    generateOutput(const double time,
                   double *output_vector);

    PolicyPtr
    getGenome();

    void
    setGenome(PolicyPtr policy);

    /**
     * Generate cache policy
     */
    void
    update_cache();

// STATIC METHODS -----

    static void
    InterpolateCubic(Policy *const source_y,
                     Policy *destination_y);

    static PolicyController *
    GenerateRandomController(double noise_sigma,
                             unsigned int n_actuators,
                             unsigned int n_spline_points,
                             unsigned int interpolation_cache_size);

    static PolicyController *
    GenerateRandomController(double noise_sigma,
                             unsigned int n_actuators,
                             unsigned int n_spline_points)
    {
        return GenerateRandomController(noise_sigma,
                                        n_actuators,
                                        n_spline_points,
                                        INTERPOLATION_CACHE_SIZE);
    }

protected:
    // number of actuators the controller is expecting to send signal
    const unsigned int n_actuators_;
    // number of `interpolation_cache` data points
    const unsigned int interpolation_cache_size_;
    // pointer to the current policy
    PolicyPtr policy_;
    // pointer to the interpolated current_policy_ (default 100 points)
    PolicyPtr interpolation_cache_;
    // start time of one cycle from which we count
    double cycle_start_time_;
private:

};

}
}

#endif //TOL_REVOLVE_SPLINES_CONTROLLER_H
