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

#include "ldCore/Helpers/Maths/ldRect.h"

#include <cmath>

bool ldRect::isNull() const
{
    return bottom_left.isNull() && top_right.isNull();
}

float ldRect::width() const
{
    return fabsf(bottom_left.x - top_right.x);
}

float ldRect::height() const
{
    return fabsf(bottom_left.y - top_right.y);
}

float ldRect::bottom() const
{
    return std::min(bottom_left.y, top_right.y);
}

float ldRect::top() const
{
    return std::max(bottom_left.y, top_right.y);
}

float ldRect::left() const
{
    return std::min(bottom_left.x, top_right.x);
}

float ldRect::right() const
{
    return std::max(bottom_left.x, top_right.x);
}

ldVec2 ldRect::center() const
{
    return ldVec2(left() + width()/2.f, bottom() + height()/2.f);
}

ldVec2 ldRect::size() const
{
    return ldVec2(width(), height());
}



