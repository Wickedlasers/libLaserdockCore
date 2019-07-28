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

#include <math.h>

#include "ldCore/ldCore_global.h"

const float M_PIf = static_cast<float> (M_PI);
const float M_PI_2f = static_cast<float> (M_PI_2);
const float M_PI_4f = static_cast<float> (M_PI_4);
const double M_2PI  = 2.0*M_PI;
const float M_2PIf = static_cast<float> (M_2PI);

LDCORESHARED_EXPORT bool cmpf(float a, float b, float epsilon = 0.005f);

#endif // LDGLOBALS_H


