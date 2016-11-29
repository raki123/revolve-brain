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

#include "neattypes.h"
#include <cstddef>

namespace NEAT {

    //---
    //--- CLASS NetDims
    //---
    struct NetDims {
        struct {
            node_size_t bias;
            node_size_t sensor;
            node_size_t output;
            node_size_t hidden;

            node_size_t all;
            node_size_t input;
            node_size_t noninput;
        } nnodes;

        link_size_t nlinks;
    };

    //---
    //--- CLASS NetLink
    //---
    struct NetLink {
        real_t weight; // Weight of connection
        node_size_t in_node_index; // NetNode inputting into the link
        node_size_t out_node_index; // NetNode gaining energy from the link
    };

    //---
    //--- CLASS NetNode
    //---
    struct NetNode {
        link_size_t incoming_start;
        link_size_t incoming_end;
    };

    //---
    //--- CLASS Network
    //---
    class Network {
    public:
        static Network *create();

        std::size_t population_index;

        virtual ~Network() {}

        virtual void configure(const NetDims &dims,
                               NetNode *nodes,
                               NetLink *links) = 0;

        virtual NetDims get_dims() = 0;
    };

    //---
    //--- CLASS NetworkEvaluator
    //---
    class NetworkEvaluator {
    public:
        virtual ~NetworkEvaluator() {}

        virtual void execute(class Network **nets_,
                             class OrganismEvaluation *results,
                             size_t nnets) = 0;
    };

} // namespace NEAT
