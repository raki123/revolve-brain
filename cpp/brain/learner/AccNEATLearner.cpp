//
// Created by matteo on 3/13/17.
//

#include <ctime>
#include "brain/controller/AccNEATCPPNController.h"
#include "AccNEATLearner.h"

using namespace revolve::brain;


AccNEATLearner::AccNEATLearner(EvaluatorPtr evaluator, size_t n_inputs, size_t n_outputs, const float evaluationTime,
                               const long maxEvaluations)
    : BaseLearner(std::unique_ptr<BaseController>(new AccNEATCPPNController(n_inputs, n_outputs))),
      evaluator(evaluator),
      n_inputs(n_inputs),
      n_outputs(n_outputs),
      generation_counter(0),
      start_eval_time(std::numeric_limits<double>::lowest()),
      EVALUATION_TIME(evaluationTime),
      MAX_EVALUATIONS(maxEvaluations)
{
  this->initAsyncNeat();
}

void AccNEATLearner::initAsyncNeat()
{
  std::unique_ptr<AsyncNeat> neat(new AsyncNeat(
      (unsigned int) n_inputs,
      (unsigned int) n_outputs,
      (int) std::time(0) // random seed
  ));
  this->neat = std::move(neat);
}

BaseController *AccNEATLearner::update(const std::vector<SensorPtr> &sensors, double t, double step)
{

  // Evaluate policy on certain time limit
  if ((t - start_eval_time) > EVALUATION_TIME) {

    // check if to stop the experiment. Negative value for MAX_EVALUATIONS will never stop the experiment
    if (MAX_EVALUATIONS > 0 && generation_counter > MAX_EVALUATIONS) {
      std::cout << "#AccNEATLearner::update() Max Evaluations (" << MAX_EVALUATIONS << ") reached. stopping now."
                << std::endl;
      std::exit(0);
    }
    generation_counter++;
    std::cout
        << "#AccNEATLearner::update() EVALUATING NEW BRAIN (generation "
        << generation_counter
        << " )"
        << std::endl;
    this->nextBrain();
    start_eval_time = t;
  }

  return BaseLearner::update(sensors, t, step);
}

void AccNEATLearner::nextBrain()
{
  if (current_evalaution) {
    // not first `nextBrain`
    current_evalaution->finish(getFitness());
  }

  current_evalaution = neat->getEvaluation();
  NEAT::CpuNetwork *cppn = reinterpret_cast< NEAT::CpuNetwork * > (
      current_evalaution->getOrganism()->net.get()
  );

  AccNEATCPPNController *controller = (AccNEATCPPNController *) active_controller.get();
  controller->setCPPN(cppn);
  evaluator->start();
}

float AccNEATLearner::getFitness()
{
  //Calculate fitness for current policy
  float fitness = (float) evaluator->fitness();
  std::cout << "#AccNEATLearner::getFitness() Evaluating gait, fitness = " << fitness << std::endl;
  return fitness;
}
