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

//
//  ldGeometryUtil.cpp
//  ldCore
//
//  Created by feldspar on 2/5/15.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//

#include "ldCore/Helpers/Maths/ldGeometryUtil.h"

void particle2d::advance (float t) {
    x = x + vx*t + 0.5f*ax*t*t;
    y = y + vy*t + 0.5f*ay*t*t;
    vx = vx + ax*t;
    vy = vy + ay*t;
}

void particle3d::advance (float t) {
    x = x + vx*t + 0.5f*ax*t*t;
    y = y + vy*t + 0.5f*ay*t*t;
    z = z + vz*t + 0.5f*az*t*t;
    vx = vx + ax*t;
    vy = vy + ay*t;
    vz = vz + az*t;
}

float angleIncrement(float& angle, float increment) {
    angle += increment;
    if (angle >= TWOPI) angle -= TWOPI;
    if (angle <=     0) angle += TWOPI;
    return angle;
}

float turnsIncrement(float& angle, float increment) {
    angle += increment;
    if (angle >= 1) angle -= 1;
    if (angle <= 0) angle += 1;
    return angle;
}
