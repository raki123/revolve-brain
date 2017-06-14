#ifndef REVOLVE_BRAIN_BASECONTROLLER_H_
#define REVOLVE_BRAIN_BASECONTROLLER_H_

#include <vector>
#include "brain/Actuator.h"
#include "brain/Sensor.h"

namespace revolve {
namespace brain {

class BaseController
{
public:
    // REMEMBER TO MAKE YOUR CHILD DECONSTRUCTORS VIRTUAL AS WELL
    virtual ~BaseController()
    {}

    /**
     * @brief Update the controller to the next step and sends signals
     * to the actuators
     *
     * @param actuators outputs of the controller
     * @param sensors inputs of the controller
     * @param t global time reference
     * @param step time since last update
     */

    virtual void
    update(const std::vector<ActuatorPtr> &actuators,
           const std::vector<SensorPtr> &sensors,
           double t,
           double step) = 0;
};

}
}

#endif // REVOLVE_BRAIN_BASECONTROLLER_H_
