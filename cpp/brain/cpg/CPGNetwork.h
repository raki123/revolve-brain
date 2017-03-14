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

    // setting raw values
    void setRGEWeight(real_t value) {rge->setWeight(value);}
    void setRGFWeight(real_t value) {rgf->setWeight(value);}
    void setRGEWeightNeighbour(real_t value, size_t index) {rge->setWeightNeighbour(value, index);}
    void setRGFWeightNeighbour(real_t value, size_t index) {rgf->setWeightNeighbour(value, index);}
    void setRGEC(real_t value) {rge->setC(value);}
    void setRGFC(real_t value) {rgf->setC(value);}
    void setRGEAmplitude(real_t value) {rge->setAmplitude(value);}
    void setRGFAmplitude(real_t value) {rgf->setAmplitude(value);}
    void setRGEOffset(real_t value) {rge->setOffset(value);}
    void setRGFOffset(real_t value) {rgf->setOffset(value);}
    void setPFEAlpha(real_t value) {pfe->setAlpha(value);}
    void setPFFAlpha(real_t value) {pff->setAlpha(value);}
    void setPFETheta(real_t value) {pfe->setTheta(value);}
    void setPFFTheta(real_t value) {pff->setTheta(value);}


    // setting percentage values
    void setRGEWeightPercentage(real_t value) {rge->setWeightPercentage(value);}
    void setRGFWeightPercentage(real_t value) {rgf->setWeightPercentage(value);}
    void setRGEWeightNeighbourPercentage(real_t value, size_t index) {rge->setWeightNeighbourPercentage(value, index);}
    void setRGFWeightNeighbourPercentage(real_t value, size_t index) {rgf->setWeightNeighbourPercentage(value, index);}
    void setRGECPercentage(real_t value) {rge->setCPercentage(value);}
    void setRGFCPercentage(real_t value) {rgf->setCPercentage(value);}
    void setRGEAmplitudePercentage(real_t value) {rge->setAmplitudePercentage(value);}
    void setRGFAmplitudePercentage(real_t value) {rgf->setAmplitudePercentage(value);}
    void setRGEOffsetPercentage(real_t value) {rge->setOffsetPercentage(value);}
    void setRGFOffsetPercentage(real_t value) {rgf->setOffsetPercentage(value);}
    void setPFEAlphaPercentage(real_t value) {pfe->setAlphaPercentage(value);}
    void setPFFAlphaPercentage(real_t value) {pff->setAlphaPercentage(value);}
    void setPFEThetaPercentage(real_t value) {pfe->setThetaPercentage(value);}
    void setPFFThetaPercentage(real_t value) {pff->setThetaPercentage(value);}

protected:
    void updateRythmGeneration(real_t step);
    void updatePatternFormation(const std::vector<real_t> &sensor_readings, real_t step);
    void updateMotoNeuron(real_t step);

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
