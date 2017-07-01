//
// Created by matteo on 3/13/17.
//

#include <random>
#include "CPGController.h"

using namespace revolve::brain;

#define NOISE_SIGMA 0.1

CPGController::CPGController(size_t n_inputs, size_t n_outputs)
    : n_inputs(n_inputs), n_outputs(n_outputs), cpgs(n_outputs, nullptr),
      connections(n_outputs, std::vector<cpg::CPGNetwork::Weights>(n_outputs))
{
  inputs_vector = new double[n_inputs];
  outputs_vector = new double[n_outputs];


  size_t n_connections = n_outputs - 1;

  for (size_t i = 0; i < n_outputs; ++i) {
    cpgs[i] = new cpg::CPGNetwork((size_t) n_inputs, (size_t) n_connections);
  }

  for (size_t i = 0; i < n_outputs; ++i) {
    for (size_t j = 0; j < n_outputs; ++j) {
      if (i == j) continue;
      cpgs[i]->addConnection(cpgs[j]);
    }
  }

  initRandom(NOISE_SIGMA);
}

CPGController::~CPGController()
{
  delete[] inputs_vector;
  delete[] outputs_vector;

  for (cpg::CPGNetwork *ptr : cpgs)
    delete ptr;
}

void CPGController::update(const std::vector<ActuatorPtr> &actuators, const std::vector<SensorPtr> &sensors, double t,
                           double step)
{
  // Read sensor data and feed the neural network
  size_t p = 0;
  for (auto sensor : sensors) {
    sensor->read(&inputs_vector[p]);
    p += sensor->inputs();
  }
  assert(p == n_inputs);

  std::vector<cpg::real_t> inputs_readings(n_inputs, 0);
  for (size_t i = 0; i < n_inputs; ++i)
    inputs_readings[i] = (cpg::real_t) inputs_vector[i];

  for (size_t i = 0; i < n_outputs; ++i) {
    cpg::CPGNetwork *cpg_network = cpgs[i];
    outputs_vector[i] = cpg_network->update(inputs_readings, step) * 100;
  }

  p = 0;
  for (auto actuator: actuators) {
    actuator->update(&outputs_vector[p], step);
    p += actuator->outputs();
  }
  assert(p == n_outputs);
}

void CPGController::initRandom(float sigma)
{
  std::random_device rd;
  std::mt19937 mt(rd());
  std::normal_distribution<float> dist(0, sigma);

  for (auto cpg: cpgs) {
    std::shared_ptr<std::vector<cpg::real_t>> genome = cpg->get_genome();
    size_t genome_size = genome->size();
    for (size_t i = 0; i < genome_size; ++i) {
      genome->at(i) = dist(mt);
    }

    cpg->update_genome();
  }

}
