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
//  ldAffineTransform.cpp
//  LaserdockEngine
//
//  Created by rock on 1/6/14.
//  Copyright (c) 2014 Wicked Lasers. All rights reserved.
//

#include "ldCore/Shape/ldAffineTransform.h"

#include <algorithm>
#include <math.h>

ldAffineTransform __ldAffineTransformMake(float a, float b, float c, float d, float tx, float ty)
{
    ldAffineTransform t;
    t.a = a; t.b = b; t.c = c; t.d = d; t.tx = tx; t.ty = ty;
    return t;
}

CCPoint __CCPointApplyAffineTransform(const CCPoint& point, const ldAffineTransform& t)
{
    CCPoint p;
    p.x = (float)((double)t.a * point.x + (double)t.c * point.y + t.tx);
    p.y = (float)((double)t.b * point.x + (double)t.d * point.y + t.ty);
    return p;
}

CCSize __CCSizeApplyAffineTransform(const CCSize& size, const ldAffineTransform& t)
{
    CCSize s;
    s.width = (float)((double)t.a * size.width + (double)t.c * size.height);
    s.height = (float)((double)t.b * size.width + (double)t.d * size.height);
    return s;
}


ldAffineTransform ldAffineTransformMakeIdentity()
{
    return __ldAffineTransformMake(1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
}

extern const ldAffineTransform ldAffineTransformIdentity = ldAffineTransformMakeIdentity();


ldAffineTransform ldAffineTransformTranslate(const ldAffineTransform& t, float tx, float ty)
{
    return __ldAffineTransformMake(t.a, t.b, t.c, t.d, t.tx + t.a * tx + t.c * ty, t.ty + t.b * tx + t.d * ty);
}

ldAffineTransform ldAffineTransformScale(const ldAffineTransform& t, float sx, float sy)
{
    return __ldAffineTransformMake(t.a * sx, t.b * sx, t.c * sy, t.d * sy, t.tx, t.ty);
}

ldAffineTransform ldAffineTransformRotate(const ldAffineTransform& t, float anAngle)
{
    float fSin = sin(anAngle);
    float fCos = cos(anAngle);
    
    return __ldAffineTransformMake(    t.a * fCos + t.c * fSin,
                                   t.b * fCos + t.d * fSin,
                                   t.c * fCos - t.a * fSin,
                                   t.d * fCos - t.b * fSin,
                                   t.tx,
                                   t.ty);
}

/* Concatenate `t2' to `t1' and return the result:
 t' = t1 * t2 */
ldAffineTransform ldAffineTransformConcat(const ldAffineTransform& t1, const ldAffineTransform& t2)
{
    return __ldAffineTransformMake(    t1.a * t2.a + t1.b * t2.c, t1.a * t2.b + t1.b * t2.d, //a,b
                                   t1.c * t2.a + t1.d * t2.c, t1.c * t2.b + t1.d * t2.d, //c,d
                                   t1.tx * t2.a + t1.ty * t2.c + t2.tx,                  //tx
                                   t1.tx * t2.b + t1.ty * t2.d + t2.ty);                  //ty
}

/* Return true if `t1' and `t2' are equal, false otherwise. */
bool ldAffineTransformEqualToTransform(const ldAffineTransform& t1, const ldAffineTransform& t2)
{
    return (t1.a == t2.a && t1.b == t2.b && t1.c == t2.c && t1.d == t2.d && t1.tx == t2.tx && t1.ty == t2.ty);
}

ldAffineTransform ldAffineTransformInvert(const ldAffineTransform& t)
{
    float determinant = 1 / (t.a * t.d - t.b * t.c);
    
    return __ldAffineTransformMake(determinant * t.d, -determinant * t.b, -determinant * t.c, determinant * t.a,
                                   determinant * (t.c * t.ty - t.d * t.tx), determinant * (t.b * t.tx - t.a * t.ty) );
}
