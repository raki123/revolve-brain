#ifndef REVOLVE_BRAIN_RLPOWER_PYTHON_H
#define REVOLVE_BRAIN_RLPOWER_PYTHON_H

#include <brain/rlpower.h>
#include <boost/python/list.hpp>

namespace revolve {
namespace brain {

class RLPower_python : public revolve::brain::RLPower
{
public:
RLPower_python(revolve::brain::RLPower::EvaluatorPtr evaluator, unsigned int n_actuators, unsigned int n_sensors);
void update(boost::python::list& actuators, const boost::python::list& sensors, double t, double step);
};

}}

#endif // REVOLVE_BRAIN_RLPOWER_PYTHON_H
