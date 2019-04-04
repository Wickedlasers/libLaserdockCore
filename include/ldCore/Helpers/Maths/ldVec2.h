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

#ifndef LDVEC2_H
#define LDVEC2_H

#include "ldCore/ldCore_global.h"

class LDCORESHARED_EXPORT ldVec2 {
public:
    static void registerMetaTypes();

    const static ldVec2 zero;
    const static ldVec2 one;
    const static ldVec2 minus_one;
    const static ldVec2 up;
    const static ldVec2 down;
    const static ldVec2 left;
    const static ldVec2 right;

    float x = 0.f;
    float y = 0.f;

    ldVec2();
    ldVec2(float p_x, float p_y);

    bool isNull() const;
    float distance(const ldVec2 &n) const;
    float magnitude() const;
    ldVec2 normalize() const;
    float toRadians() const;
    void rotate(float rotation); // (angle in radians)

    ldVec2 operator+ (const ldVec2& other) const;
    ldVec2& operator+= (const ldVec2& other);
    ldVec2 operator- (const ldVec2& other) const;
    ldVec2& operator-= (const ldVec2& other);
    ldVec2& operator*= (float s);
    ldVec2& operator*= (const ldVec2& other);
    ldVec2& operator/= (float s);
    ldVec2& operator/= (const ldVec2& other);
    bool operator == (const ldVec2& other) const;
    bool operator != (const ldVec2& other) const;
};
Q_DECLARE_METATYPE(ldVec2)

QDebug operator << (QDebug debug, const ldVec2& v);

#endif // LDVEC2_H


