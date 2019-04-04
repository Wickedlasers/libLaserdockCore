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

#include "ldCore/Helpers/Maths/ldRect3.h"

#include <math.h>

bool ldRect3::isNull() const
{
    return bottom_left.isNull() && top_right.isNull();
}

float ldRect3::width() const
{
    return fabsf(bottom_left.x - top_right.x);
}

float ldRect3::height() const
{
    return fabsf(bottom_left.y - top_right.y);
}

float ldRect3::depth() const
{
    return fabsf(bottom_left.z - top_right.z);
}

float ldRect3::bottom() const
{
    return std::min(bottom_left.y, top_right.y);
}

float ldRect3::top() const
{
    return std::max(bottom_left.y, top_right.y);
}

float ldRect3::left() const
{
    return std::min(bottom_left.x, top_right.x);
}

float ldRect3::right() const
{
    return std::max(bottom_left.x, top_right.x);
}

float ldRect3::far() const
{
    return std::max(bottom_left.z, top_right.z);
}

float ldRect3::near() const
{
    return std::min(bottom_left.z, top_right.z);
}

ldVec3 ldRect3::center() const
{
    return ldVec3(left() + width()/2.f, bottom() + height()/2.f, near() + depth()/2.f);
}


