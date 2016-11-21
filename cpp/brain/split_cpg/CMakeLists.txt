cmake_minimum_required(VERSION 3.0)

add_library(split_cpg STATIC
	    extended_neural_network_controller.cpp
	    weight_vector_learner.cpp
	    simple_split_brain.cpp
	    neuron.cpp
	    differential_cpg.cpp
	    bias_neuron.cpp
	    input_neuron.cpp
	    leaky_integreator.cpp
	    linear_neuron.cpp
	    neural_connection.cpp
	    oscillator_neuron.cpp
	    sigmoid_neuron.cpp
	    v_oscillator.cpp
	    x_oscillator.cpp
	    controller.h
)