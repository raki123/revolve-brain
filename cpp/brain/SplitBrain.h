/*
 * Specifies a utility `SplitBrain` base class. Here the learner and controller are separeted explicitly.
 */

#ifndef REVOLVEBRAIN_BRAIN_SPLITBRAIN_H_
#define REVOLVEBRAIN_BRAIN_SPLITBRAIN_H_

#include "Brain.h"
#include "brain/controller/Controller.h"
#include "brain/learner/Learner.h"

namespace revolve {
namespace brain {

template <typename G, typename H>
class SplitBrain
        : public Brain
{
public:
    virtual ~SplitBrain()
    {};


protected:
    boost::shared_ptr<Controller<G>> controller;    //control unit responsible for the movement of the robot
    boost::shared_ptr<Learner<H>> learner;        //learner used to get new genomes
};

}
}

#endif // REVOLVEBRAIN_BRAIN_SPLITBRAIN_H_
