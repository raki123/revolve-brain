/*
 * Specifies a utility `SplitBrain` base class. Here the learner and controller are separeted explicitly.
 */

#ifndef REVOLVE_BRAIN_SPLIT_BRAIN_H_
#define REVOLVE_BRAIN_SPLIT_BRAIN_H_

#include "../brain.h"
#include "controller.h"
#include "learner.h"

namespace revolve {
namespace brain {

template <typename G>
class SplitBrain : public Brain {
public:
    /**
     * Constructor for a split brain
     * @param controller: controller used
     * @param learner: learner used
     * @return pointer to the brain
     */
    SplitBrain(Controller<G> &controller,
	       Learner<G> &learner);
    virtual ~SplitBrain() {};

    
protected:
    Controller<G> controller;	//control unit responsible for the movement of the robot
    Learner<G> learner; 	//learner used to get new genomes
};

}
}

#endif /* REVOLVE_BRAIN_SPLIT_BRAIN_H_ */
