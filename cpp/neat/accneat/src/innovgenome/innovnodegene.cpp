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

InnovNodeGene::InnovNodeGene(const std::string &robot_name)
  : creator_name(robot_name)
  , creator_index(-1)
{

}

InnovNodeGene::InnovNodeGene(nodetype ntype,
                             int nodeid,
                             const std::string &robot_name,
                             const std::string &creator_name,
                             const int creator_index)
  : creator_name(creator_name)
  , creator_index(creator_index)
  , type(ntype)
  , node_id(nodeid)
  , frozen(false)
  , trait_id(1)
{
}

InnovNodeGene::InnovNodeGene(const InnovNodeGene &other)
        : InnovNodeGene(other.type,
                        other.node_id,other.creator_name, creator_name, creator_index)
{
  frozen = other.frozen;
  trait_id = other.trait_id;
}

InnovNodeGene::~InnovNodeGene()
{
}

bool YAML::convert<NEAT::InnovNodeGene>::decode(const YAML::Node &node, InnovNodeGene &rhs)
{
  rhs.node_id = node["node_id"].as<int>();
  rhs.set_trait_id(node["trait_id"].as<int>());
  rhs.set_type(node["type"].as<nodetype>());
  rhs.frozen = node["frozen"].as<bool>();
  rhs.creator_name = node["creator_name"].as<std::string>();
  rhs.creator_index = node["creator_index"].as<int>();

  return true;
}

YAML::Node YAML::convert<NEAT::InnovNodeGene>::encode(const InnovNodeGene &rhs)
{
  YAML::Node node;
  node["node_id"] = rhs.node_id;
  node["trait_id"] = rhs.get_trait_id();
  node["type"] = rhs.get_type();
  node["frozen"] = rhs.frozen;
  node["creator_name"] = rhs.creator_name;
  node["creator_index"] = rhs.creator_index;

  return node;
}
