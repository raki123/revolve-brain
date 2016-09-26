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

#ifndef SUPGBRAIN_PYTHON_H
#define SUPGBRAIN_PYTHON_H

#include "brain/supgbrain.h"
#include <boost/python/list.hpp>
#include <vector>

namespace revolve {
namespace brain {

class SUPGBrain_python : public revolve::brain::SUPGBrain
{
public:
    SUPGBrain_python(revolve::brain::EvaluatorPtr evaluator,
                     std::vector< std::vector< float > > neuron_coordinates,
                     std::vector< ActuatorPtr> &motors,
                     const std::vector< SensorPtr> &sensors);


    void update(boost::python::list& actuators,
                const boost::python::list& sensors,
                double t, double step);

};

}
}

#endif // SUPGBRAIN_PYTHON_H
