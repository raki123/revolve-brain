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
            const size_t _to,
            const size_t _from,
            const double _weight,
            const size_t _innovationNumber = 0,
            const bool _enabled = true,
            const std::string _parentsName = "",
            const int _parentsIndex = -1,
            const std::string _socket = "")
            : Gene(_innovationNumber, _enabled, _parentsName, _parentsIndex)
            , from_(_from)
            , to_(_to)
            , weight_(_weight)
            , socket_(_socket)
    { this->type_ = Gene::CONNECTION_GENE; };

    ConnectionGene(ConnectionGene &_copy)
            : Gene(_copy.InnovationNumber(),
                   _copy.IsEnabled(),
                   _copy.ParentsName(),
                   _copy.ParentsIndex())
            , from_(_copy.from_)
            , to_(_copy.to_)
            , weight_(_copy.weight_)
            , socket_(_copy.socket_)
    {
      this->type_ = Gene::CONNECTION_GENE;
    }

    inline double Weight()
    {
      return this->weight_;
    }

    inline double ModifyWeight(const double _weight)
    {
      return this->weight_ += _weight;
    }

    inline size_t From()
    {
      return this->from_;
    }

    inline size_t To()
    {
      return this->to_;
    }

    private: size_t from_;
    private: size_t to_;

    private: double weight_;

    private: std::string socket_;
  };
}

#endif // REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_CONNECTIONGENOME_H_