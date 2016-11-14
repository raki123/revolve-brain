//
// Created by matteo on 09/11/16.
//

#include "CPGBrain_python.h"
#include "python_list_wrap.h"

using namespace revolve::brain;

CPGBrain_python::CPGBrain_python(std::string robot_name,
                                 EvaluatorPtr evaluator,
                                 unsigned int n_actuators,
                                 unsigned int n_sensors)
    : CPGBrain(robot_name, evaluator, n_actuators, n_sensors)
{

}

void CPGBrain_python::update(boost::python::list &actuators,
                             const boost::python::list &sensors,
                             double t, double step)
{
    python_list_wrap<ActuatorPtr> actuator_wrap(&actuators);
    python_list_wrap<SensorPtr>   sensor_wrap(&sensors);

    CPGBrain::update
        <python_list_wrap<ActuatorPtr>, python_list_wrap<SensorPtr>>
        (actuator_wrap, sensor_wrap, t, step);
}


