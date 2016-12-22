/*
 * Specifies a utility `SplitBrain` base class. Here the learner and controller are separeted explicitly.
 */

#ifndef REVOLVE_BRAIN_SPLIT_BRAIN_H_
#define REVOLVE_BRAIN_SPLIT_BRAIN_H_

#include "controller/controller.h"
#include "learner/learner.h"
#include "brain.h"

namespace revolve {
namespace brain {

template <typename G, typename H>
class SplitBrain : public Brain {
public:
    virtual ~SplitBrain() {};

    
protected:
    boost::shared_ptr<Controller<G>> controller;	//control unit responsible for the movement of the robot
    boost::shared_ptr<Learner<H>> learner; 		//learner used to get new genomes
};

}
}

#endif /* REVOLVE_BRAIN_SPLIT_BRAIN_H_ */
