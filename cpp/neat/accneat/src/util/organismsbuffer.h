#pragma once

#include "organism.h"
#include "innovgenome/innovgenome.h"
#include "rng.h"
#include <assert.h>
#include <iostream>

namespace NEAT {

    template<typename TOrganism = Organism>
    class OrganismsBuffer {
        size_t _n;
        std::vector<TOrganism> _a;
        std::vector<TOrganism> _b;
        std::vector<TOrganism> *_curr;
        std::vector<TOrganism> *_prev;
    public:
    OrganismsBuffer(rng_t rng,
                    std::vector<std::unique_ptr<Genome>> &seeds,
                    size_t n,
                    size_t population_index = 0)
            : _n(n) {
            _a.reserve(n);
            _b.reserve(n);
            _curr = &_a;
            _prev = &_b;
            for(size_t i = 0; i < n; i++) {
                _a.emplace_back(*seeds[i + population_index]);
                size_t ipop = i + population_index;
                _a[i].population_index = ipop;
                _a[i].net->population_index = ipop;
                _a[i].genome->genome_id = ipop;
                _a[i].genome->rng.seed(rng.integer());
            }
            for(size_t i = 0; i < n; i++) {
                _b.emplace_back(*seeds[i + population_index]);
                size_t ipop = i + population_index;
                _b[i].population_index = ipop;
                _b[i].net->population_index = ipop;
                _b[i].genome->genome_id = ipop;
                _b[i].genome->rng.seed(rng.integer());
            }
        }

        void init_phenotypes() {
#pragma omp parallel for
            for(size_t i = 0; i < _n; i++) {
                Organism &org = curr()[i];
		revolve::brain::ExtNNController::ExtNNConfig config;
                dynamic_cast<InnovGenome *>(org.genome.get())->init_phenotype(config);
		org.net->configure("TEST", config); 
            }
        }

        size_t size(){
            return _n;
        }

        std::vector<TOrganism> &curr() {
            return *_curr;
        }

        std::vector<TOrganism> &prev() {
            return *_prev;
        }

        void next_generation(int generation) {
            if(_curr == &_a) {
                _curr = &_b;
                _prev = &_a;
            } else {
                _curr = &_a;
                _prev = &_b;
            }

            assert( _curr->size() == _n );

            for(TOrganism &org: curr())
                org.init(generation);
        }

    };

}
