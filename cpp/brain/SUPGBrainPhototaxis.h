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

#ifndef REVOLVE_BRAIN_SUPGBRAINPHOTOTAXIS_H
#define REVOLVE_BRAIN_SUPGBRAINPHOTOTAXIS_H

#include "FakeLightSensor.h"
#include "SUPGBrain.h"

namespace revolve {
namespace brain {

class SUPGBrainPhototaxis : protected SUPGBrain
{
  public:
  /**
   * Light sensor constructors are passed the coordinates with already the offset included
   */
  SUPGBrainPhototaxis(const std::string &robot_name,
                      EvaluatorPtr evaluator,
                      std::function<boost::shared_ptr<FakeLightSensor>(std::vector<float> coordinates)> _light_constructor_left,
                      std::function<boost::shared_ptr<FakeLightSensor>(std::vector<float> coordinates)> _light_constructor_right,
                      double light_radius_distance,
                      const std::vector<std::vector<float> > &neuron_coordinates,
                      const std::vector<ActuatorPtr> &actuators,
                      const std::vector<SensorPtr> &sensors);

  using SUPGBrain::update;
  virtual void update(const std::vector<ActuatorPtr> &actuators,
                      const std::vector<SensorPtr> &sensors,
                      double t, double step) override;

  protected:
  SUPGBrainPhototaxis(EvaluatorPtr evaluator);

  virtual double getFitness() override;

  virtual double getPhaseFitness();

  virtual void learner(double t) override;

  //// Templates ---------------------------------------------------------

  enum PHASE
  {
    CENTER = 0,
    LEFT = 1,
    MORELEFT = 2,
    RIGHT = 3,
    MORERIGHT = 4,
    END = 5,
  } phase;

  std::function<boost::shared_ptr<FakeLightSensor>(std::vector<float> coordinates)>
          light_constructor_left,
          light_constructor_right;

  boost::shared_ptr<FakeLightSensor> current_light_left, current_light_right;

  double light_radius_distance;
  double partial_fitness;
};

}
}

#endif // REVOLVE_BRAIN_SUPGBRAINPHOTOTAXIS_H
