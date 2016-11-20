#ifndef REVOLVE_BRAIN_SIMPLE_SPLIT_BRAIN_H_
#define REVOLVE_BRAIN_SIMPLE_SPLIT_BRAIN_H_

#include "split_brain.h"

namespace revolve {
namespace brain {


template <typename G>
class SimpleSplitBrain : public SplitBrain<G> {
public:
    SimpleSplitBrain() {};
    virtual ~SimpleSplitBrain() {};
    
    /**
     * Update step called for the brain.
     * @param actuators List of actuators
     * @param sensors List of sensors
     * @param t Current simulation time
     * @param step Actuation step size in seconds
     */
    virtual void update(const std::vector< ActuatorPtr > & actuators,
                        const std::vector< SensorPtr > & sensors,
                        double t,
                        double step);

    
protected:
    boost::shared_ptr<Controller<G>> controller = NULL;	//control unit responsible for the movement of the robot
    boost::shared_ptr<Learner<G>> learner = NULL; 	//learner used to get new genomes
    double start_eval_time_ = 0;
    double evaluation_rate_ = 30;
    int generation_counter_ = 0;
    int max_evaluations_ = 1000;
};

}
}

#endif /* REVOLVE_BRAIN_SIMPLE_SPLIT_BRAIN_H_ */
