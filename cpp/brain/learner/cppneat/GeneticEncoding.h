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
            : neurons_(neuron_genes)
            , connections_(connection_genes)
            , is_layered_(false)
    {}

    GeneticEncoding(std::vector<std::vector<NeuronGenePtr>> layers,
                    std::vector<ConnectionGenePtr> connection_genes)
            : layers_(layers)
            , connections_(connection_genes)
            , is_layered_(true)
    {}

    GeneticEncoding(bool layered)
            : is_layered_(layered)
    {}

    GeneticEncodingPtr copy();

    size_t num_genes();

    size_t num_neuron_genes();

    size_t num_connection_genes();

    bool HasConnection(
            const size_t _from,
            const size_t _to);

    static double Dissimilarity(
            GeneticEncodingPtr genotype1,
            GeneticEncodingPtr genotype2,
            double excess_coef,
            double disjoint_coef,
            double weight_diff_coef);

    static void ExcessDisjoint(
            GeneticEncodingPtr genotype1,
            GeneticEncodingPtr genotype2,
            int &excess_num,
            int &disjoint_num);

    static std::vector<std::pair<GenePtr, GenePtr>> Pairs(
            std::vector< GenePtr > genes_sorted1,
            std::vector< GenePtr > genes_sorted2);

    static std::vector<std::pair<int, int>>
    get_space_map(std::vector<GeneticEncodingPtr> genotypes,
                  std::map<Neuron::Ntype, Neuron::NeuronTypeSpec> brain_spec);

    void adopt(GeneticEncodingPtr adoptee);

    std::vector<GenePtr> SortedGenes();

    std::pair<int, int> InnovationsRange();

    GenePtr Find(const size_t innov_number);

    //non-layered
    void AddNeuron(NeuronGenePtr neuron_gene);

    //layered
    void AddNeuron(
            NeuronGenePtr _neuron,
            const size_t _layer,
            const bool _newLayer);

    void AddConnection(ConnectionGenePtr _connection);

    void RemoveConnection(const size_t _index);

    //non-layered
    void RemoveNeuron(const size_t index);

    //layered
    void RemoveNeuron(
            const size_t _layer,
            const size_t _index);

    bool HasNeuron(const size_t _innovationNumber);

#ifdef CPPNEAT_DEBUG
    bool is_valid();
#endif

    std::pair<size_t, size_t>
    convert_index_to_layer_index(size_t index);

    std::pair<size_t, size_t>
    convert_in_to_layer_index(const size_t innov_number);

public:
    //non-layered
    std::vector<NeuronGenePtr> neurons_;

    //layered
    std::vector<std::vector<NeuronGenePtr>> layers_;

    //both
    std::vector<ConnectionGenePtr> connections_;

private:
    std::vector<GenePtr> all_genes_sorted;
    bool all_genes_valid;
public:
    bool is_layered_;

};
}

#endif // REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_GENETICENCODING_H_