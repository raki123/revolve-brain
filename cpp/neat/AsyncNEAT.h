#ifndef REVOLVE_NEAT_NEAT_H_
#define REVOLVE_NEAT_NEAT_H_

#include <list>
#include <memory>

#include "organism.h"
#include "population.h"

#include "NEATEvaluation.h"

/**
 * This is an asynchronous NEAT implemetation, it could be hard to understand
 *
 * The main loop cycle must be implemented outside (for more flexibility in real
 * time).
 *
 * To use this NEAT implemetation, get evaluations to complete using getEvaluation().
 * Then complete each evaluation calling Evaluation.finish(). If not all evaluations
 * are finished, the getEvaluation could fail.
 *
 * Multiple evaluation can be tested at the same time, is not a problem.
 *
 * It conflicts with the classical implemeation of accuNEAT since there are some
 * global objects to be used, be carefull.
 */
class AsyncNeat
{
public:
    AsyncNeat(unsigned int n_inputs,
              unsigned int n_outputs,
              int rng_seed,
              const std::string &robot_name);

    virtual ~AsyncNeat();

    /**
     * If it returns nullptr, wait until all evaluations are finished to get
     * the next generation
     */
    std::shared_ptr<NeatEvaluation>
    getEvaluation();

    /**
     * to be called before any AsyncNeat object can be used
     * @param robot_name robot_name to
     */
    static void
    Init(const std::string &robot_name)
    {
      NEAT::env->genome_manager = NEAT::GenomeManager::create(robot_name);
    };

    /**
     * to be called before any AsyncNeat object can be used
     *
     * It initializes the environment with a personalized genome manager instead of the standard one.
     * The delete of this object will be handled by AsyncNeat::CleanUp()
     */
    static void
    Init(std::unique_ptr<NEAT::GenomeManager> genome_manager)
    {
      NEAT::env->genome_manager = genome_manager.release();
    };

    /**
     * to be called after all AsyncNeat object are not in use anymore
     */
    static void
    CleanUp()
    {
      delete NEAT::env->genome_manager;
      NEAT::env->genome_manager = nullptr;
    };

    static void
    SetSearchType(NEAT::GeneticSearchType type)
    {
      NEAT::env->search_type = type;
    }

    static void
    SetPopulationSize(unsigned int n)
    {
      NEAT::env->pop_size = n;
    }

    static void
    SetPopulationType(NEAT::PopulationType type)
    {
      NEAT::env->population_type = type;
    }

    // default is 0.01
    static void
    SetMutateAddNodeProb(NEAT::real_t value)
    {
      NEAT::env->mutate_add_node_prob = value;
    }

    // default is 0.3
    static void
    SetMutateAddLinkProb(NEAT::real_t value)
    {
      NEAT::env->mutate_add_link_prob = value;
    }

    /**
     * Set the prob. that a link mutation which doesn't have to be recurrent will be made recurrent
     * @param prob new probability
     */
    static void
    SetRecurProb(NEAT::real_t prob)
    {
      NEAT::env->recur_prob = prob;
    }

    /**
     * Set the probability of forcing selection of ONLY links that are naturally recurrent
     * @param prob new probability
     */
    static void
    SetRecurOnlyProb(NEAT::real_t prob)
    {
      NEAT::env->recur_only_prob = prob;
    }

    std::shared_ptr<NeatEvaluation>
    getFittest() const
    {
      return fittest;
    }

protected:
    void setFittest(std::shared_ptr<NeatEvaluation> new_fittest, float new_fitness);

private:
    unsigned int n_inputs;
    unsigned int n_outputs;
    unsigned int generation;
    unsigned int best_fitness_counter;
    int rng_seed;

    NEAT::Population *population;
    std::list<std::shared_ptr<NeatEvaluation>> evaluatingList;
    std::list<std::shared_ptr<NeatEvaluation>> evaluatingQueue;

    std::shared_ptr<NeatEvaluation> fittest;
    float fittest_fitness;

    const std::string robot_name;

    void
    singleEvalutionFinished(std::shared_ptr<NeatEvaluation> evaluation,
                            float fitness);

    void
    next_generation();

    void
    refill_evaluation_queue();
};

#endif // REVOLVE_NEAT_NEAT_H_
