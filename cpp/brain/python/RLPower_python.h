#ifndef REVOLVE_BRAIN_RLPOWER_PYTHON_H
#define REVOLVE_BRAIN_RLPOWER_PYTHON_H

#include "brain/RLPower.h"
#include <boost/python/object.hpp>
#include <boost/python/list.hpp>
#include <string>

namespace revolve {
namespace brain {

class RLPower_python
        : public revolve::brain::RLPower
{
public:
    RLPower_python(std::string robot_name,
                   const boost::python::object &conf,
                   revolve::brain::EvaluatorPtr evaluator,
                   unsigned int n_actuators,
                   unsigned int n_sensors);

    void
    update(boost::python::list &actuators,
           const boost::python::list &sensors,
           double t,
           double step);

    static RLPower::Config
    create_config(const boost::python::object &conf);
};

}
}

#endif // REVOLVE_BRAIN_RLPOWER_PYTHON_H
