//
// Created by matteo on 09/11/16.
//

#include "CPGNetwork.h"
#include <iostream>

using namespace revolve::brain::cpg;

CPGNetwork::CPGNetwork(unsigned int n_sensors, unsigned int n_connections)
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
    , n_connections(n_connections)
{
    std::vector<real_t> weight_neigbours_e(n_connections, 0);
    std::vector<real_t> weight_neigbours_f(n_connections, 0);
//     for (int i=0; i<weight_neigbours.size(); i++) {
//         weight_neigbours_e[i] = weight_neigbours[i].we;
//         weight_neigbours_f[i] = weight_neigbours[i].wf;
//     }

    // RG
    rge = new cpg::RythmGenerationNeuron(1,weight_neigbours_e, .5,1,0);
    rgf = new cpg::RythmGenerationNeuron(1,weight_neigbours_f,-.5,1,0);

    // PF
    std::vector<cpg::real_t> pfe_weights(1+n_sensors, 0.0001);
    std::vector<cpg::real_t> pff_weights(1+n_sensors, 0.0001);
    pfe_weights[n_sensors] = pff_weights[n_sensors] = 1;
    pfe = new cpg::PatternFormationNeuron(pfe_weights);
    pff = new cpg::PatternFormationNeuron(pff_weights);

    // MN
    mn = new cpg::MotoNeuron(1);

    genome_limits = {
        // Rythm Generation
        { rge->WEIGHT_MIN,    rge->WEIGHT_MAX },
        { rge->C_MIN,         rge->C_MAX },
        { rge->AMPLITUDE_MIN, rge->AMPLITUDE_MAX },
        { rge->OFFSET_MIN,    rge->OFFSET_MAX },

        { rgf->WEIGHT_MIN,    rgf->WEIGHT_MAX },
        { rgf->C_MIN,         rgf->C_MAX },
        { rgf->AMPLITUDE_MIN, rgf->AMPLITUDE_MAX },
        { rgf->OFFSET_MIN,    rgf->OFFSET_MAX },

        // Pattern formation
        { pfe->ALPHA_MIN,     pfe->ALPHA_MAX },
        { pfe->THETA_MIN,     pfe->THETA_MAX },

        { pff->ALPHA_MIN,     pff->ALPHA_MAX },
        { pff->THETA_MIN,     pff->THETA_MAX },

        // MotoNeuron
        //NONE
    };

    for (size_t i=0; i<n_connections; ++i) {
        genome_limits.push_back({rge->WEIGHT_MIN, rge->WEIGHT_MAX});
        genome_limits.push_back({rgf->WEIGHT_MIN, rgf->WEIGHT_MAX});
    }

//     std::vector<real_t>_genome = {
//         // Rythm Generation
//         1, // rge weight
//         0, // rge phase
//         1, // rge amplitude
//         0, // rge offset
//
//         1, // rgf weight
//         0, // rgf phase
//         1, // rgf amplitude
//         0, // rgf offset
//
//         // Pattern formation
//         1, // pfe alpha
//         0, // pfe theta
//
//         1, // pff alpha
//         0, // pff theta
//
//         // MotoNeuron
//         //NONE
//     };

    // percentage version
    std::vector<real_t>_genome = std::vector<real_t>(12 + 2*n_connections, 0.5);

    genome = std::make_shared<std::vector<real_t>>(_genome);
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
    real_t r_step = (real_t) step;
    updateRythmGeneration(r_step);
    updatePatternFormation(sensor_readings, r_step);
    updateMotoNeuron(r_step);

    return mn_out;
}

void CPGNetwork::updateRythmGeneration(real_t step)
{
    real_t phi_e = rge->getPhi();
    real_t phi_f = rgf->getPhi();

    std::vector<real_t> inputs_e = { phi_f };
    std::vector<real_t> inputs_f = { phi_e };

    for (size_t i=0; i<n_connections; ++i) {
        inputs_e.push_back(connections[i]->rge->getPhi());
        inputs_f.push_back(connections[i]->rgf->getPhi());
    }

    rge_out = rge->update(inputs_e, step)[0];
    rgf_out = rgf->update(inputs_f, step)[0];
}

