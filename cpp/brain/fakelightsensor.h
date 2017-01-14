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

#ifndef REVOLVE_BRAIN_FAKELIGHT_H
#define REVOLVE_BRAIN_FAKELIGHT_H

#include "sensor.h"
#include <iostream>

namespace revolve {
namespace brain {

class FakeLightSensor : public revolve::brain::Sensor
{
public:
    // disable default constructor
    FakeLightSensor() = delete;

    explicit FakeLightSensor(float fov)
        : half_fov(fov/2)
        // TODO noise
    {
//         std::cout << "revolve::brain::FakeLightSensor()" << std::endl;
    }

    virtual double read();
    virtual void read(double* input_vector) override;
    virtual unsigned int inputs() const override;

protected:
    virtual float light_distance() = 0;
    virtual float light_angle() = 0;

    virtual float light_attenuation(float distance, float angle);

    float half_fov;
};

}
}

#endif // REVOLVE_BRAIN_FAKELIGHT_H
