#ifndef REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_GENEOME_H_
#define REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_GENEOME_H_

#include <string>

//class containg meta information about a gene
namespace CPPNEAT
{
  class Gene
  {
    public:
    enum GeneType
    {
      NEURON_GENE,
      CONNECTION_GENE
    };

    Gene(
            const size_t _innovationNumber = 0,
            const bool _enabled = true,
            const std::string &_parentsName = "",
            const size_t _parentsIndex = -1
    )
            : enabled_(_enabled)
            , innovationNumber_(_innovationNumber)
            , parentsIndex_(_parentsIndex)
            , parentsName_(_parentsName)
    {}

    virtual inline size_t InnovationNumber()
    {
      return this->innovationNumber_;
    }

    virtual inline bool IsEnabled()
    {
      return this->enabled_;
    }

    virtual inline void setEnabled(bool _enabled)
    {
      this->enabled_ = _enabled;
    }

    virtual inline std::string ParentsName()
    {
      return this->parentsName_;
    }

    virtual size_t ParentsIndex()
    {
      return this->parentsIndex_;
    }

    virtual GeneType Type()
    {
      return this->type_;
    }

    protected: GeneType type_;

    private: bool enabled_;

    private: size_t innovationNumber_;
    private: size_t parentsIndex_;

    private: std::string parentsName_;
  };
}

#endif // REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_GENEOME_H_