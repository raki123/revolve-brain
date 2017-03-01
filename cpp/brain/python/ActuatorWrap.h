#ifndef REVOLVE_BRAIN_ACTUATOR_WRAP_H
#define REVOLVE_BRAIN_ACTUATOR_WRAP_H

#include <boost/python/wrapper.hpp>
#include <boost/python/object.hpp>
#include "brain/actuator.h"
#include "brain/python/python_array.h"

#include <iostream>

namespace revolve {
namespace brain {

/**
 * Class needed to enable calling virtual methods implemented child python classes
 */
class ActuatorWrap : public revolve::brain::Actuator, public boost::python::wrapper<Actuator>
{
public:
    virtual void update(double *output_vector, double step) {
        this->get_override("update")(python_array<double>(output_vector), step);
    }

    virtual unsigned int outputs() const {
        return this->get_override("outputs")();
    };

};

}
}

#endif // REVOLVE_BRAIN_ACTUATOR_WRAP_H
