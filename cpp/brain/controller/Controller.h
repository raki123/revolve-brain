/*
 * Specifies a utility `Contoller` base class, which is supposed to control the robot according to a given genome.
 */

#ifndef REVOLVEBRAIN_BRAIN_CONTROLLER_CONTROLLER_H_
#define REVOLVEBRAIN_BRAIN_CONTROLLER_CONTROLLER_H_

#include "brain/Actuator.h"
#include "brain/Sensor.h"
#include <vector>


namespace revolve {
namespace brain {

template <typename Phenotype>
class Controller
{
public:

    virtual ~Controller()
    {};

    /**
     * Get method for the genome
     * @return current genome
     */
    virtual Phenotype getPhenotype() = 0;

    /**
     * Set method for the phenome
     * @param newGenome: new genome to use instead of the old one
     */
    virtual void setPhenotype(Phenotype phenotype) = 0;

    /**
     * Update step called for the controller.
     *
     * @param actuators List of actuators
     * @param sensors List of sensors
     * @param t Current simulation time
     * @param step Actuation step size in seconds
     */
    virtual void update(const std::vector<ActuatorPtr> &actuators,
                        const std::vector<SensorPtr> &sensors,
                        double t,
                        double step) = 0;


};

}
}

#endif // REVOLVEBRAIN_BRAIN_CONTROLLER_CONTROLLER_H_
