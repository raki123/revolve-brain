#ifndef REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_GENEOME_H_
#define REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_GENEOME_H_

#include <string>

//class containg meta information about a gene
namespace CPPNEAT {
class Gene
{
public:
    enum GeneType
    {
        NEURON_GENE,
        CONNECTION_GENE
    };

    Gene(int innov_number = 0,
         bool enabled = true,
         std::string parent_name = "",
         int parent_index = -1)
            :
            innov_number(innov_number)
            , enabled(enabled)
            , parent_name(parent_name)
            , parent_index(parent_index)
    {}

    virtual inline size_t
    getInnovNumber()
    { return innov_number; }

    virtual inline bool
    isEnabled()
    { return enabled; }

    virtual inline void
    setEnabled(bool enabled)
    { this->enabled = enabled; }

    virtual inline std::string
    get_parent_name()
    { return this->parent_name; }

    virtual inline int
    get_parent_index()
    { return this->parent_index; }

    GeneType gene_type;
private:
    unsigned int innov_number;
    bool enabled;
    std::string parent_name;
    int parent_index;

};
}

#endif // REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_GENEOME_H_