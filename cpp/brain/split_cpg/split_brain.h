/*
 * Specifies a utility `SplitBrain` base class. Here the learner and controller are separeted explicitly.
 */

#ifndef REVOLVE_BRAIN_SPLIT_BRAIN_H_
#define REVOLVE_BRAIN_SPLIT_BRAIN_H_

#include "controller.h"
#include "learner.h"

namespace revolve {
namespace brain {

template <typename G>
class SplitBrain {
public:
    /**
     * Constructor for a split brain
     * @param controller: controller used
     * @param learner: learner used
     * @return pointer to the brain
     */
    SplitBrain() {};
    virtual ~SplitBrain() {};

    
protected:
    boost::shared_ptr<Controller<G>> controller;	//control unit responsible for the movement of the robot
    boost::shared_ptr<Learner<G>> learner; 		//learner used to get new genomes
};

}
}

#endif /* REVOLVE_BRAIN_SPLIT_BRAIN_H_ */
