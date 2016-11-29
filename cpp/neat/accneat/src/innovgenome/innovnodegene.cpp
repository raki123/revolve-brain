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
#include "innovnodegene.h"
#include <sstream>
using namespace NEAT;

InnovNodeGene::InnovNodeGene(nodetype ntype,int nodeid, neurontype neuron_type) {
    type=ntype; //NEURON or SENSOR type
    this->neuron_type = neuron_type;
    node_id=nodeid;
    frozen=false;
    trait_id=1;
}

InnovNodeGene::InnovNodeGene (const char *argline) {
    std::stringstream ss(argline);
    int nodety, nodepl;
    ss >> node_id >> trait_id >> nodety >> nodepl;
    type = (nodetype)nodety;

    if(trait_id == 0)
        trait_id = 1;

    // Get the Sensor Identifier and Parameter String
    // mySensor = SensorRegistry::getSensor(id, param);
    frozen=false;  //TODO: Maybe change
}

InnovNodeGene::~InnovNodeGene() {
}

InnovNodeGene & InnovNodeGene::operator=(const InnovNodeGene &other) {
    this->type = other.get_type();
    this->neuron_type = other.neuron_type;
    this->node_id = other.node_id;
    this->frozen = other.frozen;
    this->trait_id = other.get_trait_id();
}

void InnovNodeGene::print_to_file(std::ostream &outFile) {
    outFile<<"node "<<node_id<<" ";
    outFile<<trait_id<<" ";
    outFile<<(int)type<<std::endl;
}
