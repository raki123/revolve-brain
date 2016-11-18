/*
 * Specifies a utility `Learner` base class, which is supposed to change the genome of the robot.
 */

#ifndef REVOLVE_BRAIN_SPLIT_BRAIN_LEARNER_H_
#define REVOLVE_BRAIN_SPLIT_BRAIN_LEARNER_H_

#include <string>

namespace revolve {
namespace brain {

template <typename G>
class Learner {
public:
    /**
     * Constuctor for the Learner
     * @return pointer to the learner
     */
    Learner() =0;
    virtual ~Learner() {};
    
    /**
     * Method to report the fitness of a robot
     * @param id: identifier of the robot (in case there are multiple ones)
     * @param G: genome that was tested on the robot
     * @param fitness: value of the fitness evaluation
     */   
    virtual void reportFitness(std::string id, 
			       G genome,
			       double fitness) = 0;
			       
    /**
     * Get new genome for robot
     * reportFitness should be called first so the learner can make a more informed descision
     * @param id: identifier of the robot (in case there are multiple ones)
     * @return new genome
     */
    virtual G newGenome(std::string id) =0;

};

}
}

#endif /* REVOLVE_BRAIN_SPLIT_BRAIN_LEARNER_H_ */
