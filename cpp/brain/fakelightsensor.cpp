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

#include "fakelightsensor.h"
#include <cmath>

using namespace revolve::brain;

unsigned int FakeLightSensor::inputs() const
{
    return 1;
}

double revolve::brain::FakeLightSensor::read()
{
    return light_attenuation(light_distance(), light_angle());
}


void FakeLightSensor::read(double* input_vector)
{
    input_vector[0] = read();
}

float revolve::brain::FakeLightSensor::light_attenuation(float distance, float angle)
{
    float abs_angle = std::fabs(angle);

    if (abs_angle > half_fov)
        return 0;

    //TODO noise
    return distance;
}
