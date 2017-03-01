/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2016  Matteo De Carlo <matteo.dek@covolunablu.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "SUGPBrain_python.h"
#include "PythonListWrap.h"

using namespace revolve::brain;

// SUPGBrain_python::SUPGBrain_python(EvaluatorPtr evaluator,
//                                    std::vector< std::vector< float > > neuron_coordinates,
//                                    std::vector< ActuatorPtr >& motors,
//                                    const std::vector< SensorPtr >& sensors)
//  : SUPGBrain(evaluator,
//              neuron_coordinates,
//              motors, sensors)
// {}

revolve::brain::SUPGBrain_python::SUPGBrain_python(revolve::brain::EvaluatorPtr evaluator,
                                                   const boost::python::list& neuron_coordinates_list,
                                                   const boost::python::list& actuators,
                                                   const boost::python::list& sensors)
 : SUPGBrain(evaluator)
{
    python_list_wrap<ActuatorPtr> actuator_wrap(&actuators);
    python_list_wrap<SensorPtr>   sensor_wrap(&sensors);


    // init neuron_coordinates from neuron_coordinates_list
    int previous_input = -1;
    boost::python::ssize_t n_actuators = boost::python::len(neuron_coordinates_list);
    neuron_coordinates.resize(n_actuators);

    for(boost::python::ssize_t i=0; i<n_actuators; i++) {
        boost::python::object elem = neuron_coordinates_list[i];
        boost::python::list node = boost::python::extract<boost::python::list>(elem);


        boost::python::ssize_t n_inputs = boost::python::len(node);
        if (previous_input >= 0 && previous_input != n_inputs) {
            std::stringstream ss;
            ss << "neuron coordinate list inner size mismatch: len(list[" << i-1 << "]) == " << previous_input << " and len(list[" << i << "]) == " << n_inputs << " are different!";
            throw std::invalid_argument( ss.str());
        }
        previous_input = n_inputs;

        std::vector<float> cpp_coordinates(n_inputs);
        for(boost::python::ssize_t j=0; j<n_inputs; j++) {
            boost::python::object elem = node[j];
            float coordinate = boost::python::extract<float>(elem);
            cpp_coordinates[j] = coordinate;
        }

        this->neuron_coordinates[i] = cpp_coordinates;
    }



    if (actuator_wrap.size() != neuron_coordinates.size()) {
        std::stringstream ss;
        ss << "actuator size [" << actuator_wrap.size() << "] and neuron coordinates size [" << neuron_coordinates.size() << "] are different!";
        throw std::invalid_argument( ss.str());
    }

    unsigned int p = 0;
    std::cout<<"sensor->sensorId()"<<std::endl;
    for (auto sensor : sensor_wrap) {
        std::cout << "sensor: " << sensor->sensorId() << "(inputs: " << sensor->inputs() << ")" << std::endl;
        p += sensor->inputs();
    }
    std::cout<<"END sensor->sensorId()"<<std::endl;
    this->n_inputs = p;

    p = 0;
    for (auto actuator : actuator_wrap) {
        p += actuator->outputs();
    }
    this->n_outputs = p;


    this->init_async_neat();
}


void SUPGBrain_python::update(boost::python::list& actuators,
                              const boost::python::list& sensors,
                              double t, double step)
{
    python_list_wrap<ActuatorPtr> actuator_wrap(&actuators);
    python_list_wrap<SensorPtr>   sensor_wrap(&sensors);

    SUPGBrain::update
        <python_list_wrap<ActuatorPtr>, python_list_wrap<SensorPtr>>
        (actuator_wrap, sensor_wrap, t, step);
}
