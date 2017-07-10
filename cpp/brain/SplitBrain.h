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

template <typename Phenotype, typename Genotype>
class SplitBrain
        : public Brain
{
public:

    virtual ~SplitBrain() {};

protected:

    /// control responsible for the movement of the robot
    boost::shared_ptr<Controller<Phenotype>> controller_;

    /// learner used to get new genomes
    boost::shared_ptr<Learner<Genotype>> learner_;
};

}
}

#endif // REVOLVEBRAIN_BRAIN_SPLITBRAIN_H_
