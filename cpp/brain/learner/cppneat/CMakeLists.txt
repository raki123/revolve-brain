cmake_minimum_required(VERSION 3.0)

add_library(cppneat STATIC
	    crossover.cpp
	    genetic_encoding.cpp
	    mutator.cpp
	    neuron.cpp
)