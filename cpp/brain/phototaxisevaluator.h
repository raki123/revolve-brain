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

#ifndef REVOLVE_BRAIN_PHOTOTAXISEVALUATOR_H
#define REVOLVE_BRAIN_PHOTOTAXISEVALUATOR_H

#include "evaluator.h"

namespace revolve {
namespace brain {

class PhototaxisEvaluator : public revolve::brain::Evaluator
{
public:
    enum PHASE {
        CENTER = 0,
        LEFT = 1,
        MORELEFT = 2,
        RIGHT = 3,
        MORERIGHT = 4,
        END = 5,
    };

    PhototaxisEvaluator() {
        reset();
    }


    virtual double light_distance() = 0;
    virtual void set_arrived(float time);
    virtual PHASE nextPhase();

    virtual void reset();

    virtual double fitness() override;

    virtual void start() override;

    /** angle of the light for each phase in degrees
     * CENTER -> 0
     * LEFT -> NEGATIVE
     * RIGH -> POSITIVE
     */
    static float ANGLE(PHASE phase);

    /** in meters */
    static float DISTANCE(PHASE phase);

protected:
    PHASE phase;
    double fitness_center,
           fitness_left,
           fitness_more_left,
           fitness_right,
           fitness_more_right;
    float time_arrived;
};

}
}

#endif // REVOLVE_BRAIN_PHOTOTAXISEVALUATOR_H
