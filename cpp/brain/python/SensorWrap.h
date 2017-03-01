#ifndef REVOLVE_BRAIN_SENSOR_WRAP_H
#define REVOLVE_BRAIN_SENSOR_WRAP_H

#include <boost/python/wrapper.hpp>
#include <boost/python/object.hpp>
#include "brain/sensor.h"
#include "brain/python/python_array.h"

#include <string>

namespace revolve {
namespace brain {

/**
 * Class needed to enable calling virtual methods implemented child python classes
 */
class SensorWrap : public Sensor, public boost::python::wrapper<Sensor>
{
public:
    virtual void read(double *input_vector) {
        this->get_override("read")(python_array<double>(input_vector));
    }

    virtual unsigned int inputs() const {
        return this->get_override("inputs")();
    };

    virtual std::string sensorId() const {
        return this->get_override("sensorId")();
    };

};

}
}

#endif // REVOLVE_BRAIN_SENSOR_WRAP_H
