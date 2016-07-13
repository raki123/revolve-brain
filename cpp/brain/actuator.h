#ifndef REVOLVE_BRAIN_ACTUATOR_H
#define REVOLVE_BRAIN_ACTUATOR_H

#include <boost/shared_ptr.hpp>

namespace revolve {
namespace brain {

class Actuator
{
public:
    virtual void update(double *output_vector, double step) = 0;
    virtual unsigned int outputs() const = 0;
};

typedef std::shared_ptr< Actuator > ActuatorPtr;

}
}


#endif // REVOLVE_BRAIN_ACTUATOR_H
