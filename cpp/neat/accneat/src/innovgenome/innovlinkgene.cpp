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
#include "innovlinkgene.h"
#include <sstream>

using namespace NEAT;

InnovLinkGene::InnovLinkGene(real_t w,
                             int inode_id,
                             int onode_id,
                             bool recur,
                             int innov,
                             real_t mnum,
                             const std::string &creator_name,
                             const int creator_index)
 : _weight(w)
 , _in_node_id(inode_id)
 , _out_node_id(onode_id)
 , _is_recurrent(recur)
 , _trait_id(1)
 , innovation_num(innov)
 , mutation_num(mnum)
 , enable(true)
 , frozen(false)
 , creator_name(creator_name)
 , creator_index(creator_index)
{

}


//Construct a gene with a trait
InnovLinkGene::InnovLinkGene(int trait_id,
                             real_t w,
                             int inode_id,
                             int onode_id,
                             bool recur,
                             int innov,
                             real_t mnum,
                             const std::string &creator_name,
                             const int creator_index)
  : InnovLinkGene(w, inode_id, onode_id, recur, innov, mnum, creator_name, creator_index)
{
  _trait_id = trait_id;
}

InnovLinkGene::InnovLinkGene(InnovLinkGene *g,
                             int trait_id,
                             int inode_id,
                             int onode_id)
{
  _weight = g->_weight;
  _in_node_id = inode_id;
  _out_node_id = onode_id;
  _is_recurrent = g->_is_recurrent;
  _trait_id = trait_id;

  innovation_num = g->innovation_num;
  mutation_num = g->mutation_num;
  enable = g->enable;

  frozen = g->frozen;

  creator_name = g->creator_name;
  creator_index = g->creator_index;
}

InnovLinkGene::InnovLinkGene(const InnovLinkGene &gene)
{
  innovation_num = gene.innovation_num;
  mutation_num = gene.mutation_num;
  enable = gene.enable;
  frozen = gene.frozen;

  _weight = gene._weight;
  _in_node_id = gene._in_node_id;
  _out_node_id = gene._out_node_id;
  _is_recurrent = gene._is_recurrent;
  _trait_id = gene._trait_id;
}

InnovLinkGene::~InnovLinkGene()
{
}

bool NEAT::InnovLinkGene::operator==(const NEAT::InnovLinkGene &rhs) const {
  return this->_weight == rhs._weight
         && this->_in_node_id == rhs._in_node_id
         && this->_out_node_id == rhs._out_node_id
         && this->_is_recurrent == rhs._is_recurrent
         && this->_trait_id == rhs._trait_id
         && this->innovation_num == rhs.innovation_num
         && this->mutation_num == rhs.mutation_num
         && this->enable == rhs.enable
         && this->frozen == rhs.frozen;
}


bool YAML::convert<NEAT::InnovLinkGene>::decode(const YAML::Node &node, NEAT::InnovLinkGene &rhs) {
  rhs._trait_id = node["trait_id"].as<int>();
  rhs._in_node_id = node["in_node_id"].as<int>();
  rhs._out_node_id = node["out_node_id"].as<int>();
  rhs._weight = node["weight"].as<real_t>();
  rhs._is_recurrent = node["recurrent"].as<bool>();
  rhs.innovation_num = node["innovation_num"].as<int>();
  rhs.mutation_num = node["mutation_num"].as<real_t>();
  rhs.enable = node["enable"].as<bool>();
  rhs.frozen = node["frozen"].as<bool>();
  rhs.creator_name = node["creator_name"].as<std::string>();
  rhs.creator_index = node["creator_index"].as<int>();

  return true;
}

YAML::Node YAML::convert<NEAT::InnovLinkGene>::encode(const NEAT::InnovLinkGene &rhs) {
  YAML::Node node;

  node["trait_id"] = rhs._trait_id;
  node["in_node_id"] = rhs._in_node_id;
  node["out_node_id"] = rhs._out_node_id;
  node["weight"] = rhs._weight;
  node["recurrent"] = rhs._is_recurrent;
  node["innovation_num"] = rhs.innovation_num;
  node["mutation_num"] = rhs.mutation_num;
  node["enable"] = rhs.enable;
  node["frozen"] = rhs.frozen;
  node["creator_name"] = rhs.creator_name;
  node["creator_index"] = rhs.creator_index;

  return node;
}
