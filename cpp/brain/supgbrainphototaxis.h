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

#ifndef SUPGBRAINPHOTOTAXIS_H
#define SUPGBRAINPHOTOTAXIS_H

#include "fakelightsensor.h"
#include "supgbrain.h"
#include <functional>
#include <cmath>

namespace revolve {
namespace brain {

class SUPGBrainPhototaxis :  SUPGBrain
{
public:
    /**
     * Light sensor constructors are passed the coordinates with already the offset included
     */
    SUPGBrainPhototaxis(EvaluatorPtr evaluator,
                        std::function<FakeLightSensor *(std::vector<float> coordinates)> _light_constructor_left,
                        std::function<FakeLightSensor *(std::vector<float> coordinates)> _light_constructor_right,
                        double light_radius_distance,
                        const std::vector< std::vector< float > >& neuron_coordinates,
                        const std::vector< ActuatorPtr >& actuators,
                        const std::vector< SensorPtr >& sensors);

    virtual void update(const std::vector< ActuatorPtr >& actuators,
                        const std::vector< SensorPtr >& sensors,
                        double t, double step) override;

protected:
    SUPGBrainPhototaxis(EvaluatorPtr evaluator);

    virtual double getFitness() override;

    //// Templates ---------------------------------------------------------
    template<typename SensorContainer>
    void learner(const SensorContainer &sensors, double t)
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

            generation_counter++;
            std::cout << "################# EVALUATING NEW BRAIN !!!!!!!!!!!!!!!!!!!!!!!!! (generation " << generation_counter << " )" << std::endl;
            this->nextBrain();

            //TODO control phase here
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
                    std::cerr << "TODO start new brain" << std::endl;
            }

            current_light_left = light_constructor_left(relative_coordinates);
            current_light_right = light_constructor_right(relative_coordinates);

            start_eval_time = t;
            evaluator->start();
        }
    }

    template<typename ActuatorContainer, typename SensorContainer>
    void update(const ActuatorContainer &actuators,
                const SensorContainer &sensors,
                double t,
                double step)
    {
        SUPGBrainPhototaxis::learner<SensorContainer>(sensors, t);
        SUPGBrain::controller<ActuatorContainer, SensorContainer>(actuators, sensors, t, step);
    }

    enum PHASE {
        CENTER = 0,
        LEFT = 1,
        MORELEFT = 2,
        RIGHT = 3,
        MORERIGHT = 4,
        END = 5,
    } phase;

    std::function<FakeLightSensor *(std::vector<float> coordinates)> light_constructor_left,
                                                                     light_constructor_right;

    FakeLightSensor *current_light_left,
                    *current_light_right;

    double light_radius_distance;
};

}
}

#endif // SUPGBRAINPHOTOTAXIS_H
