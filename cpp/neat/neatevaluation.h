#ifndef REVOLVE_NEAT_NEATEVALUATION_H_
#define REVOLVE_NEAT_NEATEVALUATION_H_

#include <functional>
#include <iostream>

#include "organism.h"

class NeatEvaluation
{
public:
    NeatEvaluation(NEAT::Organism *organism);

    virtual ~NeatEvaluation()
    {};

    std::function<void(float)>
    add_finished_callback(std::function<void(float)> new_callback)
    {
      std::function<void(float)> old_callback = finished_callback;
      finished_callback = new_callback;
      return old_callback;
    }

    const NEAT::Organism *
    getOrganism() const
    {
      return organism;
    }

    /**
     * Evaluation is finished and fitness is passed to the evaluation
     */
    virtual void
    finish(float fitness);

private:
    std::function<void(float)> finished_callback;
    float fitness;
    NEAT::Organism *organism;
};

#endif // REVOLVE_NEAT_NEATEVALUATION_H_
