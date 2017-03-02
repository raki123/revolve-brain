#ifndef REVOLVEBRAIN_BRAIN_ACTUATOR_H_
#define REVOLVEBRAIN_BRAIN_ACTUATOR_H_

#include <boost/shared_ptr.hpp>

namespace revolve {
namespace brain {

class Actuator
{
public:
    virtual ~Actuator()
    {}

    virtual void
    update(double *output_vector,
           double step) = 0;

    virtual unsigned int
    outputs() const = 0;
};

typedef boost::shared_ptr<Actuator> ActuatorPtr;

}
}


#endif // REVOLVEBRAIN_BRAIN_ACTUATOR_H_
