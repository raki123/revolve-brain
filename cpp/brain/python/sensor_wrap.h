#ifndef REVOLVE_BRAIN_SENSOR_WRAP_H
#define REVOLVE_BRAIN_SENSOR_WRAP_H

#include <boost/python/wrapper.hpp>
#include <boost/python/object.hpp>
#include "brain/sensor.h"

#include <iostream>

namespace revolve {
namespace brain {

class SensorWrap : public Sensor, public boost::python::wrapper<Sensor>
{
public:
    virtual void read(double *input_vector) {
        this->get_override("read")(input_vector);
    }

    virtual unsigned int inputs() const {
        return this->get_override("inputs")();
    };

};

}
}

#endif // REVOLVE_BRAIN_SENSOR_WRAP_H
