#ifndef REVOLVEBRAIN_BRAIN_PYTHON_EVALUATORWRAP_H_
#define REVOLVEBRAIN_BRAIN_PYTHON_EVALUATORWRAP_H_

#include <boost/python/wrapper.hpp>
#include <boost/python/object.hpp>
#include "brain/evaluator.h"

#include <iostream>

namespace revolve {
namespace brain {

/**
 * Class needed to enable calling virtual methods implemented child python classes
 */
class EvaluatorWrap
        : public Evaluator
          , public boost::python::wrapper<Evaluator>
{
public:
    virtual void
    start()
    {
      this->get_override("start")();
    }

    virtual double
    fitness()
    {
      return this->get_override("fitness")();
    };

};

}
}

#endif // REVOLVEBRAIN_BRAIN_PYTHON_EVALUATORWRAP_H_
