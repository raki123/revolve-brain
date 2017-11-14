#ifndef REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_CONNECTIONGENOME_H_
#define REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_CONNECTIONGENOME_H_

#include "Genome.h"
//class for connection genes
namespace CPPNEAT
{
  class ConnectionGene
          : public Gene
  {

    public:
    ConnectionGene(
            int mark_to,
            int mark_from,
            double weight,
            int innov_number = 0,
            bool enabled = true,
            std::string parent_name = "",
            int parent_index = -1,
            std::string socket = "")
            : Gene(innov_number, enabled, parent_name, parent_index)
            , from_(mark_from)
            , to_(mark_to)
            , weight(weight)
            , socket(socket)
    { this->gene_type = Gene::CONNECTION_GENE; };

    ConnectionGene(ConnectionGene &_copy)
            :
            Gene(_copy.getInnovNumber(),
                 _copy.isEnabled(),
                 _copy.get_parent_name(),
                 _copy.get_parent_index())
            , from_(_copy.from_)
            , to_(_copy.to_)
            , weight(_copy.weight)
            , socket(_copy.socket)
    { this->gene_type = Gene::CONNECTION_GENE; }

    public:
    int from_;
    int to_;
    double weight;
    std::string socket;
  };
}

#endif // REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_CONNECTIONGENOME_H_