//
// Created by matteo on 3/14/17.
//

#include "network/cpu/cpunetwork.h"
#include "brain/controller/CPGController.h"
#include "brain/cpg/CPGNetwork.h"
#include "HyperAccNEATLearner_CPGController.h"

using namespace revolve::brain;

const size_t HyperAccNEATLearner_CPGController::CPPN_OUTPUT_SIZE = 6;

HyperAccNEATLearner_CPGController::HyperAccNEATLearner_CPGController(const std::string &robot_name,
                                                                     const EvaluatorPtr &evaluator,
                                                                     const size_t n_inputs,
                                                                     const size_t n_outputs,
                                                                     const size_t n_coordinates,
                                                                     const std::vector<std::vector<bool>> &connections_active,
                                                                     const std::vector<std::vector<float>> &cpgs_coordinates,
                                                                     const float evaluationTime,
                                                                     const long maxEvaluations)
    : AccNEATLearner(robot_name,
                     evaluator,
                     (n_coordinates + 1) * 2,
                     HyperAccNEATLearner_CPGController::CPPN_OUTPUT_SIZE,
                     evaluationTime,
                     maxEvaluations),
      connections_active(connections_active),
      cpgs_coordinates(cpgs_coordinates),
      n_coordinates(n_coordinates + 1)
{
  assert(connections_active.size() == n_outputs);
  for (const auto &connection_row: connections_active)
    assert(connections_active.size() == connection_row.size());

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

BaseController * HyperAccNEATLearner_CPGController::create_new_controller(double fitness)
{
  if (current_evalaution) {
    // not first `create_new_controller`
    current_evalaution->finish(fitness);
  }
  current_evalaution = neat->getEvaluation();
  NEAT::CpuNetwork *cppn = reinterpret_cast< NEAT::CpuNetwork * > (
      current_evalaution->getOrganism()->net.get()
  );

  CPGController *controller = (CPGController *) active_controller.get();

  size_t x = 0;
  for (auto cpg_it = controller->beginCPGNetwork(); cpg_it != controller->endCPGNetwork(); cpg_it++) {
    cpg::CPGNetwork *cpg = (*cpg_it);

    // LOAD CPPN start and end coordinates (skip one input for the z)
    for (size_t i = 0; i < n_coordinates - 1; i++) {
      auto c = cpgs_coordinates[x][i];
      cppn->load_sensor(i, c); //start
      cppn->load_sensor(n_coordinates + i, c); //end
    }

    /* repeat for both z coordinates
     *
     * Z coordinates are representing the E or F part of
     * the CPG neuron. They are appended at the end of
     * the full CPG coordinate
     */
    for (int z = -1; z <= 1; z += 2) {
      // LOAD z input coordinate
      cppn->load_sensor(n_coordinates - 1, z); //start
      cppn->load_sensor(n_coordinates * 2 - 1, z); //end


      // ACTIVATE CPPN
      cppn->activate(1);
      NEAT::real_t *output = cppn->get_outputs();

      // USE CPPN OUTPUT TO UPDATE CPG PARAMETERS

      /* Instead of using the inner CPGController Genome here, we
       * use our own implementation. The only reason for this is
       * a matter of readability. Using the genome would have resulted
       * in a lot of ugly hardcoded numbers and quite unmaintainable code.
       *
       * Also we changed how to handle the connection weight. In the
       * CPGController::Genome implementation they are just other value
       * of the output, while here we want to use the HyperNEAT substrate
       * properties to handle the connection weights.
       *
       * Therefore connection weights are the first output with start
       * and end coordinates different.
       */

      if (z < 0) { //E
        // Rhythm generator parameters
        cpg->setRGEWeightPercentage(output[0]);    //1
        cpg->setRGEAmplitudePercentage(output[1]); //2
        cpg->setRGECPercentage(output[2]);         //3
        cpg->setRGEOffsetPercentage(output[3]);    //4

        // Pattern Formation parameters
        cpg->setPFEAlphaPercentage(output[4]); //5
        cpg->setPFEThetaPercentage(output[5]); //6
      } else { //F
        // Rhythm generator parameters
        cpg->setRGFWeightPercentage(output[0]);    //1
        cpg->setRGFAmplitudePercentage(output[1]); //2
        cpg->setRGFCPercentage(output[2]);         //3
        cpg->setRGFOffsetPercentage(output[3]);    //4

        // Pattern Formation parameters
        cpg->setPFFAlphaPercentage(output[4]); //5
        cpg->setPFFThetaPercentage(output[5]); //6
      }
    }

    const size_t n_cpgs = cpgs_coordinates.size();
    // Repeat for both z coordinates
    for (int z = -1; z <= 1; z += 2) {
      // LOAD z input coordinate
      cppn->load_sensor(n_coordinates - 1, z); //start
      cppn->load_sensor(n_coordinates * 2 - 1, z); //end

      /* Rhythm generator connection weights
       *
       * We rotate on the starting nodes instead of the ending
       * ones because in the current cpg we save the weights of
       * the receiving connections. Therefore the cpg in which
       * we save the weights is the end part, not the start as
       * intuition could erroneously suggest you.
       */
      for (size_t y = 0; y < n_cpgs; y++) {
        // ignore self connection
        if (x == y) {
          continue;
        }

        // change start node
        for (size_t i = 0; i < n_coordinates - 1; i++) {
          auto c = cpgs_coordinates[y][i];
          cppn->load_sensor(i, c); //start
        }

        // ACTIVATE CPPN
        cppn->activate(1);
        NEAT::real_t *output = cppn->get_outputs();

        if (connections_active[x][y]) {
          // set connection
          // (first connection is the weight)
          if (z < 0) {// E
            cpg->setRGEWeightNeighbourPercentage(output[0], y);
          } else {
            cpg->setRGFWeightNeighbourPercentage(output[0], y);
          }
        } else {
          // set connection weight to 0 to truncate connection
          cpg->setRGEWeightNeighbour(0, y);
          cpg->setRGFWeightNeighbour(0, y);
        }
      }
    }

    x++;
  }

  return controller;
}

