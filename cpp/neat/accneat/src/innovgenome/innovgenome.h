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
#pragma once

#include "genome.h"
#include "innovlinkgene.h"
#include "innovnodegene.h"
#include "innovnodelookup.h"
#include "innovation.h"
#include "brain/split_cpg/extended_neural_network_controller.h"

namespace NEAT {

    class InnovGenome : public Genome {
    public:
	std::vector<Trait> traits;
        std::vector<InnovNodeGene> nodes;
        std::vector<InnovLinkGene> links;

	struct GenomeConfig {
	    std::vector<Trait> traits;
	    std::vector<InnovNodeGene> nodes;
	    std::vector<InnovLinkGene> links;
	    unsigned int node_id;
	    unsigned int trait_id;
	    unsigned int innov;
	};
        int get_last_node_id(); //Return id of final InnovNodeGene in InnovGenome
        real_t get_last_gene_innovnum(); //Return last innovation number in InnovGenome

        InnovGenome();
        InnovGenome(rng_t rng,
                    size_t ntraits,
                    size_t ninputs,
                    size_t noutputs,
                    size_t nhidden);
	InnovGenome(rng_t rng,
                    GenomeConfig startConfig);

        virtual Genome &operator=(const Genome &other) override;

        //Destructor kills off all lists (including the trait vector)
        virtual ~InnovGenome();

        void duplicate_into(InnovGenome *offspring) const;
        InnovGenome &operator=(const InnovGenome &other);

	// Dump this genome to specified file
        virtual void print(std::ostream &out) override;
        // For debugging: A number of tests can be run on a genome to check its
        // integrity
        // Note: Some of these tests do not indicate a bug, but rather are meant
        // to be used to detect specific system states
        virtual void verify() override;
        virtual Stats get_stats() override;

        // ******* MUTATORS *******

	void mutate_random_trait();

        // Add Gaussian noise to linkweights either GAUSSIAN or COLDGAUSSIAN (from zero)
        void mutate_link_weights(real_t power,real_t rate,mutator mut_type);


        // ****** MATING METHODS *****
        static void mate(InnovGenome *genome1,
                         InnovGenome *genome2,
                         InnovGenome *offspring,
                         real_t fitness1,
                         real_t fitness2);
	static void mate_multipoint(InnovGenome *genome1,
                                  InnovGenome *genome2,
                                  InnovGenome *offspring,
                                  real_t fitness1,
                                  real_t fitness2);

        // ******** COMPATIBILITY CHECKING METHODS ********

        // This function gives a measure of compatibility between
        //   two InnovGenomes by computing a linear combination of 3
        //   characterizing variables of their compatibilty.
        //   The 3 variables represent PERCENT DISJOINT GENES,
        //   PERCENT EXCESS GENES, MUTATIONAL DIFFERENCE WITHIN
        //   MATCHING GENES.  So the formula for compatibility
        //   is:  disjoint_coeff*pdg+excess_coeff*peg+mutdiff_coeff*mdmg.
        //   The 3 coefficients are global system parameters
        real_t compatibility(InnovGenome *g);

        real_t trait_compare(Trait *t1,Trait *t2);

        // Randomize the trait pointers of all the node and connection genes
        void randomize_traits();

        Trait &get_trait(const InnovNodeGene &node);
        Trait &get_trait(const InnovLinkGene &gene);

        InnovNodeGene *get_node(int id);
        node_size_t get_node_index(int id);

        virtual void init_phenotype(class Network &net) override;
	void init_phenotype(revolve::brain::ExtNNController::ExtNNConfig &config);

    public:
        void reset();

        static bool linklist_cmp(const InnovLinkGene &a, const InnovLinkGene &b) {
            return a.innovation_num < b.innovation_num;
        }

        //Inserts a InnovNodeGene into a given ordered list of InnovNodeGenes in order
        static void add_node(std::vector<InnovNodeGene> &nlist, const InnovNodeGene &n);

        //Adds a new gene that has been created through a mutation in the
        //*correct order* into the list of links in the genome
        static void add_link(std::vector<InnovLinkGene> &glist, const InnovLinkGene &g);

    private:
        InnovLinkGene *find_link(int in_node_id, int out_node_id, bool is_recurrent);
	
        InnovNodeLookup node_lookup;
    };
}

