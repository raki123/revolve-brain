//
// Created by matteo on 3/14/17.
//

#include "network/cpu/cpunetwork.h"
#include "brain/controller/CPGController.h"
#include "brain/cpg/CPGNetwork.h"
#include "HyperAccNEATLearner_CPGController.h"

using namespace revolve::brain;

HyperAccNEATLearner_CPGController::HyperAccNEATLearner_CPGController(const EvaluatorPtr &evaluator,
                                                                     const size_t n_inputs,
                                                                     const size_t n_outputs,
                                                                     const size_t n_coordinates,
                                                                     const std::vector<std::vector<bool>> &connections,
                                                                     const std::vector<std::vector<float>> &cpgs_coordinates,
                                                                     const float evaluationTime,
                                                                     const long maxEvaluations)
    : AccNEATLearner(evaluator,
                     n_coordinates,
                     CalculateNEvolvableParameters(n_outputs),
                     evaluationTime,
                     maxEvaluations),
      connections(connections),
      cpgs_coordinates(cpgs_coordinates),
      n_evolvable_parameters_per_cpg(
          CalculateNEvolvableParameters(n_outputs)
      )
{
  assert(connections.size() == n_outputs);
  for (const auto &connection_row: connections)
    assert(connections.size() == connection_row.size());

  assert(cpgs_coordinates.size() == n_outputs);
  for (const auto &cpg_coordinates: cpgs_coordinates)
    assert(cpg_coordinates.size() == n_coordinates);

  std::unique_ptr<CPGController> controller(
      new CPGController(n_inputs, n_outputs)
  );

  this->active_controller = std::move(controller);

  // NEAT settings
  AsyncNeat::SetRecurProb(0);
  AsyncNeat::SetRecurOnlyProb(0);
}

void HyperAccNEATLearner_CPGController::nextBrain()
{
  current_evalaution = neat->getEvaluation();
  NEAT::CpuNetwork *cppn = reinterpret_cast< NEAT::CpuNetwork * > (
      current_evalaution->getOrganism()->net.get()
  );

  CPGController *controller = (CPGController *) active_controller.get();

  size_t x = 0;
  for (auto cpg_it = controller->beginCPGNetwork(); cpg_it != controller->endCPGNetwork(); cpg_it++) {
    cpg::CPGNetwork *cpg = (*cpg_it);

    // LOAD AND ACTIVATE CPPN
    for (size_t i = 0; i < AccNEATLearner::n_inputs; i++) {
      cppn->load_sensor(i, cpgs_coordinates[x][i]);
    }
    cppn->activate(1);
    NEAT::real_t *output = cppn->get_outputs();

    // USE CPPN OUTPUT TO UPDATE CPG PARAMETERS

    /* Instead of using the inner CPGController Genome here, we
     * use our own implementation. The only reason for this is
     * a matter of readability. Using the genome would have resulted
     * in a lot of ugly hardcoded numbers and quite unmaintainable code.
     */

    size_t p = 0;
    // Rhythm generator parameters
    cpg->setRGEWeightPercentage(output[p++]); //1
    cpg->setRGFWeightPercentage(output[p++]); //2
    cpg->setRGEAmplitudePercentage(output[p++]); //3
    cpg->setRGFAmplitudePercentage(output[p++]); //4
    cpg->setRGECPercentage(output[p++]); //5
    cpg->setRGFCPercentage(output[p++]); //6
    cpg->setRGEOffsetPercentage(output[p++]); //7
    cpg->setRGFOffsetPercentage(output[p++]); //8

    // Pattern Formation parameters
    cpg->setPFEAlphaPercentage(output[p++]); //9
    cpg->setPFFAlphaPercentage(output[p++]); //10
    cpg->setPFEThetaPercentage(output[p++]); //11
    cpg->setPFFThetaPercentage(output[p++]); //12

    // Rhythm generator connection weights
    const size_t n_cpgs = cpgs_coordinates.size();
    for (size_t y = 0; y<n_cpgs; y++) {

      // ignore self connection
      if (x == y) {
        continue;
      }

      if (connections[x][y]) {
        // set connection
        cpg->setRGEWeightNeighbourPercentage(output[p++], y);
        cpg->setRGFWeightNeighbourPercentage(output[p++], y);
      } else {
        // ignore connection and skip CPPN outputs
        cpg->setRGEWeightNeighbour(0, y);
        cpg->setRGFWeightNeighbour(0, y);
        p += 2;
      }
    }

    x++;
  }
}

