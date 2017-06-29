#include "genomemanager.h"
#include "innovgenome/innovgenomemanager.h"
#include "util/util.h"

using namespace NEAT;

GenomeManager *
GenomeManager::create(const std::string &robot_name)
{
  switch (env->genome_type) {
    case GenomeType::INNOV:
      return new InnovGenomeManager(robot_name);
    default: panic();
  }
}
