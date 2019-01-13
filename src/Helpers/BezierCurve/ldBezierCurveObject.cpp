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
//  ldBezierCurveObject.cpp
//
//  Created by Sergey Gavrushkin on 19/02/17.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#include "ldCore/Helpers/BezierCurve/ldBezierCurveObject.h"

#include "ldCore/Helpers/SVG/ldSvgReader.h"

const ldBezierCurveObject ldBezierCurveObject::stub = ldBezierCurveObject();

ldBezierCurveObject::ldBezierCurveObject(const svgBezierCurves &curves, bool isUnitedCoordinates)
    : m_curves(curves)
    , m_isUnitedCoordinates(isUnitedCoordinates)
{
}

ldBezierCurveObject::~ldBezierCurveObject()
{
}

bool ldBezierCurveObject::isEmpty() const
{
    return m_curves.empty();
}


void ldBezierCurveObject::clear()
{
    m_curves.clear();

    resetCache();
}

const svgBezierCurves &ldBezierCurveObject::data() const
{
    return m_curves;
}

const SvgDim &ldBezierCurveObject::dim() const
{
    if(m_cachedDim.isNull()) {
        m_cachedDim = ldSvgReader::svgDim(m_curves);
    }

    return m_cachedDim;
}

void ldBezierCurveObject::translate(const Vec2 &vec2)
{
    ldMaths::translateSvgBezierCurves(m_curves, vec2);

    resetCache();
}

void ldBezierCurveObject::rotate(float rotateValue)
{
    ldMaths::rotateSvgBezierCurves(m_curves, rotateValue);

    resetCache();
}

void ldBezierCurveObject::scale(float scaleValue)
{
    ldMaths::scaleSvgBezierCurves(m_curves, scaleValue);

    resetCache();
}


void ldBezierCurveObject::colorize(uint32_t color)
{
    for (std::vector<ldBezierCurve> &curves : m_curves) {
        for (ldBezierCurve &curve : curves) {
            curve.setColor(color);
        }
    }
}

ld3dBezierCurveObject ldBezierCurveObject::to3d() const
{
    svgBezier3dCurves threeDCurves = ldMaths::svgBezierTo3dSvgBezierCurves(m_curves);

    // pivot in center
    // SG: not sure if we need it always, probably we can do it optional via parameter or move to another function
    for (std::vector<Bezier3dCurve> &threeDCurve : threeDCurves){
        for (Bezier3dCurve &b3z : threeDCurve){
            b3z.pivot.x = dim().center().x;
            b3z.pivot.y = dim().center().y;
            b3z.pivot.z = 0;
        }
    }

    return ld3dBezierCurveObject(threeDCurves, m_isUnitedCoordinates);
}

void ldBezierCurveObject::moveToCenter()
{
    const float centerCoord = isUnitedCoordinates() ? 1.0f : 0.f;

    Vec2 centerPos;
    centerPos.x = (centerCoord - dim().width())/2.f;
    centerPos.y = (centerCoord - dim().height())/2.f;

    moveTo(centerPos);
}


void ldBezierCurveObject::moveTo(const Vec2 &pos)
{
    Vec2 moveDiff;
    moveDiff.x = pos.x - dim().left();
    moveDiff.y = pos.y - dim().bottom();

    translate(moveDiff);

    resetCache();
}

bool ldBezierCurveObject::isUnitedCoordinates() const
{
    return m_isUnitedCoordinates;
}

bool ldBezierCurveObject::isValidForLaserOutput() const
{
    float limit = 1.05f;

    float x = dim().left();
    float y = dim().bottom();
    float a = dim().right();
    float b = dim().top();

    if(m_isUnitedCoordinates) {
        float x1 = -0.05f;
        float y1 = -0.05f;
        float a1 = limit;
        float b1 = limit;

        if(a < x1 || a1 < x || b < y1 || b1 < y)
            return false;
        else
            return true;
    } else {
        float x1 = -limit;
        float y1 = -limit;
        float a1 = limit;
        float b1 = limit;

        if(a < x1 || a1 < x || b < y1 || b1 < y)
            return false;
        else
            return true;
    }

    return false;
}

void ldBezierCurveObject::resetCache()
{
    m_cachedDim = SvgDim();
}

void ldBezierCurveObject::add(const svgBezierCurves &curves)
{
    for(const auto &curve : curves) {
        m_curves.push_back(curve);
    }

    resetCache();
}

void ldBezierCurveObject::setUnitedCoordinates(bool isUnitedCoordinates)
{
    if(isUnitedCoordinates == m_isUnitedCoordinates) {
        return;
    }

    m_isUnitedCoordinates = isUnitedCoordinates;

    if(isUnitedCoordinates) {
        ldMaths::svgBezierLaserToUnitedCoords(m_curves);
    } else {
        ldMaths::svgBezierUnitedToLaserCoords(m_curves);
    }
    resetCache();
}
