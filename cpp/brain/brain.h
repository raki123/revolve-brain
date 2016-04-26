/*
 * Specifies a utility `Brain` base class. If your brain doesn't
 * fit this model, something else can easily be used by simply
 * ignoring the default brain behavior in the `RobotController`.
 */

#ifndef REVOLVE_GAZEBO_BRAIN_BRAIN_H_
#define REVOLVE_GAZEBO_BRAIN_BRAIN_H_

#include <vector>
#include "sensor.h"
#include "actuator.h"

class Brain {
public:
    Brain() : offline(false) {};
    virtual ~Brain() {};

    /**
     * Update step called for the brain.
     *
     * @param actuators List of actuators
     * @param sensors List of sensors
     * @param t Current simulation time
     * @param step Actuation step size in seconds
     */
    virtual void update(const std::vector< ActuatorPtr > & actuators, const std::vector< SensorPtr > & sensors,
                double t, double step) = 0;

    /**
     * If offline, the robot won't update his controller. This is delegated to
     * a supervisor.
     * @return true if the brain is offline
     */
    bool isOffline() {
        return this->offline;
    }

    /**
     * If offline, the robot won't update his controller. This is delegated to
     * a supervisor
     * @param offline set offline mode on or off
     */
    virtual void setOffline(bool offline) {
        this->offline = offline;
    }

protected:
    bool offline;
};

#endif /* REVOLVE_GAZEBO_BRAIN_BRAIN_H_ */
