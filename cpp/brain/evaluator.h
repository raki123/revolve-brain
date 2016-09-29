#ifndef REVOLVE_BRAIN_EVALUATOR_H
#define REVOLVE_BRAIN_EVALUATOR_H

#include <boost/shared_ptr.hpp>

namespace revolve {
namespace brain {

class Evaluator
{
public:
    virtual ~Evaluator() {};

    virtual void start() = 0;
    virtual double fitness() = 0;
};

typedef boost::shared_ptr<revolve::brain::Evaluator> EvaluatorPtr;

}
}

#endif // REVOLVE_BRAIN_EVALUATOR_H
