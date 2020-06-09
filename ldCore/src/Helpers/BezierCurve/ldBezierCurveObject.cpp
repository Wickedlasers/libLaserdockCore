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

ldBezierCurveObject::ldBezierCurveObject(const ldBezierPaths &curves)
    : m_paths(curves)
{
}

bool ldBezierCurveObject::isEmpty() const
{
    return m_paths.empty();
}


void ldBezierCurveObject::clear()
{
    m_paths.clear();

    resetCache();
}

const ldBezierPaths &ldBezierCurveObject::data() const
{
    return m_paths;
}

const ldRect &ldBezierCurveObject::dim() const
{
    if(m_cachedDim.isNull()) {
        m_cachedDim = ldSvgReader::svgDim(m_paths);
    }

    return m_cachedDim;
}

int ldBezierCurveObject::totalPoints() const
{
    return ldSvgReader::totalPoints(m_paths);
}

void ldBezierCurveObject::translate(const ldVec2 &vec2)
{
    ldMaths::translateSvgBezierCurves(m_paths, vec2);

    resetCache();
}

void ldBezierCurveObject::rotate(float rotateValue)
{
    ldMaths::rotateSvgBezierCurves(m_paths, rotateValue);

    resetCache();
}

void ldBezierCurveObject::scale(float scaleValue)
{
    ldMaths::scaleSvgBezierCurves(m_paths, scaleValue);

    resetCache();
}


void ldBezierCurveObject::colorize(uint32_t color)
{
    for (ldBezierPath &path : m_paths) {
        path.setColor(color);
    }
}

ld3dBezierCurveObject ldBezierCurveObject::to3d() const
{
    ld3dBezierCurves threeDCurves = ldMaths::svgBezierTo3dSvgBezierCurves(m_paths);

    // pivot in center
    // SG: not sure if we need it always, probably we can do it optional via parameter or move to another function
    for (std::vector<ld3dBezierCurve> &threeDCurve : threeDCurves){
        for (ld3dBezierCurve &b3z : threeDCurve){
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

    ldVec2 centerPos;
    centerPos.x = (centerCoord - dim().width())/2.f;
    centerPos.y = (centerCoord - dim().height())/2.f;

    moveTo(centerPos);
}


void ldBezierCurveObject::moveTo(const ldVec2 &pos)
{
    ldVec2 moveDiff;
    moveDiff.x = pos.x - dim().left();
    moveDiff.y = pos.y - dim().bottom();

    translate(moveDiff);

    resetCache();
}

const ldRect &ldBezierCurveObject::explicitRect() const
{
    return m_explicitRect;
}

void ldBezierCurveObject::setExplicitRect(const ldRect &rect)
{
    m_explicitRect = rect;
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
    m_cachedDim = ldRect();
}

void ldBezierCurveObject::add(const ldBezierPath &path)
{
    m_paths.push_back(path);

    resetCache();
}


void ldBezierCurveObject::add(const ldBezierPaths &paths)
{
    for(const ldBezierPath &path : paths) {
        m_paths.push_back(path);
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
        ldMaths::svgBezierLaserToUnitedCoords(m_paths);
    } else {
        ldMaths::svgBezierUnitedToLaserCoords(m_paths);
    }
    resetCache();
}
