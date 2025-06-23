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

#ifndef LDGLOBALS_H
#define LDGLOBALS_H

#include <cmath>

#include "ldCore/ldCore_global.h"

#ifndef M_PIf
constexpr float M_PIf = static_cast<float> (M_PI);
#endif

#ifndef M_PI_2f
constexpr float M_PI_2f = static_cast<float> (M_PI_2);
#endif

#ifndef M_PI_4f
constexpr float M_PI_4f = static_cast<float> (M_PI_4);
#endif

constexpr double M_2PI  = 2.0*M_PI;
constexpr float M_2PIf = static_cast<float> (M_2PI);

inline LDCORESHARED_EXPORT bool cmpf(float a, float b, float epsilon = 0.005f)
{
    return (fabsf(a - b) < epsilon);
}

#endif // LDGLOBALS_H


