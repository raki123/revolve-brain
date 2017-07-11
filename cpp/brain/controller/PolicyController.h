#ifndef REVOLVEBRAIN_BRAIN_CONTROLLER_POLICYCONTROLLER_H_
#define REVOLVEBRAIN_BRAIN_CONTROLLER_POLICYCONTROLLER_H_

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
    static const size_t INTERPOLATION_CACHE_SIZE; // = 100 points

  /// \brief Constructor
  explicit PolicyController(size_t n_actuators,
                            size_t interpolation_cache_size);

  /// \brief Constructor
  explicit PolicyController(size_t n_actuators);

  /// \brief Destructor
  virtual ~PolicyController() override;

    void update(const std::vector<ActuatorPtr> &actuators,
                const std::vector<SensorPtr> &sensors,
                double t,
                double step) override;

    void generateOutput(const double time,
                        double *output_vector);

    PolicyPtr getPhenotype() override;

    void setPhenotype(PolicyPtr policy) override;

    /// \brief Generate cache policy
    void update_cache();

// STATIC METHODS -----

    static void
    InterpolateCubic(Policy *const source_y,
                     Policy *destination_y);

    static PolicyController *
    GenerateRandomController(double noise_sigma,
                             size_t n_actuators,
                             size_t n_spline_points,
                             size_t interpolation_cache_size);

    static PolicyController *
    GenerateRandomController(double noise_sigma,
                             size_t n_actuators,
                             size_t n_spline_points)
    {
      return GenerateRandomController(noise_sigma,
                                      n_actuators,
                                      n_spline_points,
                                      INTERPOLATION_CACHE_SIZE);
    }

protected:
    // number of actuators the controller is expecting to send signal
    const size_t n_actuators_;
    // number of `interpolation_cache` data points
    const size_t interpolation_cache_size_;
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

#endif // REVOLVEBRAIN_BRAIN_CONTROLLER_POLICYCONTROLLER_H_
