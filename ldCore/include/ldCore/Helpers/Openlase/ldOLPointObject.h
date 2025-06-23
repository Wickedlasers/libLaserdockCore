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
//  ldOLPointObject.h
//
//  Created by Sergey Gavrushkin on 25/01/22.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#ifndef LDOLPOINTOBJECT_H
#define LDOLPOINTOBJECT_H

#include <ldCore/ldCore_global.h>
#include <ldCore/Helpers/Maths/ldRect.h>

#include <openlase/ldLibol.h>

typedef std::vector<OLPoint> PointVector;

/**
 *  Object for easier access to OLPoints parameters with some caching improvements
 */
class LDCORESHARED_EXPORT ldOLPointObject
{
public:
    explicit ldOLPointObject(const std::vector<PointVector> &pointVectors
                             = std::vector<PointVector>());

    bool isEmpty() const;

    void clear();

    /** Get internal curve data */
    const std::vector<PointVector> &data() const;
    std::vector<PointVector> &data();

    const ldRect &dim() const;

    /** Translate each curve */
    void translate(const ldVec2 &vec2);
    void transform(const ldVec2 &v, int alignment);
    void rotate(float angle);
    void rotate(float angle, const ldVec2 &center);
    void scale(float scaleValue);

    void add(const PointVector &pv);

    void colorize(uint32_t color);

    void moveTo(const ldVec2 &pos);
    void moveToCenter();

private:
    void resetCache();

    std::vector<PointVector> m_pointVectors;

    // cached data
    // access it only with dim()
    mutable ldRect m_cachedDim;
};

#endif // LDOLPOINTOBJECT_H

