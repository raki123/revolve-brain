//
// Created by matteo on 09/11/16.
//

#ifndef REVOLVE_BRAIN_CPGNETWORK_H
#define REVOLVE_BRAIN_CPGNETWORK_H

#include "RythmGenerationNeuron.h"
#include "PatternFormationNeuron.h"
#include "MotoNeuron.h"

namespace revolve {
namespace brain {
namespace cpg {

class CPGNetwork {
public:
    CPGNetwork(unsigned int n_sensors);

    virtual ~CPGNetwork();

    real_t update(const std::vector<real_t> &sensor_readings, double step);

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
};

}
}
}



#endif //REVOLVE_BRAIN_CPGNETWORK_H
