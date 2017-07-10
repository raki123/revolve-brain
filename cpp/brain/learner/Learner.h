/*
 * Specifies a utility `Learner` base class, which is supposed to change the genome of the robot.
 */

#ifndef REVOLVEBRAIN_BRAIN_LEARNER_BRAINLEARNER_H_
#define REVOLVEBRAIN_BRAIN_LEARNER_BRAINLEARNER_H_

#include <string>

namespace revolve {
namespace brain {

template <typename Genotype>
class Learner
{
public:

    virtual ~Learner()
    {};

    /// \brief Method to report the fitness of a robot
    /// \param[in] id: identifier of the robot (in case there are multiple ones)
    /// \param[in] genome: genome that was tested on the robot
    /// \param[in] fitness: value of the fitness evaluation
    virtual void reportFitness(std::string id,
                               Genotype genotype,
                               double fitness) = 0;


    /// \brief Getter for a new robot genotype
    /// \note reportFitness should be called first so the learner can make a more informed decision
    /// \param[in] id: identifier of the robot (in case there are multiple ones)
    /// \return new genome
    virtual Genotype currentGenotype() = 0;

};

}
}

#endif // REVOLVEBRAIN_BRAIN_LEARNER_BRAINLEARNER_H_
