#ifndef REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_GENETICENCODING_H_
#define REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_GENETICENCODING_H_

#include <vector>
#include <utility>

#include "CPPNTypes.h"
#include "CPPNNeuron.h"
#include "ConnectionGenome.h"
#include "NeuronGenome.h"

//class for the encoding of one genotype
namespace CPPNEAT {
class GeneticEncoding
{

public:
    GeneticEncoding(std::vector<NeuronGenePtr> neuron_genes,
                    std::vector<ConnectionGenePtr> connection_genes)
            : neuron_genes_(neuron_genes)
            , connection_genes_(connection_genes)
            , is_layered_(false)
    {}

    GeneticEncoding(std::vector<std::vector<NeuronGenePtr>> layers,
                    std::vector<ConnectionGenePtr> connection_genes)
            : layers_(layers)
            , connection_genes_(connection_genes)
            , is_layered_(true)
    {}

    GeneticEncoding(bool layered)
            : is_layered_(layered)
    {}

    GeneticEncodingPtr copy();

    size_t num_genes();

    size_t num_neuron_genes();

    size_t num_connection_genes();

    bool
    connection_exists(int mark_from,
                      int mark_to);

    static double
    get_dissimilarity(GeneticEncodingPtr genotype1,
                      GeneticEncodingPtr genotype2,
                      double excess_coef,
                      double disjoint_coef,
                      double weight_diff_coef);

    static void
    get_excess_disjoint(GeneticEncodingPtr genotype1,
                        GeneticEncodingPtr genotype2,
                        int &excess_num,
                        int &disjoint_num);

    static std::vector<std::pair<GenePtr, GenePtr>>
    get_pairs(std::vector<GenePtr> genes_sorted1,
              std::vector<GenePtr> genes_sorted2);

    static std::vector<std::pair<int, int>>
    get_space_map(std::vector<GeneticEncodingPtr> genotypes,
                  std::map<Neuron::Ntype, Neuron::NeuronTypeSpec> brain_spec);

    void
    adopt(GeneticEncodingPtr adoptee);

    std::vector<GenePtr>
    get_sorted_genes();

    std::pair<int, int>
    min_max_innov_numer();

    GenePtr
    find_gene_by_in(const size_t innov_number);

    //non-layered
    void
    add_neuron_gene(NeuronGenePtr neuron_gene);

    //layered
    void
    add_neuron_gene(NeuronGenePtr neuron_gene,
                    int layer,
                    bool is_new_layer);

    void
    add_connection_gene(ConnectionGenePtr connection_gene);

    void
    remove_connection_gene(int index);

    //non-layered
    void
    remonve_neuron_gene(int index);

    //layered
    void
    remove_neuron_gene(int layer,
                       int index);

    bool
    neuron_exists(const size_t innov_number);

#ifdef CPPNEAT_DEBUG
    bool is_valid();
#endif

    std::pair<size_t, size_t>
    convert_index_to_layer_index(size_t index);

    std::pair<size_t, size_t>
    convert_in_to_layer_index(const size_t innov_number);

public:
    //non-layered
    std::vector<NeuronGenePtr> neuron_genes_;

    //layered
    std::vector<std::vector<NeuronGenePtr>> layers_;

    //both
    std::vector<ConnectionGenePtr> connection_genes_;

private:
    std::vector<GenePtr> all_genes_sorted;
    bool all_genes_valid;
public:
    bool is_layered_;

};
}

#endif // REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_GENETICENCODING_H_