void CPGNetwork::updatePatternFormation(const std::vector<real_t> &sensor_readings, real_t step)
{
    std::vector<real_t> pfe_inputs = std::vector<real_t>(sensor_readings);
    std::vector<real_t> pff_inputs = std::vector<real_t>(sensor_readings);
    pfe_inputs.push_back(rge_out);
    pff_inputs.push_back(rgf_out);

    try {
        pfe_out = pfe->update(pfe_inputs, step)[0];
        pff_out = pff->update(pff_inputs, step)[0];
    } catch (const std::exception& e) {
        std::cerr<<"exception!! "<<e.what()<<std::endl;
        throw;
    }
}

void CPGNetwork::updateMotoNeuron(real_t step)
{
    mn_out = mn->update({pfe_out, pff_out}, step)[0];
}

// GENOME MANAGEMENT ----------------------------------------------------------
std::shared_ptr<std::vector<real_t>> CPGNetwork::get_genome() {
    return genome;
}

const std::shared_ptr<const std::vector<real_t>> CPGNetwork::get_genome() const {
    return genome;
}

const std::vector<CPGNetwork::Limit> &CPGNetwork::get_genome_limits() {
    return genome_limits;
}

void revolve::brain::cpg::CPGNetwork::set_genome(std::vector<real_t> other)
{
    std::shared_ptr<std::vector<real_t>> other_p =
        std::make_shared<std::vector<real_t>>(other);
    this->genome.swap(other_p);
    try {
        update_genome();
    } catch (const std::exception &e) {
        this->genome.swap(other_p);
        throw;
    }
}

void revolve::brain::cpg::CPGNetwork::update_genome()
{
    assert(genome->size() == (12 + 2*n_connections));
    size_t i=0;

    std::cout << "new parameters: {";

//     // Rythm generator
//     rge->setWeight((*genome)[i++]);
//     rge->setC((*genome)[i++]);
//     rge->setAmplitude((*genome)[i++]);
//     rge->setOffset((*genome)[i++]);
//
//     rgf->setWeight((*genome)[i++]);
//     rgf->setC((*genome)[i++]);
//     rgf->setAmplitude((*genome)[i++]);
//     rgf->setOffset((*genome)[i++]);
//
//     // Pattern Formation
//     pfe->setAlpha((*genome)[i++]);
//     pfe->setTheta((*genome)[i++]);
//
//     pff->setAlpha((*genome)[i++]);
//     pff->setTheta((*genome)[i++]);
//
//    // MotoNeuron
//    //NONE

    // Rythm generator
    std::cout << "\"weight_e\": " <<
    rge->setWeightPercentage((*genome)[i++]);    //0
//     std::cout << "\",c_e\": " <<
    i++;//rge->setCPercentage((*genome)[i++]);   //1
    std::cout << "\",amplitude_e\": " <<
    rge->setAmplitudePercentage((*genome)[i++]); //2
    std::cout << "\",offset_e\": " <<
    rge->setOffsetPercentage((*genome)[i++]);    //3

    std::cout << "\",weight_f\": " <<
    rgf->setWeightPercentage((*genome)[i++]);    //4
//     std::cout << "\",c_f\": " <<
    i++;//rgf->setCPercentage((*genome)[i++]);   //5
    std::cout << "\",amplitude_f\": " <<
    rgf->setAmplitudePercentage((*genome)[i++]); //6
    std::cout << "\",offset_f\": " <<
    rgf->setOffsetPercentage((*genome)[i++]);    //7

    // Pattern Formation
//     std::cout << "\",alpha_e\": " <<
    i++;//pfe->setAlphaPercentage((*genome)[i++]); //8
//     std::cout << "\",theta_e\": " <<
    i++;//pfe->setThetaPercentage((*genome)[i++]); //9

//     std::cout << "\",alpha_f\": " <<
    i++;//pff->setAlphaPercentage((*genome)[i++]); //10
//     std::cout << "\",theta_f\": " <<
    i++;//pff->setThetaPercentage((*genome)[i++]); //11

    // MotoNeuron
    //NONE

    // Connection Weights
    for (size_t j=0; j<n_connections; j++) { //12 + 2*j
        rge->setWeightNeighbourPercentage((*genome)[i++], j);
        rgf->setWeightNeighbourPercentage((*genome)[i++], j);
    }

    std::cout << '}' << std::endl;
}
