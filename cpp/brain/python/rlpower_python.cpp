#include "rlpower_python.h"

#include "python_list_wrap.h"

using namespace revolve::brain;

RLPower_python::RLPower_python(revolve::brain::RLPower::EvaluatorPtr evaluator,
                               unsigned int n_actuators,
                               unsigned int n_sensors)
    : RLPower::RLPower(evaluator,
                       n_actuators,
                       n_sensors)
{}

void RLPower_python::update(boost::python::list & actuators,
                            const boost::python::list & sensors,
                            double t,
                            double step)
{
    python_list_wrap<ActuatorPtr> actuator_wrap(&actuators);
    python_list_wrap<SensorPtr>   sensor_wrap(&sensors);

    RLPower::update<python_list_wrap<ActuatorPtr>, python_list_wrap<SensorPtr>>(actuator_wrap, sensor_wrap, t, step);
}
