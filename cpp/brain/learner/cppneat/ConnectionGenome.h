#ifndef REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_CONNECTIONGENOME_H_
#define REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_CONNECTIONGENOME_H_

#include "Genome.h"
//class for connection genes
namespace CPPNEAT {
class ConnectionGene
        : public Gene
{

public:
    ConnectionGene(int mark_to,
                   int mark_from,
                   double weight,
                   int innov_number = 0,
                   bool enabled = true,
                   std::string parent_name = "",
                   int parent_index = -1,
                   std::string socket = "")
            : Gene(innov_number, enabled, parent_name, parent_index)
            , mark_to(mark_to)
            , mark_from(mark_from)
            , weight(weight)
            , socket(socket)
    { this->gene_type = Gene::CONNECTION_GENE; };

    ConnectionGene(ConnectionGene &copy_of)
            :
            Gene(copy_of.getInnovNumber(),
                 copy_of.isEnabled(),
                 copy_of.get_parent_name(),
                 copy_of.get_parent_index())
            , mark_to(copy_of.mark_to)
            , mark_from(copy_of.mark_from)
            , weight(copy_of.weight)
            , socket(copy_of.socket)
    { this->gene_type = Gene::CONNECTION_GENE; }

public:
    int mark_to;
    int mark_from;
    double weight;
    std::string socket;
};
}

#endif // REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_CONNECTIONGENOME_H_