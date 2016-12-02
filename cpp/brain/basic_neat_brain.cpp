#include "basic_neat_brain.h"
#include "neat/asyncneat.h"
#include <sstream>
#include <stdexcept>
#include <limits>
#include <iomanip>
#include <string>
#include <ctime>
#include <fstream>

using namespace revolve::brain;


BasicBrain::BasicBrain(EvaluatorPtr evaluator,
                     unsigned int n_actuators,
                     unsigned int n_sensors,
		     NEAT::InnovGenome::GenomeConfig startConfig)
  : evaluator(evaluator)
  , start_eval_time(std::numeric_limits< double >::lowest())
  , generation_counter(0)
  , current_evalaution(NULL)
  , run_count(1)
  , reset(true)
  , resetDuration(5)
  , evalRunning(false)

{

    n_inputs = n_sensors;

    n_outputs = n_actuators;
    this->init_async_neat(startConfig);
    std::cout << "brain initailized\n";
}

void BasicBrain::init_async_neat(NEAT::InnovGenome::GenomeConfig startConfig) {
    AsyncNeat::Init();
    unsigned long populationSize = 10;
    NEAT::GeneticSearchType geneticSearchType = NEAT::GeneticSearchType::PHASED;


    AsyncNeat::SetPopulationSize(populationSize); // 10 - 25 - 50 - 75 - 100 - 1000
    AsyncNeat::SetSearchType(geneticSearchType);
    std::unique_ptr< AsyncNeat > neat(new AsyncNeat(
        n_inputs,
        n_outputs,
        std::time(0),
	startConfig
    ));

    this->neat = std::move(neat);
}


void BasicBrain::update(const std::vector< ActuatorPtr >& actuators,
                       const std::vector< SensorPtr >& sensors,
                       double t, double step)
{
    this->update<std::vector<ActuatorPtr>, std::vector<SensorPtr>>(actuators, sensors, t, step);
}

double BasicBrain::getFitness()
{
    //Calculate fitness for current policy
    double fitness = evaluator->fitness();
    evalRunning = false;
    std::cout << "Evaluating gait, fitness = " << fitness << std::endl;
    writeCurrent(fitness);
    return fitness;
}

void BasicBrain::nextBrain()
{
    bool init_supgs;
    if (!current_evalaution) {
        // first evaluation
        init_supgs = true;
    } else {
        // normal change of evaluation
        init_supgs = false;
        current_evalaution->finish(getFitness());
    }

    current_evalaution = neat->getEvaluation();
//     cppn = current_evalaution->getOrganism()->net.get();
}
void BasicBrain::writeCurrent(double fitness) 
{
    std::ofstream outputFile;
    if(generation_counter == 1) {
        std::ifstream infile("spider_9-" + std::to_string(run_count) + ".log");
        while(infile.good()) {
	    run_count++;
	    infile = std::ifstream("spider_9-" + std::to_string(run_count) + ".log");
	}
    }
    outputFile.open("spider_9-" + std::to_string(run_count) + ".log", std::ios::app | std::ios::out | std::ios::ate);
    outputFile << "- generation: " << generation_counter << std::endl;
    outputFile << "  velocities:" << std::endl;
    outputFile << "  - " << fitness << std::endl;
    outputFile.close();
}