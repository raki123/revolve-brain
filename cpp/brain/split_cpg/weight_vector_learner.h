#ifndef REVOLVE_LEARNER_WEIGHT_VECTOR_H_
#define REVOLVE_LEARNER_WEIGHT_VECTOR_H_

#include "learner.h"
#include <vector>


namespace revolve {
namespace brain {

class WeightVectorLearner : public Learner<std::vector<double>>
{
public:
    WeightVectorLearner();
    virtual ~WeightVectorLearner() {};
    
    /**
     * Method to report the fitness of a robot
     * @param id: identifier of the robot (in case there are multiple ones)
     * @param genome: genome that was tested on the robot
     * @param fitness: value of the fitness evaluation
     */   
    virtual void reportFitness(std::string id, 
			       std::vector<double> genome,
			       double fitness);
			       
    /**
     * Get new genome for robot
     * reportFitness should be called first so the learner can make a more informed descision
     * @param id: identifier of the robot (in case there are multiple ones)
     * @return new genome
     */
    virtual std::vector<double> getNewGenome(std::string id);
protected:
    std::vector<double> oldWeights;
    std::vector<double> newWeights; 
    double oldFitness = 0;
};

}
}

#endif /* REVOLVE_LEARNER_WEIGHT_VECTOR_H_ */
