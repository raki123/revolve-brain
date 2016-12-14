#ifndef REVOLVE_BRAIN_SPLINE_CONTROLLER_H
#define REVOLVE_BRAIN_SPLINE_CONTROLLER_H

#include "base_controller.h"

namespace revolve {
namespace brain {

class base_learner;

class spline_controller : public base_controller
{
public: // typedefs
    typedef std::vector<double> Spline;
    typedef std::vector<Spline> Policy;
    typedef std::shared_ptr<Policy> PolicyPtr;

    friend class base_learner;

// METHODS-FUNCTIONS ----------------------------------------------------------
public: // STATIC METHODS
    /**
    * Generate interpolated spline based on number of sampled control points in 'source_y'
    * @param source_y: set of control points over which interpolation is generated
    * @param destination_y: set of interpolated control points (default 100 points)
    */
    static void Interpolate_cubic(Policy *const source_y,
                                  Policy *destination_y);

    static spline_controller* GenerateRandomController(double noise_sigma,
            unsigned int n_actuators,
            unsigned int n_spline_points,
            unsigned int interpolation_cache_size);

    static spline_controller* GenerateRandomController(double noise_sigma,
            unsigned int n_actuators,
            unsigned int n_spline_points)
    {
        return GenerateRandomController(noise_sigma,
                                        n_actuators,
                                        n_spline_points,
                                        INTERPOLATION_CACHE_SIZE);
    }

public: // methods
    explicit spline_controller(unsigned int n_actuators,
                               unsigned int n_spline_points,
                               unsigned int interpolation_cache_size);

    explicit spline_controller(unsigned int n_actuators,
                               unsigned int n_spline_points);

    virtual ~spline_controller();

    virtual void update(const std::vector<ActuatorPtr> &actuators,
                        const std::vector<SensorPtr> &sensors,
                        double t,
                        double step) override;

    virtual void generateOutput(const double time,
                                double *output_vector);

    /**
     * Generate cache policy
     */
    void update_cache();

// VARIABLES-CONSTANTS --------------------------------------------------------
public: // STATIC CONSTANTS
    // cycle lenght in seconds
    static const double CYCLE_LENGTH;// = 5 seconds
    /** Default value for the number of `interpolation_cache` data points
     * (default value for `interpolation_cache_size`)
     */
    static const unsigned int INTERPOLATION_CACHE_SIZE;// = 100

protected: // consts
    // number of actuators the spline is expeting to write into
    const unsigned int n_actuators;
    // number of spline points
    const unsigned int n_spline_points;
    // Number of `interpolation_cache` data points
    const unsigned int interpolation_cache_size;

protected: // variables
    // Pointer to the current policy
    PolicyPtr policy;
    // Pointer to the interpolated current_policy_ (default 100 points)
    PolicyPtr interpolation_cache = nullptr;
    double cycle_start_time;
};

}
}

#endif // REVOLVE_BRAIN_SPLINE_CONTROLLER_H
