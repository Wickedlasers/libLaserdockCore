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
//  ldBezierCurveFrame.h
//
//  Created by Sergey Gavrushkin on 21/02/17.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#ifndef LDBEZIERCURVEFRAME_H
#define LDBEZIERCURVEFRAME_H

#include "ldCore/Helpers/Maths/ldMaths.h"

#include "ldBezierCurveObject.h"
#include "ld3dBezierCurveFrame.h"

/**
 *  Visualizer frame representation using ldBezierCurveObjects as internal structure
 */
class LDCORESHARED_EXPORT ldBezierCurveFrame
{
public:
    explicit ldBezierCurveFrame(const std::vector<ldBezierCurveObject> &curves = std::vector<ldBezierCurveObject>());
    ~ldBezierCurveFrame();


    /** Manage objects */
    void add(const ldBezierCurveObject &object);
    void clear();

    /** Get internal curve data */
    const std::vector<ldBezierCurveObject> &data() const;

    /** Get dimension */
    SvgDim dim() const;

    /** Modify */
    void translate(const Vec2 &vec2);
    void rotate(float rotateValue);
    void scale(float scaleValue);
    void colorize(uint32_t color);

    /** Convert 2D curve object to 3D representation. Pivot will be centered too */
    ld3dBezierCurveFrame to3d() const;

    ldBezierCurveObject asObject() const;

    /** Move it, move it */
    void moveToCenter();
    void moveTo(const Vec2 &pos);

private:
    void resetCache() const;
    void resetCachedDim() const;
    void resetCachedObject() const;
    void updateCachedDim() const;
    void updateCachedObject() const;

    std::vector<ldBezierCurveObject> m_curves;

    // cached data
    mutable ldBezierCurveObject m_cachedObject;
    mutable SvgDim m_cachedDim;
};

#endif // LDBEZIERCURVEFRAME_H

