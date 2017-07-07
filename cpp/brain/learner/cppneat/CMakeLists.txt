cmake_minimum_required(VERSION 3.0)

add_library(cppneat STATIC
            CPPNCrossover.cpp
            GeneticEncoding.cpp
            CPPNMutator.cpp
            CPPNNeuron.cpp
            )