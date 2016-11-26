#include "basic_neat_brain.h"
#include "neat/asyncneat.h"
#include <sstream>
#include <stdexcept>
#include <limits>
#include <iomanip>
#include <string>
#include <ctime>

using namespace revolve::brain;


BasicBrain::BasicBrain(EvaluatorPtr evaluator,
                     const std::vector< ActuatorPtr >& actuators,
                     const std::vector< SensorPtr >& sensors)
  : evaluator(evaluator)
  , start_eval_time(std::numeric_limits< double >::lowest())
  , generation_counter(0)

{
    unsigned int p = 0;
    std::cout<<"sensor->sensorId()"<<std::endl;
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

void BasicBrain::init_async_neat() {
    AsyncNeat::Init();
    unsigned long populationSize = 10;
    NEAT::GeneticSearchType geneticSearchType = NEAT::GeneticSearchType::PHASED;


    AsyncNeat::SetPopulationSize(populationSize); // 10 - 25 - 50 - 75 - 100 - 1000
    AsyncNeat::SetSearchType(geneticSearchType);
    std::unique_ptr< AsyncNeat > neat(new AsyncNeat(
        n_inputs,
        n_outputs,
        std::time(0)
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
    std::cout << "Evaluating gait, fitness = " << fitness << std::endl;
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
    cppn = reinterpret_cast< NEAT::CpuNetwork* > (
        current_evalaution->getOrganism()->net.get()
    );
}
