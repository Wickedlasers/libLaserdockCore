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
//  ldBezierCurveObject.h
//
//  Created by Sergey Gavrushkin on 19/02/17.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#ifndef LDBEZIERCURVEOBJECT_H
#define LDBEZIERCURVEOBJECT_H

#include "ldCore/Helpers/Maths/ldMaths.h"

#include "ld3dBezierCurveObject.h"

/**
 *  Object for easier access to svgBezierCurves parameters with some caching improvements
 */
class LDCORESHARED_EXPORT ldBezierCurveObject
{
public:
    static const ldBezierCurveObject stub;

    explicit ldBezierCurveObject(const svgBezierCurves &curves = svgBezierCurves(), bool isUnitedCoordinates = false);
    ~ldBezierCurveObject();

    bool isEmpty() const;

    void add(const svgBezierCurves &curves);
    void clear();

    /** Get internal curve data */
    const svgBezierCurves &data() const;

    /* Convert to united or laser coordinates */
    void setUnitedCoordinates(bool isUnitedCoordinates);
    bool isUnitedCoordinates() const;

    bool isValidForLaserOutput() const;

    /** Get dimension */
    const SvgDim &dim() const;

    /** Translate each curve */
    void translate(const Vec2 &vec2);
    void rotate(float rotateValue);
    void scale(float scaleValue);
    void colorize(uint32_t color);

    /** Convert 2D curve object to 3D representation. Pivot will be centered too */
    ld3dBezierCurveObject to3d() const;

    void moveToCenter();
    void moveTo(const Vec2 &pos);

private:
    void resetCache();

    svgBezierCurves m_curves;

    bool m_isUnitedCoordinates = false;

    // cached data
    // access it only with dim()
    mutable SvgDim m_cachedDim;
};

#endif // LDBEZIERCURVEOBJECT_H

