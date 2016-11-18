/*
 * Specifies a utility `Contoller` base class, which is supposed to control the robot according to a given genome.
 */

#ifndef REVOLVE_BRAIN_SPLIT_BRAIN_CONTROLLER_H_
#define REVOLVE_BRAIN_SPLIT_BRAIN_CONTROLLER_H_

namespace revolve {
namespace brain {

template <typename G>
class Controller {
public:
    /**
     * Constuctor for the Controller
     * @param genome: genome specifying the way the the robot is controlled
     * @return pointer to the controller
     */
    Controller(G genome) : genome(genome) {};
    virtual ~Controller() {};
    
    /**
     * Get method for the genome
     * @return current genome
     */
    virtual G getGenome() { return genome; };
    
    /**
     * Set method for the genome
     * @param newGenome: new genome to use instead of the old one
     */
    virtual void setGenome(G newGenome) { this->genome = newGenome; };
    
    /**
     * Update step called for the controller.
     *
     * @param actuators List of actuators
     * @param sensors List of sensors
     * @param t Current simulation time
     * @param step Actuation step size in seconds
     */
    virtual void update(const std::vector< ActuatorPtr > & actuators,
                        const std::vector< SensorPtr > & sensors,
                        double t,
                        double step) = 0;
    
protected:
     G genome; //genome used to control the robot
    
};

}
}

#endif /* REVOLVE_BRAIN_SPLIT_BRAIN_CONTROLLER_H_ */
