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

#include "supgbrainphototaxis.h"

using namespace revolve::brain;

SUPGBrainPhototaxis::SUPGBrainPhototaxis(EvaluatorPtr evaluator,
                                         std::function<FakeLightSensor *(std::vector<float> coordinates)> _light_constructor_left,
                                         std::function<FakeLightSensor *(std::vector<float> coordinates)> _light_constructor_right,
                                         double _light_radius_distance,
                                         const std::vector< std::vector< float > >& neuron_coordinates,
                                         const std::vector< ActuatorPtr >& actuators,
                                         const std::vector< SensorPtr >& sensors)
    : SUPGBrain(evaluator, neuron_coordinates, actuators, sensors)
    , light_constructor_left(_light_constructor_left)
    , light_constructor_right(_light_constructor_right)
    , current_light_left(nullptr)
    , current_light_right(nullptr)
    , light_radius_distance(_light_radius_distance)
{

}

void SUPGBrainPhototaxis::update(const std::vector< ActuatorPtr >& actuators,
                                 const std::vector< SensorPtr >& sensors,
                                 double t, double step)
{
    SUPGBrain::update(actuators, sensors, t, step);
}

SUPGBrainPhototaxis::SUPGBrainPhototaxis(EvaluatorPtr evaluator)
    : SUPGBrain(evaluator)
{

}

double revolve::brain::SUPGBrainPhototaxis::getFitness()
{
    double left_eye = current_light_left == nullptr ? std::numeric_limits<double>::max() :
                      current_light_left->read();
    double right_eye = current_light_right == nullptr ? std::numeric_limits<double>::max() :
                       current_light_right->read();

    return (left_eye + right_eye)
         - ((left_eye - right_eye) * (left_eye - right_eye));

//     return (left_eye + right_eye)
//          - std::fabs(left_eye - right_eye);
}
