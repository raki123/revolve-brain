
#include "SUPGBrain.h"
#include "neat/AsyncNEAT.h"
#include <sstream>
#include <stdexcept>
#include <limits>
#include <iomanip>
#include <string>
#include <ctime>

using namespace revolve::brain;

const char* SUPGBrain::getVARenv(const char* var_name)
{
    const char* env_p = std::getenv(var_name);
    if(env_p) {
        std::cout << "ENV " << var_name << " is: " << env_p << std::endl;
    } else {
        std::cout << "ENV " << var_name << " not found, using default value: ";
    }
    return env_p;
}

long SUPGBrain::GetMAX_EVALUATIONSenv()
{
    if(const char* env_p = SUPGBrain::getVARenv("SUPG_MAX_EVALUATIONS")) {
        //TODO catch exception
        long value = std::stol(env_p);
        return value;
    }
    std::cout << -1 << std::endl;
    return -1;
}


double SUPGBrain::GetFREQUENCY_RATEenv()
{
    if(const char* env_p = SUPGBrain::getVARenv("SUPG_FREQUENCY_RATE")) {
        //TODO catch exception
        double value = std::stod(env_p);
        return value;
    }
    std::cout << 30 << std::endl;
    return 30;
}

double SUPGBrain::GetCYCLE_LENGTHenv()
{
    if(const char* env_p = SUPGBrain::getVARenv("SUPG_CYCLE_LENGTH")) {
        //TODO catch exception
        double value = std::stod(env_p);
        return value;
    }
    std::cout << 5 << std::endl;
    return 5;
}

revolve::brain::SUPGBrain::SUPGBrain(EvaluatorPtr evaluator)
  : evaluator(evaluator)
  , start_eval_time(std::numeric_limits< double >::lowest())
  , generation_counter(0)
  , MAX_EVALUATIONS(GetMAX_EVALUATIONSenv())
  , FREQUENCY_RATE(GetFREQUENCY_RATEenv())
  , CYCLE_LENGTH(GetCYCLE_LENGTHenv())
  , neuron_coordinates(0)
{
}


SUPGBrain::SUPGBrain(const std::string &robot_name,
                     EvaluatorPtr evaluator,
                     const std::vector< std::vector< float > > &neuron_coordinates,
                     const std::vector< ActuatorPtr >& actuators,
                     const std::vector< SensorPtr >& sensors)
  : evaluator(evaluator)
  , robot_name(robot_name)
  , start_eval_time(std::numeric_limits< double >::lowest())
  , neuron_coordinates(neuron_coordinates)
  , generation_counter(0)
  , MAX_EVALUATIONS(GetMAX_EVALUATIONSenv())
  , FREQUENCY_RATE(GetFREQUENCY_RATEenv())
  , CYCLE_LENGTH(GetCYCLE_LENGTHenv())
{
    if (actuators.size() != neuron_coordinates.size()) {
        std::stringstream ss;
        ss << "actuator size [" << actuators.size() << "] and neuron coordinates size [" << neuron_coordinates.size() << "] are different!";
        std::cerr<<"THROWING EXCEPTION:\n"<<ss.str()<<std::endl;
        throw std::invalid_argument( ss.str());
    }

    unsigned int p = 0;
    std::cout<<"sensor->sensorId() ["<<sensors.size()<<" sensors]"<<std::endl;
    for (auto sensor : sensors) {
        std::cout << "sensor: " << sensor->sensorId() << "(inputs: " << sensor->inputs() << ")" << std::endl;
        p += sensor->inputs();
    }
    std::cout<<"END sensor->sensorId()"<<std::endl;
    n_inputs = p;

    p = 0;
    for (auto actuator : actuators) {
        p += actuator->outputs();
    }
    n_outputs = p;

    this->init_async_neat();
}

void SUPGBrain::init_async_neat() {
    std::unique_ptr< AsyncNeat > neat(new AsyncNeat(
        SUPGNeuron::GetDimensionInput(n_inputs, neuron_coordinates[0].size()),
        SUPGNeuron::GetDimensionOutput(n_outputs),
        std::time(0),
        robot_name
    ));
    this->neat = std::move(neat);
}


void SUPGBrain::update(const std::vector< ActuatorPtr >& actuators,
                       const std::vector< SensorPtr >& sensors,
                       double t, double step)
{
    this->update<std::vector<ActuatorPtr>, std::vector<SensorPtr>>(actuators, sensors, t, step);
}

double SUPGBrain::getFitness()
{
    //Calculate fitness for current policy
    double fitness = evaluator->fitness();
    std::cout << "Evaluating gait, fitness = " << fitness << std::endl;
    return fitness;
}

#include <string>
#include <fstream>
#include <iostream>
#include <innovgenome/innovgenome.h>


void SUPGBrain::nextBrain()
{
    bool init_supgs;
    unsigned int how_many_neurons;
    if (!current_evalaution) {
        // first evaluation
        init_supgs = true;
        how_many_neurons = neuron_coordinates.size();
    } else {
        // normal change of evaluation
        init_supgs = false;
        how_many_neurons = neurons.size();
        current_evalaution->finish(getFitness());

//         // temporary genome save & load test
//         {
//             std::fstream genome_save;
//             genome_save.open("/tmp/genome.yaml", std::ios::out);
//             current_evalaution->getOrganism()->genome->save(genome_save);
//         }

//         {
//             std::fstream genome_load;
//             genome_load.open("/tmp/genome.yaml", std::ios::in);
//             NEAT::InnovGenome genome;
//             genome.load(genome_load);
//         }
    }

    current_evalaution = neat->getEvaluation();
    NEAT::CpuNetwork *cppn = reinterpret_cast< NEAT::CpuNetwork* > (
        current_evalaution->getOrganism()->net.get()
    );

    for (unsigned int i=0; i< how_many_neurons; i++) {
        if (init_supgs)
            neurons.push_back(std::unique_ptr<SUPGNeuron>(new SUPGNeuron(cppn, neuron_coordinates[i], CYCLE_LENGTH)));
        else
            neurons[i]->setCppn(cppn);
    }
}

void SUPGBrain::learner(double t)
{
    // Evaluate policy on certain time limit
    if (!this->isOffline()
            && (t-start_eval_time) > SUPGBrain::FREQUENCY_RATE)
    {
        // check if to stop the experiment. Negative value for MAX_EVALUATIONS will never stop the experiment
        if (SUPGBrain::MAX_EVALUATIONS > 0 && generation_counter > SUPGBrain::MAX_EVALUATIONS) {
            std::cout << "Max Evaluations (" << SUPGBrain::MAX_EVALUATIONS << ") reached. stopping now." << std::endl;
            std::exit(0);
        }
        generation_counter++;
        std::cout << "################# EVALUATING NEW BRAIN !!!!!!!!!!!!!!!!!!!!!!!!! (generation " << generation_counter << " )" << std::endl;
        this->nextBrain();
        start_eval_time = t;
        evaluator->start();
    }
}
