#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <boost/shared_ptr.hpp>

class Actuator
{
public:
    Actuator();

    virtual void update(const double *output_vector, const double step) = 0;
    virtual unsigned int outputs() const = 0;
};

typedef boost::shared_ptr< Actuator > ActuatorPtr;

#endif // ACTUATOR_H
