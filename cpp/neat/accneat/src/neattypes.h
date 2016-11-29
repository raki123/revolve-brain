#pragma once

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

    enum nodetype {
        NT_BIAS = 0,
        NT_SENSOR = 1,
        NT_OUTPUT = 2,
        NT_HIDDEN = 3
    };
    enum neurontype {
	SIMPLE = 0,
	SIGMOID = 1,
	DIFFERENTIAL_CPG = 2,
	INPUT =3,
	BIAS = 4
    };

    struct OrganismEvaluation {
        real_t fitness;
        real_t error;

        void reset() {fitness = error = 0.0;}
    };

    #define accneat_in const
    #define accneat_out
    #define accneat_inout
}
