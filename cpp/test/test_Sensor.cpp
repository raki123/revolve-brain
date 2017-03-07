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

#include "test_Sensor.h"
#include <iostream>

TestSensor::TestSensor(bool verbose)
    : verbose(verbose)
    , rd()
    , gen(rd())
    , dis(0, 2)
{
}

std::string TestSensor::sensorId() const
{
    return std::string("TestSensor");
}

unsigned int TestSensor::inputs() const
{
    return 1;
}

void TestSensor::read(double* input_vector)
{
    input_vector[0] = dis(gen);

    if (verbose)
        std::cout << "TestSensor::read <- " << input_vector[0] << std::endl;
}

