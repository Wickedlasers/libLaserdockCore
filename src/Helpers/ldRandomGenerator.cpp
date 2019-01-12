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

#include <ldCore/Helpers/ldRandomGenerator.h>

ldRandomGenerator::ldRandomGenerator()
    : rng(rd())
{
}

ldRandomGenerator *ldRandomGenerator::instance()
{
    static ldRandomGenerator instance;
    return &instance;
}

int ldRandomGenerator::generate(int from, int to)
{
    std::uniform_int_distribution<int> uni(from, to); // guaranteed unbiased
    return uni(rng);
}

float ldRandomGenerator::generatef(float from, float to)
{
    std::uniform_real_distribution<float> uni(from, to); // guaranteed unbiased
    return uni(rng);
}
