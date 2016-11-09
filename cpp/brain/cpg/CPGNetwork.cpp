//
// Created by matteo on 09/11/16.
//

#include "CPGNetwork.h"

using namespace revolve::brain::cpg;

CPGNetwork::CPGNetwork(unsigned int n_sensors)
    : rge(nullptr)
    , rgf(nullptr)
    , pfe(nullptr)
    , pff(nullptr)
    , mn(nullptr)
    , rge_out(0)
    , rgf_out(0)
    , pfe_out(0)
    , pff_out(0)
    , mn_out(0)
{
    // RG
    rge = new cpg::RythmGenerationNeuron(1,1,1,0);
    rgf = new cpg::RythmGenerationNeuron(1,1,1,0);

    // PF
    std::vector<cpg::real_t> pfe_weights(1+n_sensors, 1);
    std::vector<cpg::real_t> pff_weights(1+n_sensors, 1);
    pfe = new cpg::PatternFormationNeuron(pfe_weights);
    pff = new cpg::PatternFormationNeuron(pff_weights);

    // MN
    mn = new cpg::MotoNeuron(1);
}

CPGNetwork::~CPGNetwork()
{
    delete rge;
    delete rgf;
    delete pfe;
    delete pff;
    delete mn;
}

real_t CPGNetwork::update(const std::vector<real_t> &sensor_readings, double step)
{
    updateRythmGeneration(step);
    updatePatternFormation(step);
    updateMotoNeuron(step);

    return mn_out;
}

void CPGNetwork::updateRythmGeneration(double step)
{
    real_t phi_e = rge->getPhi();
    real_t phi_f = rgf->getPhi();

    rge_out = rge->update({phi_f}, step)[0];
    rgf_out = rgf->update({phi_e}, step)[0];
}

void CPGNetwork::updatePatternFormation(double step)
{
    pfe_out = pfe->update({rge_out}, step)[0];
    pff_out = pff->update({rgf_out}, step)[0];
}

void CPGNetwork::updateMotoNeuron(double step)
{
    mn_out = mn->update({pfe_out, pff_out}, step)[0];
}
