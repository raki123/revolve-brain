//
// Created by matteo on 09/11/16.
//

#ifndef REVOLVE_BRAIN_CPGNETWORK_H
#define REVOLVE_BRAIN_CPGNETWORK_H

#include "RythmGenerationNeuron.h"
#include "PatternFormationNeuron.h"
#include "MotoNeuron.h"
#include <memory>

namespace revolve {
namespace brain {
namespace cpg {

class CPGNetwork {
public:
    struct Weights {
        cpg::real_t we;
        cpg::real_t wf;
    };

public:
    CPGNetwork(unsigned int n_sensors, unsigned int n_connections);

    virtual ~CPGNetwork();

    /**
     * @brief calculates next output of the network
     *
     * Updates the network to the new steps and returns the next result
     *
     * @param sensor_readings vector containing sensor readings
     * @param step time passed since last update
     * @return revolve::brain::cpg::real_t output for the network after the update
     */
    real_t update(const std::vector<real_t> &sensor_readings, double step);

    // GETTERS and SETTERS

    // Genome getter and setters
    std::shared_ptr<std::vector<real_t>> get_genome();
    const std::shared_ptr<const std::vector<real_t>> get_genome() const;
    void set_genome(std::vector<real_t>);
    void update_genome();

    struct Limit {
        real_t lower;
        real_t upper;
        Limit(real_t l, real_t u)
            : lower(l), upper(u) {}
        Limit()
            : lower(std::numeric_limits<real_t>::lowest())
            , upper(std::numeric_limits<real_t>::max())
        {}
        Limit &operator=(const Limit& o) {
            this->upper = o.upper;
            this->lower = o.lower;
            return *this;
        }
    };

    const std::vector<Limit> &get_genome_limits();

    void addConnection(CPGNetwork* new_connection) {
        connections.push_back(new_connection);
    }


protected:
    void updateRythmGeneration(double step);
    void updatePatternFormation(const std::vector<real_t> &sensor_readings, double step);
    void updateMotoNeuron(double step);

protected:

    RythmGenerationNeuron *rge;
    RythmGenerationNeuron *rgf;
    PatternFormationNeuron *pfe;
    PatternFormationNeuron *pff;
    MotoNeuron *mn;

    real_t rge_out;
    real_t rgf_out;
    real_t pfe_out;
    real_t pff_out;
    real_t mn_out;

    const unsigned int n_connections;
    std::vector<CPGNetwork*> connections;

    std::shared_ptr<std::vector<real_t>> genome;
    std::vector<Limit> genome_limits;
};

}
}
}



#endif //REVOLVE_BRAIN_CPGNETWORK_H
