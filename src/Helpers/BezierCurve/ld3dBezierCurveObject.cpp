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
//  ld3dBezierCurveObject.cpp
//
//  Created by Sergey Gavrushkin on 20/02/17.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#include "ldCore/Helpers/BezierCurve/ld3dBezierCurveObject.h"


#include "ldCore/Helpers/SVG/ldSvgReader.h"

ld3dBezierCurveObject::ld3dBezierCurveObject(const svgBezier3dCurves &curves, bool isUnitedCoordinates)
    : m_curves(curves)
    , m_isUnitedCoordinates(isUnitedCoordinates)
{
    updateDim();
}

ld3dBezierCurveObject::~ld3dBezierCurveObject()
{
}

const svgBezier3dCurves &ld3dBezierCurveObject::data() const
{
    return m_curves;
}

bool ld3dBezierCurveObject::isUnitedCoordinates() const
{
    return m_isUnitedCoordinates;
}

uint ld3dBezierCurveObject::countPoints() const
{
    uint result = 0;
    for(const std::vector<Bezier3dCurve> &vec : m_curves) {
        result += vec.size();
    }
    return result;
}

Svg3dDim ld3dBezierCurveObject::dim() const
{
    return m_dim;
}

void ld3dBezierCurveObject::updateDim()
{
    m_dim = ldSvgReader::svgDim(m_curves);
}
