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

#include "supgbrain_python.h"
#include "python_list_wrap.h"

using namespace revolve::brain;

SUPGBrain_python::SUPGBrain_python(EvaluatorPtr evaluator,
                                   std::vector< std::vector< float > > neuron_coordinates,
                                   std::vector< ActuatorPtr >& motors,
                                   const std::vector< SensorPtr >& sensors)
 : SUPGBrain(evaluator,
             neuron_coordinates,
             motors, sensors)
{}

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
