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
//  ldBezierCurveFrame.cpp
//
//  Created by Sergey Gavrushkin on 19/02/17.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#include "ldCore/Helpers/BezierCurve/ldBezierCurveFrame.h"

#include "ldCore/Helpers/SVG/ldSvgReader.h"

ldBezierCurveFrame::ldBezierCurveFrame(const std::vector<ldBezierCurveObject> &curves)
    : m_curves(curves)
{
}

ldBezierCurveFrame::~ldBezierCurveFrame()
{
}


void ldBezierCurveFrame::add(const ldBezierCurveObject &object)
{
    m_curves.push_back(object);

    resetCache();
}

const std::vector<ldBezierCurveObject> &ldBezierCurveFrame::data() const
{
    return m_curves;
}

SvgDim ldBezierCurveFrame::dim() const
{
    if(m_cachedDim.isNull()) {
        updateCachedDim();
    }

    return m_cachedDim;
}

void ldBezierCurveFrame::translate(const Vec2 &vec2)
{
    for(ldBezierCurveObject &object : m_curves) {
        object.translate(vec2);
    }

    resetCache();
}

void ldBezierCurveFrame::rotate(float rotateValue)
{
    for(ldBezierCurveObject &object : m_curves) {
        object.rotate(rotateValue);
    }

    resetCache();
}

void ldBezierCurveFrame::scale(float scaleValue)
{
    for(ldBezierCurveObject &object : m_curves) {
        object.scale(scaleValue);
    }

    resetCache();
}

void ldBezierCurveFrame::colorize(uint32_t color)
{
    for(ldBezierCurveObject &object : m_curves) {
        object.colorize(color);
    }

    resetCachedObject();
}

ld3dBezierCurveFrame ldBezierCurveFrame::to3d() const
{
    std::vector<ld3dBezierCurveObject> m_3dObjects;
    for(const ldBezierCurveObject &object : m_curves) {
        m_3dObjects.push_back(object.to3d());
    }

    return ld3dBezierCurveFrame(m_3dObjects);
}

ldBezierCurveObject ldBezierCurveFrame::asObject() const
{
    if(m_cachedObject.data().empty()) {
        updateCachedObject();
    }

    return m_cachedObject;
}

void ldBezierCurveFrame::resetCache() const
{
    resetCachedDim();
    resetCachedObject();
}

void ldBezierCurveFrame::resetCachedDim() const
{
    m_cachedDim = SvgDim();
}

void ldBezierCurveFrame::resetCachedObject() const
{
    m_cachedObject = ldBezierCurveObject();
}

void ldBezierCurveFrame::updateCachedDim() const
{
    SvgDim res;
    bool init = false;
    for (const ldBezierCurveObject &curveObject : m_curves) {
        if (!init) {
            res = curveObject.dim();
            init = true;
            continue;
        }

        // bottom left
        res.bottom_left.x = std::min(res.bottom_left.x, curveObject.dim().left());
        res.bottom_left.y = std::min(res.bottom_left.y, curveObject.dim().bottom());

        // top right
        res.top_right.x = std::max(res.top_right.x, curveObject.dim().right());
        res.top_right.y = std::max(res.top_right.y, curveObject.dim().top());
    }

    m_cachedDim = res;
}

void ldBezierCurveFrame::updateCachedObject() const
{
    m_cachedObject.clear();
    for(const ldBezierCurveObject &object : m_curves) {
        if(m_cachedObject.data().empty())
            m_cachedObject.setUnitedCoordinates(object.isUnitedCoordinates());

        m_cachedObject.add(object.data());
    }
}

void ldBezierCurveFrame::moveToCenter()
{
    for (ldBezierCurveObject &bezierCurves : m_curves) {
        const float centerCoord = bezierCurves.isUnitedCoordinates() ? 1.0f : 0.f;
        Vec2 centerPos;
        centerPos.x = (centerCoord - dim().width())/2.f;
        centerPos.y = (centerCoord - dim().height())/2.f;

        bezierCurves.translate(centerPos);
    }

    resetCache();
}

void ldBezierCurveFrame::moveTo(const Vec2 &pos)
{
    Vec2 moveDiff;
    moveDiff.x = pos.x - dim().left();
    moveDiff.y = pos.y - dim().bottom();

    translate(moveDiff);

    resetCache();
}

void ldBezierCurveFrame::clear()
{
    m_curves.clear();

    resetCache();
}
