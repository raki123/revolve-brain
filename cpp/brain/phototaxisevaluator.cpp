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

#include "phototaxisevaluator.h"

using namespace revolve::brain;

double PhototaxisEvaluator::fitness()
{
    return fitness_center
         + fitness_left
         + fitness_more_left
         + fitness_right
         + fitness_more_right;
}

void PhototaxisEvaluator::start()
{
    reset();
}

void PhototaxisEvaluator::reset()
{
    phase = CENTER;
    fitness_center = 0;
    fitness_left = 0;
    fitness_more_left = 0;
    fitness_right = 0;
    fitness_more_right = 0;
    time_arrived = -1;
}

void PhototaxisEvaluator::set_arrived(float time)
{
    time_arrived = time;
}


PhototaxisEvaluator::PHASE PhototaxisEvaluator::nextPhase()
{
    double *fitness = nullptr;
    switch (phase) {
        case CENTER:
            fitness = &fitness_center;
            break;
        case LEFT:
            fitness = &fitness_left;
            break;
        case MORELEFT:
            fitness = &fitness_more_left;
            break;
        case RIGHT:
            fitness = &fitness_right;
            break;
        case MORERIGHT:
            fitness = &fitness_more_right;
            break;
        case END:
            throw std::string("CURRENT PHASE IS END, THERE IS NO NEXT PHASE");
    }

    fitness = 0;

    //
    if (time_arrived > 0) {
        fitness = 0;
    }
    else
    {

    }

    phase = static_cast<PHASE>(phase + 1);

    if (phase == PHASE::END)
        throw std::string("NEXT PHASE DOESN'T EXIST");

    time_arrived = -1;

    return phase;
}


float PhototaxisEvaluator::ANGLE(PhototaxisEvaluator::PHASE phase)
{
    static const float center_angle = 37.5;
    static const float more_angle = 75;

    switch (phase) {
        case CENTER:
            return 0;
        case LEFT:
            return -center_angle;
        case RIGHT:
            return center_angle;
        case MORELEFT:
            return -more_angle;
        case MORERIGHT:
            return more_angle;
        case END:
            throw std::string("no angle for end phase");
    }
}

float PhototaxisEvaluator::DISTANCE(PhototaxisEvaluator::PHASE phase)
{
    return 0.5;
}

