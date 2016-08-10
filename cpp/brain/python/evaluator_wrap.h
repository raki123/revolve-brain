#ifndef REVOLVE_BRAIN_EVALUATOR_WRAP_H
#define REVOLVE_BRAIN_EVALUATOR_WRAP_H

#include <boost/python/wrapper.hpp>
#include <boost/python/object.hpp>
#include "brain/evaluator.h"

#include <iostream>

namespace revolve {
namespace brain {

/**
 * Class needed to enable calling virtual methods implemented child python classes
 */
class EvaluatorWrap : public Evaluator, public boost::python::wrapper<Evaluator>
{
public:
    virtual void start() {
        this->get_override("start")();
    }

    virtual double fitness() {
        return this->get_override("fitness")();
    };

};

}
}

#endif // REVOLVE_BRAIN_EVALUATOR_WRAP_H
