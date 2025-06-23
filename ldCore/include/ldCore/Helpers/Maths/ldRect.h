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

#ifndef LDRECT_H
#define LDRECT_H

#include <QtCore/QRectF>

#include "ldCore/ldCore_global.h"

#include "ldVec2.h"

/// Try to have the same API as QRectF. QRectF is based on qreal primitive but we need to use float
struct LDCORESHARED_EXPORT ldRect {
    ldVec2 bottom_left;
    ldVec2 top_right;

    bool isNull() const;

    float width() const;
    float height() const;

    float bottom() const;
    float top() const;
    float left() const;
    float right() const;

    ldVec2 center() const;

    ldVec2 size() const;

    QRectF toRect() const;
};

#endif // LDRECT_H


