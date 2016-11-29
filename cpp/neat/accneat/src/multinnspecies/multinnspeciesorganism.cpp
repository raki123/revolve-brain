#include "network/network.h"
#include "multinnspeciesorganism.h"
#include "multinnspecies.h"

using namespace NEAT;
using namespace std;

MultiNNSpeciesOrganism::MultiNNSpeciesOrganism(const MultiNNSpeciesOrganism &other) {
    this->genome = env->genome_manager->make_default();
    this->net = unique_ptr<revolve::brain::ExtNNController>(new revolve::brain::ExtNNController());
    other.copy_into(*this);
}

MultiNNSpeciesOrganism::MultiNNSpeciesOrganism(const Genome &genome) {
    this->genome = env->genome_manager->make_default();
    *this->genome = genome;
    this->net = unique_ptr<revolve::brain::ExtNNController>(new revolve::brain::ExtNNController());
    init(0);
}

MultiNNSpeciesOrganism::~MultiNNSpeciesOrganism() {
}

void MultiNNSpeciesOrganism::init(int gen) {
    Organism::init(gen);

    species = nullptr;  //Start it in no MultiNNSpecies
    adjusted_fitness=0.0;
    expected_offspring=0;
    eliminate=false;
    champion=false;
    super_champ_offspring=0;
}

void MultiNNSpeciesOrganism::copy_into(Organism &dst_) const {
    Organism::copy_into(dst_);

    MultiNNSpeciesOrganism *dst = dynamic_cast<MultiNNSpeciesOrganism *>(&dst_);

#define copy(field) dst->field = this->field;

    copy(species);
    copy(adjusted_fitness);
    copy(expected_offspring);
    copy(eliminate);
    copy(champion);
    copy(super_champ_offspring);

#undef copy
}
