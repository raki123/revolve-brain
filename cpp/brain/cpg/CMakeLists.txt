# CPG bits
cmake_minimum_required(VERSION 2.8)

add_library(cpg STATIC
        RythmGenerationNeuron.cpp
        PatternFormationNeuron.cpp
        MotoNeuron.cpp
        CPGNetwork.cpp
)
