
add_library(extnn STATIC
	    neuron.cpp
	    differential_cpg.cpp
	    bias_neuron.cpp
	    input_neuron.cpp
	    leaky_integrator.cpp
	    linear_neuron.cpp
	    neural_connection.cpp
	    oscillator_neuron.cpp
	    sigmoid_neuron.cpp
	    v_oscillator.cpp
	    x_oscillator.cpp
	    input_dependent_oscillator_neuron.cpp
)
