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

// ldMaths.cpp
//  Created by Eric Brugère on 4/12/16.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.

#include "ldCore/Helpers/Maths/ldVec3.h"

#include <math.h>

#include "ldCore/Helpers/Maths/ldMaths.h"

const ldVec3 ldVec3::X_VECTOR = ldVec3(1.0f, 0.0f, 0.f);
const ldVec3 ldVec3::Y_VECTOR = ldVec3(0.0f, 1.0f, 0.f);
const ldVec3 ldVec3::Z_VECTOR = ldVec3(0.0f, 0.0f, 1.f);
const ldVec3 ldVec3::X_NEG_VECTOR = ldVec3(-1.0f, 0.0f, 0.f);
const ldVec3 ldVec3::Y_NEG_VECTOR = ldVec3(0.0f, -1.0f, 0.f);
const ldVec3 ldVec3::Z_NEG_VECTOR = ldVec3(0.0f, 0.0f, -1.f);
const ldVec3 ldVec3::ZERO_VECTOR = ldVec3(0.0f, 0.0f, 0.f);


// rotate3d
ldVec3 ldVec3::rotate3d(const ldVec3 &p, float angle, const ldVec3 &axis)
{
    //
    ldVec3 output;
    output.x=p.x;
    output.y=p.y;
    output.z=p.z;

    //
    angle = ldMaths::periodIntervalKeeper(angle, 0.0, M_2PIf);
    float rotationMatrix[3][3];
    float inputMatrix[3][1] = {{0.0}, {0.0}, {0.0}};
    float outputMatrix[3][1] = {{0.0}, {0.0}, {0.0}};
    outputMatrix[0][0] = 0.0;
    outputMatrix[1][0] = 0.0;
    outputMatrix[2][0] = 0.0;

    float u=axis.x;
    float v=axis.y;
    float w=axis.z;
    float u2 = u * u;
    float v2 = v * v;
    float w2 = w * w;
    float L = (u2 + v2 + w2);
    if (L<1.0e-4f) return p;

    // rotationMatrix
    rotationMatrix[0][0] = (u2 + (v2 + w2) * cosf(angle)) / L;
    rotationMatrix[0][1] = (u * v * (1.0f - cosf(angle)) - w * sqrtf(L) * sinf(angle)) / L;
    rotationMatrix[0][2] = (u * w * (1.0f - cosf(angle)) + v * sqrtf(L) * sinf(angle)) / L;

    rotationMatrix[1][0] = (u * v * (1.0f - cosf(angle)) + w * sqrtf(L) * sinf(angle)) / L;
    rotationMatrix[1][1] = (v2 + (u2 + w2) * cosf(angle)) / L;
    rotationMatrix[1][2] = (v * w * (1.0f - cosf(angle)) - u * sqrtf(L) * sinf(angle)) / L;

    rotationMatrix[2][0] = (u * w * (1.0f - cosf(angle)) - v * sqrtf(L) * sinf(angle)) / L;
    rotationMatrix[2][1] = (v * w * (1.0f - cosf(angle)) + u * sqrtf(L) * sinf(angle)) / L;
    rotationMatrix[2][2] = (w2 + (u2 + v2) * cosf(angle)) / L;

    //
    inputMatrix[0][0] = p.x;
    inputMatrix[1][0] = p.y;
    inputMatrix[2][0] = p.z;

    // rotate
    for(int i = 0; i < 3; i++ )
    {
        for (int j = 0; j < 1; j++){
            outputMatrix[i][j] = 0;
            for (int k = 0; k < 3; k++)
            {
                outputMatrix[i][j] += rotationMatrix[i][k] * inputMatrix[k][j];
            }
        }
    }

    // output
    output.x=outputMatrix[0][0];
    output.y=outputMatrix[1][0];
    output.z=outputMatrix[2][0];
    return output;
}

// rotate3dAtPoint
ldVec3 ldVec3::rotate3dAtPoint(const ldVec3 &p, float angle, const ldVec3 &axis, const ldVec3 &point)
{
    ldVec3 tmp = p - point;
    tmp = rotate3d(tmp, angle, axis);
    tmp += point;
    return tmp;
}


ldVec3::ldVec3()
{
}

ldVec3::ldVec3(float p_x, float p_y, float p_z)
    : x(p_x)
    , y(p_y)
    , z(p_z)
{

}

bool ldVec3::isNull() const
{
    return x == 0.f && y == 0.f && z == 0.f;
}

float ldVec3::distance(const ldVec3 &n) const
{
    return sqrtf(powf((x-n.x), 2) + powf((y-n.y), 2) + powf((z-n.z), 2));
}

void ldVec3::norm()
{
    float d = distance(ZERO_VECTOR);
    if(cmpf(d, 0.f))
        return;

    *this /= d;
}

float ldVec3::dotProduct(const ldVec3 &v) const
{
    return x*v.x + y*v.y + z*v.z;
}

ldVec3 ldVec3::vectProduct(const ldVec3 &v) const
{
    ldVec3 res;
    res.x=y*v.z-z*v.y;
    res.y=z*v.x-x*v.z;
    res.z=x*v.y-y*v.x;
    return res;
}

ldVec3 ldVec3::toLaserCoord() const
{
    ldVec3 result;
    result.x = ldMaths::unitedToLaserCoords(x);
    result.y = ldMaths::unitedToLaserCoords(y);
//    result.z = ldMaths::unitedToLaserCoords(z);
    return result;
}

void ldVec3::rotate(float p_x, float p_y, float p_z, ldVec3 p_pivot)
{
    ldVec3 r = *this;
    r = ldVec3::rotate3dAtPoint(r, p_x, ldVec3::X_VECTOR, p_pivot);
    r = ldVec3::rotate3dAtPoint(r, p_y, ldVec3::Y_VECTOR, p_pivot);
    r = ldVec3::rotate3dAtPoint(r, p_z, ldVec3::Z_VECTOR, p_pivot);
    *this = r;
}

ldVec3 ldVec3::operator+(const ldVec3 &other) const
{
    ldVec3 res = *this;
    res += other;
    return res;
}

ldVec3 &ldVec3::operator+=(const ldVec3 &other)
{
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
}

ldVec3 ldVec3::operator-(const ldVec3 &other) const
{
    ldVec3 res = *this;
    res -= other;
    return res;
}

ldVec3 &ldVec3::operator-=(const ldVec3 &other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;
}

ldVec3 ldVec3::operator*(float coef) const
{
    ldVec3 res = *this;
    res *= coef;
    return res;
}

ldVec3 &ldVec3::operator*=(float coef)
{
    x *= coef;
    y *= coef;
    z *= coef;

    return *this;
}

ldVec3 ldVec3::operator/(float coef) const
{
    ldVec3 res = *this;
    res /= coef;
    return res;
}

ldVec3 &ldVec3::operator/=(float coef)
{
    x /= coef;
    y /= coef;
    z /= coef;

    return *this;
}

