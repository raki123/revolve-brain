/*
 * Copyright (C) 2015-2017 Vrije Universiteit Amsterdam
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description: TODO: <Add brief description about file purpose>
 * Author: Milan Jelisavcic
 * Date: August 2, 2017.
 *
 */

#include "RythmGenerationCPG.h"

#include <cmath>
#include <iostream>
#include <sstream>

namespace revolve
{
  namespace brain
  {

    RythmGenerationCPG::RythmGenerationCPG(const std::string &id,
                                           const std::map<std::string, double> &params)
            : Neuron(id)
    {
      if (not params.count("rv:bias"))
      {
        std::cerr
                << "A `"
                << "Differential CPG"
                << "` neuron requires `rv:bias` element."
                << std::endl;
        throw std::runtime_error("Robot brain error");
      }
      this->bias_ = params.find("rv:bias")->second;
      lastTime_ = 0;
    }


    double RythmGenerationCPG::CalculateOutput(double t)
    {
      double deltaT = t - lastTime_;
      lastTime_ = t;

      if (deltaT > 0.1) deltaT = 0.1;

      double inputValue = 0;

      for (auto it = this->incomingConnections_.begin(); it != this->incomingConnections_.end(); ++it)
      {
        auto inConnection = it->second;
        inputValue += inConnection->GetInputNeuron()->Output() * inConnection->GetWeight();
      }

      double state_deriv = inputValue - this->bias_;
      double result = this->output_ + deltaT * state_deriv;

      double maxOut = 10000.0;

      // limit output:
      // if (result > maxOut) {
      // 	result = maxOut;
      // }
      // else if (result < -maxOut) {
      // 	result = -maxOut;
      // }

      // saturate output:
      double gain = 2.0 / maxOut;
      result = (2.0 / (1.0 + std::exp(-result * gain)) - 1.0) * maxOut;

      return result;
    }

    std::map<std::string, double> RythmGenerationCPG::Parameters()
    {
      std::map<std::string, double> ret;
      ret["rv:bias"] = bias_;
      return ret;
    }

    void RythmGenerationCPG::setNeuronParameters(std::map<std::string, double> params)
    {
      if (not params.count("rv:bias"))
      {
        std::cerr
                << "A `"
                << "Differential CPG"
                << "` neuron requires `rv:bias` element."
                << std::endl;
        throw std::runtime_error("Robot brain error");
      }
      this->bias_ = params.find("rv:bias")
                          ->second;
    }

    std::string RythmGenerationCPG::getType()
    {
      return "DifferentialCPG";
    }


  }
}