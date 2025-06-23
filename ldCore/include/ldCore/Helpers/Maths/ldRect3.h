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

#ifndef LDRECT3_H
#define LDRECT3_H

#include "ldCore/ldCore_global.h"

#include "ldVec3.h"
#include "ldRect.h"

#ifdef _MSC_VER
// we want to use words far and near in our code
#undef far
#undef near
#endif

/// Try to have the same API as QRectF. QRectF is based on qreal primitive but we need to use float
struct LDCORESHARED_EXPORT ldRect3 {
    ldVec3 bottom_left;
    ldVec3 top_right;

    bool isNull() const;

    float width() const;
    float height() const;
    float depth() const;

    float bottom() const;
    float top() const;
    float left() const;
    float right() const;
    float far() const;
    float near() const;

    ldVec3 center() const;

    ldRect to2d() const;
};

#endif // LDRECT3_H


