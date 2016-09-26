#ifndef REVOLVE_BRAIN_SENSOR_H
#define REVOLVE_BRAIN_SENSOR_H

#include <string>
#include <boost/shared_ptr.hpp>

namespace revolve {
namespace brain {

class Sensor
{
public:
    virtual ~Sensor() {}

    virtual void read(double *input_vector) = 0;
    virtual unsigned int inputs() const = 0;
    virtual std::string sensorId() const = 0;
};

typedef boost::shared_ptr< Sensor > SensorPtr;

}
}


#endif // REVOLVE_BRAIN_SENSOR_H
