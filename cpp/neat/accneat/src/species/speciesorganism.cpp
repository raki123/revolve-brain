#include "network/network.h"
#include "speciesorganism.h"
#include "species.h"

using namespace NEAT;
using namespace std;

SpeciesOrganism::SpeciesOrganism(const SpeciesOrganism &other) {
    this->genome = env->genome_manager->make_default();
    this->net = unique_ptr<Network>(Network::create());
    other.copy_into(*this);
}

SpeciesOrganism::SpeciesOrganism(const Genome &genome) {
    this->genome = env->genome_manager->make_default();
    *this->genome = genome;
    this->net = unique_ptr<Network>(Network::create());
    init(0);
}

SpeciesOrganism::~SpeciesOrganism() {
}

void SpeciesOrganism::init(int gen) {
    Organism::init(gen);

    species = nullptr;  //Start it in no Species
    adjusted_fitness=0.0;
    expected_offspring=0;
    eliminate=false;
    champion=false;
    super_champ_offspring=0;
}

void SpeciesOrganism::copy_into(Organism &dst_) const {
    Organism::copy_into(dst_);

    SpeciesOrganism *dst = dynamic_cast<SpeciesOrganism *>(&dst_);

#define copy(field) dst->field = this->field;

    copy(species);
    copy(adjusted_fitness);
    copy(expected_offspring);
    copy(eliminate);
    copy(champion);
    copy(super_champ_offspring);

#undef copy
}
