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

#ifndef LDVEC3_H
#define LDVEC3_H

#include "ldCore/ldCore_global.h"

struct LDCORESHARED_EXPORT ldVec3
{
    static ldVec3 rotate3d(const ldVec3 &p, float angle, const ldVec3 &axis);
    static ldVec3 rotate3dAtPoint(const ldVec3 &p, float angle, const ldVec3 &axis, const ldVec3 &point);

    const static ldVec3 X_VECTOR;
    const static ldVec3 Y_VECTOR;
    const static ldVec3 Z_VECTOR;
    const static ldVec3 X_NEG_VECTOR;
    const static ldVec3 Y_NEG_VECTOR;
    const static ldVec3 Z_NEG_VECTOR;
    const static ldVec3 ZERO_VECTOR;

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

    explicit ldVec3();
    explicit ldVec3(float p_x, float p_y, float p_z);
    bool isNull() const;

    float distance(const ldVec3 &n) const;
    void norm();

    float dotProduct(const ldVec3 &v) const;
    ldVec3 vectProduct(const ldVec3 &v) const;

    ldVec3 toLaserCoord() const;

    void rotate(float p_x, float p_y, float p_z, ldVec3 p_pivot);
    void rotate(const ldVec3 &k, float theta);

    ldVec3 operator+ (const ldVec3& other) const;
    ldVec3& operator+= (const ldVec3& other);
    ldVec3 operator- (const ldVec3& other) const;
    ldVec3& operator-= (const ldVec3& other);
    ldVec3 operator* (float coef) const;
    ldVec3& operator*= (float coef);
    ldVec3 operator/ (float coef) const;
    ldVec3& operator/= (float coef);

};

#endif // LDVEC3_H


