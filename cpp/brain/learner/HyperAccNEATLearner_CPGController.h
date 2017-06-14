//
// Created by matteo on 3/14/17.
//

#ifndef REVOLVE_BRAIN_HYPERACCNEATLEARNER_H
#define REVOLVE_BRAIN_HYPERACCNEATLEARNER_H

#include "AccNEATLearner.h"

namespace revolve { namespace brain {

    class HyperAccNEATLearner_CPGController : public AccNEATLearner {
    public: // METHODS
        /**
         * Constructor
         *
         * @param evaluator pointer to the evaluator
         * @param n_inputs number of sensory inputs of the robot
         * @param n_outputs number of outputs of the robot (number of servos). It will create a CPG for
         * each output.
         * @param n_coordinates coordinates cardinality for HyperNEAT CPG positioning
         * @param connections_active square matrix holding connection activator for different CPGs.
         * Dimensions should be n_outputs x n_outputs.
         * If connections[x][y] == false, then the connection between the x and the y cpg will be deactivated.
         * @param cpgs_coordinates vector of coordinates for each CPG.
         * First dimension number should be equivalent to the n_outputs.
         * Second dimension must equal n_coordinates.
         * @param evaluationTime how much time each evaluation should last
         * @param maxEvaluations number of evaluations after which the learner will halt.
         * A negative value will be interpreted as an infinite number of evaluations.
         */
        HyperAccNEATLearner_CPGController(const EvaluatorPtr &evaluator,
                                          const size_t n_inputs,
                                          const size_t n_outputs,
                                          const size_t n_coordinates,
                                          const std::vector<std::vector<bool>> &connections_active,
                                          const std::vector<std::vector<float>> &cpgs_coordinates,
                                          const float evaluationTime,
                                          const long maxEvaluations = -1);

        virtual ~HyperAccNEATLearner_CPGController()
        {}

    protected:
        /**
         * Creates the next brain
         */
        virtual void nextBrain() override;

    protected: // VARIABLES
        const std::vector<std::vector<bool>> connections_active;
        const std::vector<std::vector<float>> cpgs_coordinates;
        const size_t n_coordinates;

    protected:// STATIC CONSTANTS
        static const size_t CPPN_OUTPUT_SIZE; // = 6
    };

}}


#endif //REVOLVE_BRAIN_HYPERACCNEATLEARNER_H
