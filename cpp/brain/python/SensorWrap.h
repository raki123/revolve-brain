#ifndef REVOLVEBRAIN_BRAIN_PYTHON_SENSORWRAP_H_
#define REVOLVEBRAIN_BRAIN_PYTHON_SENSORWRAP_H_

#include <boost/python/wrapper.hpp>
#include <boost/python/object.hpp>
#include "brain/Sensor.h"
#include "brain/python/PythonArray.h"

#include <string>

namespace revolve {
namespace brain {

/**
 * Class needed to enable calling virtual methods implemented child python classes
 */
class SensorWrap
        : public Sensor
          , public boost::python::wrapper<Sensor>
{
public:
    virtual void
    read(double *input_vector)
    {
      this->get_override("read")(python_array<double>(input_vector));
    }

    virtual unsigned int
    inputs() const
    {
      return this->get_override("inputs")();
    };

    virtual std::string
    sensorId() const
    {
      return this->get_override("sensorId")();
    };

};

}
}

#endif // REVOLVEBRAIN_BRAIN_PYTHON_SENSORWRAP_H_
