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
//  ld3dBezierCurveFrame.h
//
//  Created by Sergey Gavrushkin on 21/02/17.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#ifndef LD3DBEZIERCURVEFRAME_H
#define LD3DBEZIERCURVEFRAME_H

#include "ldCore/Helpers/Maths/ldMaths.h"

#include "ld3dBezierCurveObject.h"

/**
 *  Object for easier access to svgBezierCurves parameters with some caching improvements
 */
class LDCORESHARED_EXPORT ld3dBezierCurveFrame
{
public:
    explicit ld3dBezierCurveFrame();
    explicit ld3dBezierCurveFrame(const ld3dBezierCurveObject &curve);
    explicit ld3dBezierCurveFrame(const std::vector<ld3dBezierCurveObject> &curves);
    ~ld3dBezierCurveFrame();

    void add(const ld3dBezierCurveObject &object);

    /** Get internal curve data */
    const std::vector<ld3dBezierCurveObject> &data() const;

    /** Get dimension */
    Svg3dDim dim() const;

    int countPoints() const;

private:
    void updateDim();

    std::vector<ld3dBezierCurveObject> m_curves;

    // cached data
    mutable Svg3dDim m_dim;
};

#endif // LD3DBEZIERCURVEFRAME_H

