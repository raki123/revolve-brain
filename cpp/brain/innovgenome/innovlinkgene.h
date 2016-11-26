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
#ifndef _GENE_H_
#define _GENE_H_

#include "neat.h"
#include "trait.h"
#include "network/network.h"

namespace NEAT {

    class InnovLinkGene {
        real_t _weight; // Weight of connection
        int _in_node_id; // NNode inputting into the link
        int _out_node_id; // NNode that the link affects
        bool _is_recurrent;
        int _trait_id;  // identify the trait derived by this link

    public:
        inline int out_node_id() const { return _out_node_id; }
        inline void set_out_node_id(int id) { _out_node_id = id; }

        inline int in_node_id() const { return _in_node_id; }
        inline void set_in_node_id(int id) { _in_node_id = id; }

        inline real_t &weight() { return _weight; }

        inline int trait_id() const { return _trait_id; }
        inline void set_trait_id(int tid) { _trait_id = tid; }

        inline bool is_recurrent() const { return _is_recurrent; }
        inline void set_recurrent(bool r) { _is_recurrent = r; }


        int innovation_num;
        real_t mutation_num;  //Used to see how much mutation has changed the link
        bool enable;  //When this is off the InnovLinkGene is disabled
        bool frozen;  //When frozen, the linkweight cannot be mutated

        // Construct a gene in an invalid default state.
        InnovLinkGene() {}

        //Construct a gene with no trait
        InnovLinkGene(real_t w,
                 int inode_id,
                 int onode_id,
                 bool recur,
                 int innov,
                 real_t mnum);

        //Construct a gene with a trait
        InnovLinkGene(int trait_id,
                 real_t w,
                 int inode_id,
                 int onode_id,
                 bool recur,
                 int innov,
                 real_t mnum);

        //Construct a gene off of another gene as a duplicate
        InnovLinkGene(InnovLinkGene *g,
                 int trait_id,
                 int inode_id,
                 int onode_id);

        //Construct a gene from a file spec given traits and nodes
        InnovLinkGene(const char *argline);

        // Copy Constructor
        InnovLinkGene(const InnovLinkGene& gene);

        ~InnovLinkGene();

        //Print gene to a file- called from Genome
        void print_to_file(std::ostream &outFile);
    };

} // namespace NEAT


#endif
