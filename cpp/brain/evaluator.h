#ifndef REVOLVE_BRAIN_EVALUATOR_H
#define REVOLVE_BRAIN_EVALUATOR_H

namespace revolve {
namespace brain {

class Evaluator
{
public:
    virtual ~Evaluator() {};

    virtual void start() = 0;
    virtual double fitness() = 0;
};

}
}

#endif // REVOLVE_BRAIN_EVALUATOR_H
