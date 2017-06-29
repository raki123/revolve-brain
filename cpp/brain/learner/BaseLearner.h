#ifndef REVOLVEBRAIN_BRAIN_LEARNER_BASELEARNER_H_
#define REVOLVEBRAIN_BRAIN_LEARNER_BASELEARNER_H_

#include <vector>
#include <memory>
#include "brain/Actuator.h"
#include "brain/Sensor.h"
#include "brain/controller/BaseController.h"

namespace revolve {
namespace brain {

class BaseLearner
{
public:
    /**
     * @brief takes ownership of the controller and moves it inside
     *
     * @param controller p_controller: the base_controller will take ownership
     * of this controller
     * @param name of the robot that the learner is instructing (for logging purposes)
     */
    explicit BaseLearner(std::unique_ptr<BaseController> controller, const std::string &robot_name);

    // REMEMBER TO MAKE YOUR CHILD DECONSTRUCTORS VIRTUAL AS WELL
    virtual ~BaseLearner();

    /**
     * Updates the learner and modifies the controller if needed. Is using the
     * sensor readings to evaluate the current controller and if is needed is
     * updating the controller to a new one.
     *
     * It usually is implemented like this: during an evaluation period it
     * returns always the same controller. After the evaluation it evaluates
     * the controller and generates a new one that is returned.
     *
     * The update function could return always the same controller with modified
     * parameters or a new controller. You should use this pointer assuming
     * either of the two could happen at random times for the same learner.
     *
     * asserts that the active_controller is not empty.
     * (`assert active_controller.get() != nullptr`)
     *
     * @brief Updates the learner and modifies the controller if needed.
     * Returns the controller you should use.
     *
     * @param sensors sensor readings
     * @param t global time reference
     * @param step time since last update
     * @return revolve::brain::base_controller* the controller you should use
     * in this update cycle. The pointer is guaranteed to be valid until the
     * end of the update cycle.
     */
    virtual BaseController *
    update(const std::vector<SensorPtr> &sensors,
           double t,
           double step);

protected:
    /**
     * Careful, if the update function is called with an empty active_controller,
     * it will fail (`assert active_controller.get() != nullptr`)
     *
     * @brief constructor that leaves the active_controller empty.
     * @param name of the robot that the learner is instructing (for logging purposes)
     */
    explicit BaseLearner(const std::string &robot_name);

    /***
     * Creates a new controller. Useful if you want to wrap the controller inside another.
     * @param fitness fitness of the previous controller.
     * @return pointer to the new controller.
     */
    virtual BaseController *
    create_new_controller(double fitness) = 0;

protected:
    /**
     * @brief pointer to the current active controller
     */
    std::unique_ptr<BaseController> active_controller;

    /**
     * @brief name of the robot that the learner is instructing (for logging purposes)
     */
    const std::string robot_name;
};

}
}

#endif // REVOLVEBRAIN_BRAIN_LEARNER_BASELEARNER_H_
