#include <boost/python.hpp>
#include "brain/brain.h"
#include "brain/neural_network.h"
#include "brain/rlpower.h"
#include "brain/python/actuator_wrap.h"
#include "brain/python/evaluator_wrap.h"
#include "brain/python/sensor_wrap.h"
#include "brain/python/rlpower_python.h"
#include "brain/python/supgbrain_python.h"
#include "brain/python/CPGBrain_python.h"
#include "brain/python/python_array.h"
#include "neat/asyncneat.h"


using namespace revolve::brain;

BOOST_PYTHON_MODULE(revolve_brain_python)
{
    // class to access arrays from python
    boost::python::class_< python_array<double> >("Array", boost::python::no_init)
        .def("__setitem__", &python_array<double>::setitem )
        .def("__getitem__", &python_array<double>::getitem )
    ;

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
        .def("sensorId", boost::python::pure_virtual(&Sensor::sensorId))
    ;

    // abstract Brain class
    boost::python::class_< Brain, boost::noncopyable>("Brain", boost::python::no_init);

    // neural network class
    boost::python::class_< NeuralNetwork,
        boost::python::bases<Brain>,
        boost::noncopyable >
        ("NeuralNetwork")
        .def("update", &NeuralNetwork::update)
    ;

    // rlpower controller class
    boost::python::class_< RLPower_python,
        boost::python::bases<Brain>,
        boost::noncopyable >
        (
            "RLPower",
            boost::python::init< std::string, boost::python::object, boost::shared_ptr<Evaluator>, unsigned int, unsigned int >()
        )
        .def("update", &RLPower_python::update)
    ;
    //boost::python::implicitly_convertible<RLPower*, Brain*>();

    // cpg controller class
    boost::python::class_< CPGBrain_python,
        boost::python::bases<Brain>,
        boost::noncopyable >
        (
            "CPGBrain",
            boost::python::init<
                std::string,
                boost::shared_ptr<Evaluator>,
                unsigned int,
                unsigned int>()
        )
        .def("update", &CPGBrain_python::update)
    ;


    // supg controller class
    boost::python::class_< SUPGBrain_python,
        boost::python::bases<Brain>,
        boost::noncopyable >
        (
            "SUPGBrain",
            boost::python::init<
                boost::shared_ptr<Evaluator>,
                boost::python::list&,
                boost::python::list&,
                boost::python::list&>()
        )
        .def("update", &SUPGBrain_python::update)
    ;

    boost::python::class_< AsyncNeat,
        boost::noncopyable >
        (
            "AsyncNeat",
            boost::python::no_init
        )
        .def("Init", static_cast<void(*)()>(&AsyncNeat::Init))
        .staticmethod("Init")
        .def("CleanUp", &AsyncNeat::CleanUp)
        .staticmethod("CleanUp")
    ;
}
