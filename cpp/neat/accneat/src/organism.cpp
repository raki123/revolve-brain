/*
 Copyright 2001 The University of Texas at Austin

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "genomemanager.h"
#include "network/network.h"
#include "organism.h"

#include <memory>
#include <ostream>

using namespace NEAT;

Organism::Organism(const Organism &other) {
    this->genome = env->genome_manager->make_default();
    this->net = std::unique_ptr<revolve::brain::ExtNNController>(new revolve::brain::ExtNNController());
    other.copy_into(*this);
}

Organism::Organism(const Genome &genome) {
    this->genome = env->genome_manager->make_default();
    *this->genome = genome;
    this->net = std::unique_ptr<revolve::brain::ExtNNController>(new revolve::brain::ExtNNController());

    //Note: We're in the base class constructor, so a derived class' init() won't
    //      be called. The derived class' constructor must also call init().
    init(0);
}

Organism::~Organism() {
}

void Organism::init(int gen) {
    generation=gen;
    eval.reset();
}

Organism &Organism::operator=(const Organism &other) {
    other.copy_into(*this);
    return *this;
}

void Organism::write(std::ostream &out) const {
    out << "/* Organism #" << population_index << " "
        << "Fitness: " << eval.fitness << " "
        << "Error: " << eval.error << " */" << std::endl;
    genome->print(out);
}

void Organism::copy_into(Organism &dst) const {
#define copy(field) dst.field = this->field;

    copy(population_index);
    copy(eval);
    *dst.genome = *this->genome;
    copy(generation);

    // Networks must be regenerated.
    revolve::brain::ExtNNController * ext = new revolve::brain::ExtNNController();
    dst.net.reset(ext);
    dst.net->configure("TEST", this->net->getConfig());

#undef copy
}
