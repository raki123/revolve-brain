#ifndef TESTSUPGBRAIN_H
#define TESTSUPGBRAIN_H

#include "brain/SUPGBrain.h"
#include <vector>
#include <string>

class TestSUPGBrain : public revolve::brain::SUPGBrain
{
public:
    TestSUPGBrain(revolve::brain::EvaluatorPtr evaluator,
                  const std::vector< std::vector< float > > &neuron_coordinates,
                  const std::vector< revolve::brain::ActuatorPtr >& actuators,
                  const std::vector< revolve::brain::SensorPtr >& sensors);


    void test_update(const std::vector<revolve::brain::ActuatorPtr> &actuators,
                     const std::vector<revolve::brain::SensorPtr> &sensors,
                     double t, double step);
};


#endif // TESTSUPGBRAIN_H
