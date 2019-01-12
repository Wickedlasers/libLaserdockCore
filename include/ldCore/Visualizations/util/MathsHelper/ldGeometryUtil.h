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
//  ldGeometryUtil.h
//  ldCore
//
//  Created by feldspar on 2/5/15.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//

#ifndef __ldCore__ldGeometryUtil__
#define __ldCore__ldGeometryUtil__

#include "ldCore/ldCore_global.h"

#define TWOPI 6.28318530718f
#define TAU TWOPI

class LDCORESHARED_EXPORT ldControlValue {
public:
    char* name;
    float* pointer;
    float min;
    float value;
    float max;
    float increment;
    ldControlValue();
    ldControlValue(char* _name, float* _pointer, float _min, float _value, float _max, float _increment)
        : name(_name)
        , pointer(_pointer)
        , min(_min)
        , value(_value)
        , max(_max)
        , increment(_increment)
    {}

    void more();
    void less();
};

inline void resetControls() {}
inline void addControl(ldControlValue cv) {*(cv.pointer) = cv.value;}

#include "ldCore/Shape/ldParticleGeometry.h"
inline float fscale(float f, float min, float max) {
    return min+(max-min)*clampf(f,0,1);
}
inline float floorf2(float f) {return (int)f;}
inline int iscale(float f, int min, int max) {
    return floorf2(0.00f+(min+(max-min+1.0f)*clampf(f,0.001f,0.999f)));
}
inline float clampfp(float &fp, float min, float max) {
    if (!(fp > min)) fp = min;
    if (!(fp < max)) fp = max;
   return fp;
}

inline float InterpolateCubicF(float f1, float f2, float f3, float f4, float factor)
{
    //return (f2 + (f3-f2)*factor);///f3*factor + f2 - f2*factor));///
    float a, b, c, d; // f(x) = ax^3+bx^2+cx+d
    float s2, s3; // slope at f2, slope at f3
    s2 = (f3 - f1)/2;
    s3 = (f4 - f2)/2;
    c = s2;
    d = f2;
    a = (s3 - s2) - 2*(f3 - c - d);
    b = f3 - c - d - a;
    return (factor*(factor*(a*factor + b) + c) + d);
}

class LDCORESHARED_EXPORT particle2d {
public:
    float x, y;
    float vx, vy;
    float ax, ay;
    void advance (float t);
};

class LDCORESHARED_EXPORT particle3d {
public:
    float x, y, z;
    float vx, vy, vz;
    float ax, ay, az;
    void advance (float t) ;
};

LDCORESHARED_EXPORT float angleIncrement(float& angle, float increment);
LDCORESHARED_EXPORT float turnsIncrement(float& angle, float increment);

#include <math.h>
inline float rcost(float f) {return 0.5+0.5*cosf(TAU*f);}
inline float rcostc(float f) {return 1-rcost(f);}
inline float rsint(float f) {return 0.5+0.5*sinf(TAU*f);}

#endif /* defined(__ldCore__ldGeometryUtil__) */
