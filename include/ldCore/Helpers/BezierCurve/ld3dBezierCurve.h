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

#ifndef LD3DBEZIERCURVE_H
#define LD3DBEZIERCURVE_H

#include "ldCore/Helpers/Maths/ldVec3.h"

struct LDCORESHARED_EXPORT ld3dBezierCurve {
    ldVec3 start;
    ldVec3 end;
    ldVec3 control1;
    ldVec3 control2;
    uint32_t color = 0;
    ldVec3 pivot;

    ldVec3 getPoint(float slope) const;
};

typedef std::vector< std::vector<ld3dBezierCurve> > ld3dBezierCurves;
typedef std::vector< ld3dBezierCurves> ld3dBezierCurvesSequence;

#endif // LD3DBEZIERCURVE_H

