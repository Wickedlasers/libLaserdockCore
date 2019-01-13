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
//  ld3dBezierCurveObject.h
//
//  Created by Sergey Gavrushkin on 20/02/17.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#ifndef LD3DBEZIERCURVEOBJECT_H
#define LD3DBEZIERCURVEOBJECT_H

#include "ldCore/Helpers/Maths/ldMaths.h"

/**
 *  Object for easier access to svgBezierCurves
 */
class LDCORESHARED_EXPORT ld3dBezierCurveObject
{
public:
    explicit ld3dBezierCurveObject(const svgBezier3dCurves &curves = svgBezier3dCurves(), bool isUnitedCoordinates = true);
    ~ld3dBezierCurveObject();

    /** Get internal curve data */
    const svgBezier3dCurves &data() const;

    bool isUnitedCoordinates() const;

    /** Get dimension */
    Svg3dDim dim() const;

    uint countPoints() const;

private:
    void updateDim();

    svgBezier3dCurves m_curves;

    bool m_isUnitedCoordinates = false;

    Svg3dDim m_dim;
};

#endif // LD3DBEZIERCURVEOBJECT_H

