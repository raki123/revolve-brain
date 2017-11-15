#ifndef REVOLVEBRAIN_BRAIN_LEARNER_NEATLEARNER_H_
#define REVOLVEBRAIN_BRAIN_LEARNER_NEATLEARNER_H_

//#define CPPNEAT_DEBUG

#include "brain/learner/cppneat/CPPNTypes.h"
#include "brain/learner/cppneat/CPPNMutator.h"
#include "Learner.h"

#include <map>
#include <string>
#include <vector>
#include <random>

//crossover between genotypes
namespace CPPNEAT
{
  class NEATLearner
          : public revolve::brain::Learner< GeneticEncodingPtr >
  {
    public:
    struct LearningConfiguration
    {
      bool asexual;
      int pop_size;
      int tournament_size;
      int num_children;
      int max_generations;
      int repeat_evaluations;
      int initStructMutations;
      int num_first;
      int num_second;
      std::string parent1;
      std::string parent2;
      double weight_mutation_probability;
      double weight_mutation_sigma;
      double param_mutation_probability;
      double param_mutation_sigma;
      double structural_augmentation_probability;
      double structural_removal_probability;
      double speciation_threshold;
      double interspecies_mate_probability;
      GeneticEncodingPtr start_from;
    };

    NEATLearner(
            MutatorPtr mutator,
            const std::string &_innovations,
            const std::string &_parent1,
            const std::string &_parent2,
            LearningConfiguration conf);

    void Initialise(std::vector< GeneticEncodingPtr > init_genotypes);

    std::vector< GeneticEncodingPtr > InitCppns();

    std::vector< GeneticEncodingPtr > LoadCppns(
            const std::string &_path,
            int offset);

    std::vector< GeneticEncodingPtr > LoadSecondCppns(
            const std::string &_path,
            int offset);

    void MutateStructure(GeneticEncodingPtr genotype);

    //standard parameters
    static const bool ASEXUAL;
    static const int POP_SIZE;
    static const int TOURNAMENT_SIZE;
    static const int NUM_CHILDREN;
    static const double WEIGHT_MUTATION_PROBABILITY;
    static const double WEIGHT_MUTATION_SIGMA;
    static const double PARAM_MUTATION_PROBABILITY;
    static const double PARAM_MUTATION_SIGMA;
    static const double STRUCTURAL_AUGMENTATION_PROBABILITY;
    static const double STRUCTURAL_REMOVAL_PROBABILITY;
    static const int MAX_GENERATIONS;
    static const double SPECIATION_TRESHOLD;
    static const int REPEAT_EVALUATIONS;
    static const int INITIAL_STRUCTURAL_MUTATIONS;
    static const double INTERSPECIES_MATE_PROBABILITY;
    private:
    virtual void reportFitness(
            std::string id,
            GeneticEncodingPtr genotype,
            double fitness);

    virtual GeneticEncodingPtr currentGenotype();

    void writeGenome(
            std::string robot_name,
            GeneticEncodingPtr genome);

    void RecordBest(const std::string &_name);

    void share_fitness();

    void ProduceGeneration();

    GeneticEncodingPtr OffspringGenome(
            GeneticEncodingPtr parent1,
            GeneticEncodingPtr parent2);

    void displayConfiguration();

    std::pair< GeneticEncodingPtr, GeneticEncodingPtr > Tournament(
            std::vector< std::pair< GeneticEncodingPtr, double>> candidates,
            unsigned int _tournamentSize);


    GeneticEncodingPtr activeBrain_;
    std::map< double, GeneticEncodingPtr, std::greater< double > > bestBrains_;
//    double fitness;
    std::vector< GeneticEncodingPtr > evaluation_queue_;
    std::vector< GeneticEncodingPtr > brain_population_;
    std::map< GeneticEncodingPtr, double > brain_fitness;
    std::map< GeneticEncodingPtr, double > brain_velocity;
    std::map< GeneticEncodingPtr, std::vector< GeneticEncodingPtr>> species;
    std::vector< double > fitness_buffer;
    int generation_number;
    int total_brains_evaluated;

    MutatorPtr mutator;
    std::string mutator_path;

    bool is_asexual_;

    size_t initial_structural_mutations_;
    size_t num_children_;
    size_t population_size_;
    size_t tournament_size_;

    double weight_mutation_probability;
    double weight_mutation_sigma;
    double param_mutation_probability;
    double param_mutation_sigma;
    double structural_augmentation_probability;
    double structural_removal_probability;
    int max_generations;
    double speciation_threshold;
    unsigned int repeat_evaluations;
    GeneticEncodingPtr start_from_;
    double interspecies_mate_probability;
    std::mt19937 generator;

    private: std::map< size_t, size_t > secondToFirst_;
  };
}

#endif // REVOLVEBRAIN_BRAIN_LEARNER_NEATLEARNER_H_