#include "weight_vector_learner.h"
#include <random>
#include <iostream>

using namespace std;


namespace revolve {
namespace brain {
  




WeightVectorLearner::WeightVectorLearner() 
{
  
}

void WeightVectorLearner::reportFitness(std::string id, 
					std::vector<double> genome,
					double fitness) 
{
    if(fitness > oldFitness) {
	  oldFitness = fitness;
	  oldWeights = genome;
	  for(auto it: oldWeights) {
	    std::cout << it << ",";
	    }
	    std::cout << std::endl;
    }
    std::random_device rd;
    std::mt19937 mt(rd());
    std::normal_distribution<double> dist(0, 1);
    std::vector<double> newOne(oldWeights.size(), 0);
    for(unsigned int i = 0; i < oldWeights.size(); i++) {
	 newOne[i] = oldWeights[i] + dist(mt);
    }
    newWeights = newOne;
}
			       
std::vector<double> WeightVectorLearner::getNewGenome(std::string id) 
{
    return newWeights;
}

}
}


