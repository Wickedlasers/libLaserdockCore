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
//  ldBezierCurveDrawer.h
//
//  Created by Sergey Gavrushkin on 02/02/17.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#ifndef LDBEZIERCURVEDRAWER_H
#define LDBEZIERCURVEDRAWER_H

#include <memory>

#include "ldCore/Helpers/Maths/ldMaths.h"

class ldBezierCurveObject;
class ldBezierCurveFrame;
class ldRendererOpenlase;
struct OLPoint;

class LDCORESHARED_EXPORT ldBezierCurveDrawer
{
public:
    explicit ldBezierCurveDrawer();
    ~ldBezierCurveDrawer();

    void setMaxPoints(int maxPoints);
    void setBezierLengthCoeff(int bezierLengthCoeff);

    // get bezier data using start & controls points, ignoring end point. It's weird but it's necessary in some cases.
    std::vector<std::vector<OLPoint>> getBezierData(const ldBezierCurveObject &object, bool isSafe = true) const;

    // get point data from frame
    std::vector<std::vector<OLPoint>> getDrawingData(const ldBezierCurveFrame &frame) const;
    std::vector<std::vector<OLPoint>> getDrawingData(const ldBezierCurveObject &object, bool isSafe = true) const;

private:
    std::vector<std::vector<OLPoint> > makeSafeDrawing(const std::vector<std::vector<OLPoint> > &data) const;

    int m_maxPoints = 30;
    int m_bezierLengthCoeff = 100;
};

#endif // LDBEZIERCURVEDRAWER_H

