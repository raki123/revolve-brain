#ifndef REVOLVEBRAIN_BRAIN_PYTHON_ACTUATORWRAP_H_
#define REVOLVEBRAIN_BRAIN_PYTHON_ACTUATORWRAP_H_

#include <boost/python/wrapper.hpp>
#include <boost/python/object.hpp>
#include "brain/Actuator.h"
#include "brain/python/PythonArray.h"

#include <iostream>

namespace revolve {
namespace brain {

/**
 * Class needed to enable calling virtual methods implemented child python classes
 */
class ActuatorWrap
        : public revolve::brain::Actuator
          , public boost::python::wrapper<Actuator>
{
public:
    virtual void
    update(double *output_vector,
           double step)
    {
      this->get_override("update")(python_array<double>(output_vector),
                                   step);
    }

    virtual unsigned int
    outputs() const
    {
      return this->get_override("outputs")();
    };

};

}
}

#endif // REVOLVEBRAIN_BRAIN_PYTHON_ACTUATORWRAP_H_
