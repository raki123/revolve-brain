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

#include <functional>
#include <cmath>

using namespace revolve::brain;

SUPGBrainPhototaxis::SUPGBrainPhototaxis(EvaluatorPtr evaluator,
                                         std::function<FakeLightSensor *(std::vector<float> coordinates)> _light_constructor_left,
                                         std::function<FakeLightSensor *(std::vector<float> coordinates)> _light_constructor_right,
                                         double _light_radius_distance,
                                         const std::vector< std::vector< float > >& neuron_coordinates,
                                         const std::vector< ActuatorPtr >& actuators,
                                         const std::vector< SensorPtr >& sensors)
    : SUPGBrain(evaluator, neuron_coordinates, actuators, sensors)
    , phase(END)
    , light_constructor_left(_light_constructor_left)
    , light_constructor_right(_light_constructor_right)
    , current_light_left(nullptr)
    , current_light_right(nullptr)
    , light_radius_distance(_light_radius_distance)
    , partial_fitness(0)
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

double SUPGBrainPhototaxis::getFitness()
{
    return partial_fitness;
}


double SUPGBrainPhototaxis::getPhaseFitness()
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

void SUPGBrainPhototaxis::learner(double t)
{

    // Evaluate policy on certain time limit
    if (!this->isOffline()
            && (t-start_eval_time) > SUPGBrain::FREQUENCY_RATE)
    {
        // check if to stop the experiment. Negative value for MAX_EVALUATIONS will never stop the experiment
        if (SUPGBrain::MAX_EVALUATIONS > 0 && generation_counter > SUPGBrain::MAX_EVALUATIONS) {
            std::cout << "Max Evaluations (" << SUPGBrain::MAX_EVALUATIONS << ") reached. stopping now." << std::endl;
            std::exit(0);
        }


        // FITNESS update
        if (phase != END) {
            partial_fitness += getPhaseFitness();
        }

        // Advance Phase
        switch (phase) {
            case CENTER:    phase = LEFT;      break;
            case LEFT:      phase = RIGHT;     break;
            case RIGHT:     phase = MORELEFT;  break;
            case MORELEFT:  phase = MORERIGHT; break;
            case MORERIGHT: phase = END;       break;
            case END:
                std::cout << "SUPGBrainPhototaxis::learner - INIT! (2)" << std::endl;
        }

        if (phase == END) {
            std::cout << "SUPGBrainPhototaxis::learner - NEW BRAIN (generation " << generation_counter << " )" << std::endl;

            generation_counter++;
            this->nextBrain();
            partial_fitness = 0;

            phase = CENTER;
        }

        // reposition learner lights
        delete current_light_left;
        delete current_light_right;
        std::vector<float> relative_coordinates;

        static const double pi = std::acos(-1);
        static const double angle_15 = pi/12;
        static const double angle_52_5 = 7*pi/24;

        const double radius = light_radius_distance;
        const float x_52_5 = std::cos(angle_52_5) * radius;
        const float y_52_5 = std::sin(angle_52_5) * radius;
        const float x_15   = std::cos(angle_15) * radius;
        const float y_15   = std::sin(angle_15) * radius;

        switch (phase) {
        case CENTER:
            relative_coordinates = {0, static_cast<float>(radius)};
            break;
        case LEFT:
            relative_coordinates = {-x_52_5, y_52_5};
            break;
        case RIGHT:
            relative_coordinates = { x_52_5, y_52_5};
            break;
        case MORELEFT:
            relative_coordinates = {-x_15, y_15};
            break;
        case MORERIGHT:
            relative_coordinates = { x_15, y_15};
            break;
        case END:
            std::cout << "SUPGBrainPhototaxis::learner - INIT!" << std::endl;
        }

        current_light_left = light_constructor_left(relative_coordinates);
        current_light_right = light_constructor_right(relative_coordinates);

        // evaluation restart
        start_eval_time = t;
        evaluator->start();
    }
}
