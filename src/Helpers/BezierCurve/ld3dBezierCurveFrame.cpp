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

//  ld3dBezierCurveFrame.cpp
//
//  Created by Sergey Gavrushkin on 19/02/17.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#include "ldCore/Helpers/BezierCurve/ld3dBezierCurveFrame.h"

#include "ldCore/Helpers/SVG/ldSvgReader.h"



ld3dBezierCurveFrame::ld3dBezierCurveFrame()
{
}

ld3dBezierCurveFrame::ld3dBezierCurveFrame(const ld3dBezierCurveObject &curve)
{
    m_curves.push_back(curve);
    updateDim();
}

ld3dBezierCurveFrame::ld3dBezierCurveFrame(const std::vector<ld3dBezierCurveObject> &curves)
    : m_curves(curves)
{
    updateDim();
}

ld3dBezierCurveFrame::~ld3dBezierCurveFrame()
{
}


void ld3dBezierCurveFrame::add(const ld3dBezierCurveObject &object)
{
    m_curves.push_back(object);

    updateDim();
}

const std::vector<ld3dBezierCurveObject> &ld3dBezierCurveFrame::data() const
{
    return m_curves;
}

Svg3dDim ld3dBezierCurveFrame::dim() const
{
    return m_dim;
}

void ld3dBezierCurveFrame::updateDim()
{
    Svg3dDim res;
    bool init = false;
    for (const ld3dBezierCurveObject &curveObject : m_curves) {
        if (!init) {
            res = curveObject.dim();
            init = true;
            continue;
        }

        // bottom left
        res.bottom_left.x = std::min(res.bottom_left.x, curveObject.dim().left());
        res.bottom_left.y = std::min(res.bottom_left.y, curveObject.dim().bottom());
        res.bottom_left.z = std::min(res.bottom_left.z, curveObject.dim().near());

        // top right
        res.top_right.x = std::max(res.top_right.x, curveObject.dim().right());
        res.top_right.y = std::max(res.top_right.y, curveObject.dim().top());
        res.top_right.z = std::max(res.top_right.z, curveObject.dim().far());
    }

    m_dim = res;
}

int ld3dBezierCurveFrame::countPoints() const
{
    int count = 0;

    for(const ld3dBezierCurveObject &object : m_curves) {
        count += object.countPoints();
    }
    return count;
}
