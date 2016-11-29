#include "testneatbrain.h"

#include "brain/basic_neat_brain.h"
#include <iostream>

int main(int argc, char *argv[]) {
    std::cout << "testing neat brain" << std::endl;
    TestNEATBrain test;
}

TestNEATBrain::TestNEATBrain()
{
    revolve::brain::BasicBrain brain(nullptr, 6,6);
    double t = 0;
    std::vector< revolve::brain::ActuatorPtr > empty1;
     std::vector< revolve::brain::SensorPtr > empty2;
    while(true) {
	t++;
	brain.update(empty1,
                     empty2,
                     t, 1);
    }
}
