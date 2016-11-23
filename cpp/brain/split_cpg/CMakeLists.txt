cmake_minimum_required(VERSION 3.0)

add_library(split_cpg STATIC
	    extended_neural_network_controller.cpp
	    weight_vector_learner.cpp
	    rlpower_learner.cpp
)