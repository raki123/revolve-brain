#include "weight_vector_learner.h"
#include <random>

namespace revolve {
namespace brain {

WeightVectorLearner::WeightVectorLearner() 
{
	
}

void WeightVectorLearner::reportFitness(std::string id, 
					std::vector<double> genome,
					double fitness) 
{
    oldWeights = genome;
}
			       
std::vector<double> WeightVectorLearner::newGenome(std::string id) 
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::normal_distribution<double> dist(0, 0.1);
    std::vector<double> newOne(oldWeights.size(), 0);
    for(unsigned int i = 0; i < oldWeights.size(); i++) {
	 newOne[i] = oldWeights[i] + dist(mt);
    }
    return newOne;
}

    
}
}

