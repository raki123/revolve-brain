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

#ifndef REVOLVEBRAIN_BRAIN_SUPGG_SUPGGENOMEMANAGER_H_
#define REVOLVEBRAIN_BRAIN_SUPGG_SUPGGENOMEMANAGER_H_

#include "innovgenome/innovgenomemanager.h"

class SUPGGenomeManager
        : public NEAT::InnovGenomeManager
{
public:
    SUPGGenomeManager(const std::string &robot_name);

    virtual std::vector<std::unique_ptr<NEAT::Genome>>
    create_seed_generation(size_t ngenomes,
                           NEAT::rng_t rng,
                           size_t ntraits,
                           size_t ninputs,
                           size_t noutputs,
                           size_t nhidden,
                           const std::string &robot_name) override;
};

#endif // REVOLVEBRAIN_BRAIN_SUPGG_SUPGGENOMEMANAGER_H_
