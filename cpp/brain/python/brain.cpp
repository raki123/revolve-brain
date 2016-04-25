#include <boost/python.hpp>
#include "brain/brain.h"
#include "brain/neural_network.h"

BOOST_PYTHON_MODULE(revolve_brain_python)
{
    // abstract Brain class
    boost::python::class_< Brain, boost::noncopyable>("Brain", boost::python::no_init);

    // neural network class
    boost::python::class_<NeuralNetwork, boost::noncopyable>("NeuralNetwork")
        .def("update", &NeuralNetwork::update)
    ;
};
