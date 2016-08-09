#include <boost/python.hpp>
#include "brain/brain.h"
#include "brain/neural_network.h"
#include "brain/rlpower.h"
#include "brain/python/actuator_wrap.h"
#include "brain/python/evaluator_wrap.h"
#include "brain/python/sensor_wrap.h"


using namespace revolve::brain;

BOOST_PYTHON_MODULE(revolve_brain_python)
{
    // evaluator abstract class
    boost::python::class_< EvaluatorWrap, boost::noncopyable >("Evaluator")
        .def("start", boost::python::pure_virtual(&Evaluator::start))
        .def("fitness", boost::python::pure_virtual(&Evaluator::fitness))
    ;
    //boost::python::register_ptr_to_python<boost::shared_ptr<Evaluator>>();

    // actuator abstract class
    boost::python::class_< ActuatorWrap, boost::noncopyable >("Actuator")
        .def("update", boost::python::pure_virtual(&Actuator::update))
        .def("outputs", boost::python::pure_virtual(&Actuator::outputs))
    ;

    // sensor abstract class
    boost::python::class_< SensorWrap, boost::noncopyable >("Sensor")
        .def("read", boost::python::pure_virtual(&Sensor::read))
        .def("inputs", boost::python::pure_virtual(&Sensor::inputs))
    ;

    // abstract Brain class
    boost::python::class_< Brain, boost::noncopyable>("Brain", boost::python::no_init);

    // neural network class
    boost::python::class_< NeuralNetwork, boost::python::bases<Brain> ,boost::noncopyable >("NeuralNetwork")
        .def("update", &NeuralNetwork::update)
    ;

    // rlpower controller class
    boost::python::class_< RLPower, boost::python::bases<Brain>, boost::noncopyable >("RLPower", boost::python::init< boost::shared_ptr<Evaluator>, unsigned int, unsigned int >() )
        .def("update", &RLPower::update)
    ;
    //boost::python::implicitly_convertible<RLPower*, Brain*>();
};
