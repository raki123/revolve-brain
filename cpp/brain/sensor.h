#ifndef SENSOR_H
#define SENSOR_H

#include <boost/shared_ptr.hpp>

class Sensor
{
public:
    Sensor();

    virtual void read(double *input_vector) = 0;
    virtual unsigned int inputs() const = 0;
};

typedef boost::shared_ptr< Sensor > SensorPtr;

#endif // SENSOR_H
