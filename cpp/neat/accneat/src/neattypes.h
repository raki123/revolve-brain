#pragma once

#include <string>
#include <stdio.h>
#include <yaml-cpp/yaml.h>

// Some common types that must be in their own header so they can be used by the
// CUDA compiler (C++11 features not currently supported).
namespace NEAT {

typedef float real_t;

typedef unsigned char uchar;

typedef unsigned short ushort;

#define NODES_MAX USHRT_MAX
#define LINKS_MAX USHRT_MAX

typedef unsigned short node_size_t;

typedef unsigned short link_size_t;

enum nodetype
{
    NT_BIAS = 0,
    NT_SENSOR = 1,
    NT_OUTPUT = 2,
    NT_HIDDEN = 3
};

struct OrganismEvaluation
{
    real_t fitness;
    real_t error;

    void
    reset()
    { fitness = error = 0.0; }
};

#define accneat_in const
#define accneat_out
#define accneat_inout
}

namespace YAML {
template<>
struct convert<NEAT::nodetype> {
    static Node encode(const NEAT::nodetype& rhs) {
        std::string text;
        switch (rhs) {
            case NEAT::nodetype::NT_BIAS:
                text = "BIAS";
                break;
            case NEAT::nodetype::NT_HIDDEN:
                text = "SENSOR";
                break;
            case NEAT::nodetype::NT_OUTPUT:
                text = "OUTPUT";
                break;
            case NEAT::nodetype::NT_SENSOR:
                text = "HIDDEN";
                break;
        }

        return Node(text);

    }
    static bool decode(const Node& node, NEAT::nodetype& rhs) {
        std::string text = node.as<std::string>();

        if (text == "BIAS") {
            rhs = NEAT::nodetype::NT_BIAS;
        } else if (text == "SENSOR"){
            rhs = NEAT::nodetype::NT_SENSOR;
        } else if (text == "OUTPUT"){
            rhs = NEAT::nodetype::NT_OUTPUT;
        } else if (text == "HIDDEN"){
            rhs = NEAT::nodetype::NT_HIDDEN;
        } else {
            return false;
        }

        return true;
    }
};
}
