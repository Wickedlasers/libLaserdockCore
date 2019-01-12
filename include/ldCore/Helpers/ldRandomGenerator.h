/**
    libLaserdockCore
    Copyright(c) 2018 Wicked Lasers

    This file is part of libLaserdockCore.

    libLaserdockCore is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libLaserdockCore is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libLaserdockCore.  If not, see <https://www.gnu.org/licenses/>.
**/

#ifndef LDRANDOMGENERATOR_H
#define LDRANDOMGENERATOR_H

#include <random>

#include "ldCore/ldCore_global.h"

/** Simple Random generator **/
class LDCORESHARED_EXPORT ldRandomGenerator
{
public:
    static ldRandomGenerator *instance();

    /** Generate random number in range**/
    int generate(int from, int to);
    float generatef(float from, float to);

private:
    explicit ldRandomGenerator();

    std::random_device rd;
    std::mt19937 rng;
};

#endif // LDRANDOMGENERATOR_H
