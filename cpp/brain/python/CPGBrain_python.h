//
// Created by matteo on 09/11/16.
//

#ifndef REVOLVE_BRAIN_CPGBRAIN_PYTHON_H
#define REVOLVE_BRAIN_CPGBRAIN_PYTHON_H

#include "brain/CPGBrain.h"
#include <boost/python/list.hpp>
#include <vector>
#include <vector>

namespace revolve {
namespace brain {

class CPGBrain_python : public revolve::brain::CPGBrain
{
public:
    CPGBrain_python(std::string robot_name,
                    EvaluatorPtr evaluator,
                    unsigned int n_actuators,
                    unsigned int n_sensors);

    void update(boost::python::list& actuators,
                const boost::python::list& sensors,
                double t, double step);
};

}
}


#endif //REVOLVE_BRAIN_CPGBRAIN_PYTHON_H
