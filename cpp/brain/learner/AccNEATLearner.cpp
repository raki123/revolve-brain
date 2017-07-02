//
// Created by matteo on 3/13/17.
//

#include <ctime>
#include <fstream>

#include "brain/controller/AccNEATCPPNController.h"
#include "AccNEATLearner.h"

using namespace revolve::brain;


AccNEATLearner::AccNEATLearner(const std::string &robot_name,
                               EvaluatorPtr evaluator,
                               size_t n_inputs,
                               size_t n_outputs,
                               const float evaluationTime,
                               const long maxEvaluations)
    : BaseLearner(std::unique_ptr<BaseController>(new AccNEATCPPNController(n_inputs, n_outputs)), robot_name),
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

AccNEATLearner::~AccNEATLearner()
{
  AsyncNeat::CleanUp();
}

void AccNEATLearner::initAsyncNeat()
{
  AsyncNeat::Init(robot_name);
  std::unique_ptr<AsyncNeat> neat(new AsyncNeat(
      (unsigned int) n_inputs,
      (unsigned int) n_outputs,
      (int) std::time(0), // random seed,
      robot_name
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
//    std::cout
//        << "#AccNEATLearner::update() EVALUATING NEW BRAIN (generation "
//        << generation_counter
//        << " )"
//        << std::endl;

    double_t fitness = getFitness();
    std::cout << robot_name << " : " << generation_counter << ": fitness " << fitness << std::endl;
    this->writeCurrent(fitness);

    BaseController *new_controller = this->create_new_controller(fitness);
    if (new_controller != active_controller.get()) {
      this->active_controller.reset(new_controller);
    }
    start_eval_time = t;
    evaluator->start();
  }

  return BaseLearner::update(sensors, t, step);
}

BaseController * AccNEATLearner::create_new_controller(double fitness)
{
  if (current_evalaution) {
    // not first `create_new_controller`
    current_evalaution->finish((float) fitness);
  }
  current_evalaution = neat->getEvaluation();
  NEAT::CpuNetwork *cppn = reinterpret_cast< NEAT::CpuNetwork * > (
      current_evalaution->getOrganism()->net.get()
  );

  AccNEATCPPNController *controller = (AccNEATCPPNController *) active_controller.get();
  controller->setCPPN(cppn);

  return controller;
}

float AccNEATLearner::getFitness()
{
  //Calculate fitness for current policy
  float fitness = (float) evaluator->fitness();
//  std::cout << "#AccNEATLearner::getFitness() Evaluating gait, fitness = " << fitness << std::endl;
  return fitness;
}

void AccNEATLearner::writeCurrent(double fitness)
{
  std::ofstream outputFile;
  outputFile.open(this->robot_name + ".log",
                  std::ios::app | std::ios::out | std::ios::ate);
  outputFile << "- generation: " << this->generation_counter << std::endl;
  outputFile << "  velocity: " << fitness << std::endl;
  // TODO: Should we record an entire generation?
  outputFile.close();
}
