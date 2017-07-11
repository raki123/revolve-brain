//
// Created by matteo on 3/15/17.
//

#ifndef REVOLVE_BRAIN_GENERICLEARNERBRAIN_H
#define REVOLVE_BRAIN_GENERICLEARNERBRAIN_H

#include "brain/learner/BaseLearner.h"
#include "Brain.h"

namespace revolve { namespace brain {

    class GenericLearnerBrain : public Brain {
    public:

      using Brain::update;

        GenericLearnerBrain(std::unique_ptr<BaseLearner> learner);
        GenericLearnerBrain(BaseLearner *learner);

        void update(const std::vector<ActuatorPtr> &actuators, const std::vector<SensorPtr> &sensors, double t,
                    double step) override;

    private:
        std::unique_ptr<BaseLearner> learner;
    };

}}


#endif //REVOLVE_BRAIN_GENERICLEARNERBRAIN_H}}

