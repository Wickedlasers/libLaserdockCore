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

#include "ldCore/Helpers/BezierCurve/ld3dBezierCurve.h"

#include <ldCore/Helpers/Maths/ldMaths.h>


ldVec3 ld3dBezierCurve::getPoint(float slope) const
{
    ldVec3 r;
    r.x = ldMaths::cubicBezier(slope, start.x, control1.x, control2.x, end.x);
    r.y = ldMaths::cubicBezier(slope, start.y, control1.y, control2.y, end.y);
    r.z = ldMaths::cubicBezier(slope, start.z, control1.z, control2.z, end.z);
    return r;
}

float ld3dBezierCurve::length(int maxPoints) const
{
    float res = 0;
    for (int i=0; i<maxPoints-1; i++)
    {
        float slope_i = 1.0f*i/(maxPoints-1);
        float slope_ib = 1.0f*(i+1)/(maxPoints-1);
        ldVec3 p_i = getPoint(slope_i);
        ldVec3 p_ib = getPoint(slope_ib);

        res += sqrtf( (p_ib.x-p_i.x)*(p_ib.x-p_i.x) + (p_ib.y-p_i.y)*(p_ib.y-p_i.y) + (p_ib.z-p_i.z)*(p_ib.z-p_i.z) );
    }
    return res;
}

float ld3dBezierCurve::lengthFast() const
{
    float chord = end.distance(start);

    float cont_net = start.distance(control1)
            + control2.distance(control1)
            + end.distance(control2);

    float app_arc_length = (cont_net + chord) / 2;
    return app_arc_length;
}


