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

#include "ldCore/Helpers/Maths/ldVec2.h"

#include <cmath>

#include <QtCore/QMetaType>
#include <QtCore/QtDebug>

#include "ldCore/Helpers/Maths/ldMaths.h"

const ldVec2 ldVec2::zero = ldVec2(0.0f, 0.0f);
const ldVec2 ldVec2::one = ldVec2(1.0f, 1.0f);
const ldVec2 ldVec2::minus_one = ldVec2(-1.0f, -1.0f);
const ldVec2 ldVec2::up = ldVec2(0.0f, 1.0f);
const ldVec2 ldVec2::down = ldVec2(0.0f, -1.0f);
const ldVec2 ldVec2::left = ldVec2(-1.0f, 0.0f);
const ldVec2 ldVec2::right = ldVec2(1.0f, 0.0f);

void ldVec2::registerMetaTypes()
{
    QMetaType::registerDebugStreamOperator<ldVec2>();
}

ldVec2::ldVec2()
{
}

ldVec2::ldVec2(float p_x, float p_y)
    : x(p_x)
    , y(p_y)
{
}

bool ldVec2::isNull() const
{
    return cmpf(x, 0.f) && cmpf(y, 0.f);
}

float ldVec2::distance(const ldVec2 &n) const
{
    return sqrtf(powf((x-n.x), 2) + powf((y-n.y), 2));
}

float ldVec2::magnitude() const
{
    return sqrtf(powf(x, 2) + powf(y, 2));
}

ldVec2 ldVec2::normalize() const
{
    ldVec2 normalized;

    float m = magnitude();
    if (m > 0) {
        normalized = ldVec2(x / m, y / m);
    }

    return normalized;
}

float ldVec2::toRadians() const
{
    ldVec2 normalized = normalize();
    float angle = atan2(normalized.y, normalized.x);
    angle = (normalized.x > 0 ? angle : 2 * M_PIf + angle);

    return angle;
}

void ldVec2::rotate(float rotation)
{
    float oldX = x;
    x = x*cosf(rotation)-y*sinf(rotation);
    y = oldX*sinf(rotation)+y*cosf(rotation);
}

ldVec2 ldVec2::operator+ (const ldVec2& other) const
{
    ldVec2 res = *this;
    res += other;
    return res;
}

ldVec2 &ldVec2::operator+=(const ldVec2 &other)
{
    x += other.x;
    y += other.y;

    return *this;
}


ldVec2 ldVec2::operator- (const ldVec2& other) const
{
    ldVec2 res = *this;
    res -= other;
    return res;
}

ldVec2& ldVec2::operator-=(const ldVec2 &other)
{
    x -= other.x;
    y -= other.y;

    return *this;
}

ldVec2 &ldVec2::operator*=(float s)
{
    x *= s;
    y *= s;
    return *this;
}

ldVec2 &ldVec2::operator*=(const ldVec2 &other)
{
    x *= other.x;
    y *= other.y;
    return *this;
}

ldVec2 &ldVec2::operator/=(float s)
{
    x /= s;
    y /= s;
    return *this;
}

ldVec2 &ldVec2::operator/=(const ldVec2 &other)
{
    x /= other.x;
    y /= other.y;
    return *this;
}

bool ldVec2::operator ==(const ldVec2 &other) const
{
    return cmpf(x, other.x) && cmpf(y, other.y);
}

bool ldVec2::operator !=(const ldVec2 &other) const
{
    return !(*this == other);
}

QDebug operator <<(QDebug debug, const ldVec2 &v)
{
    QDebugStateSaver saver(debug);

    debug.nospace() << "ldVec2(" << v.x << ", " << v.y << ")";

    return debug;
}
