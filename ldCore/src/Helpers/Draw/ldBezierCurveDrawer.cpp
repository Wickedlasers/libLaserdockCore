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
//  ldBezierCurveDrawer.cpp
//
//  Created by Sergey Gavrushkin on 02/02/17.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#include <ldCore/Helpers/Draw/ldBezierCurveDrawer.h>

#include <QtCore/QtDebug>
#include <QtCore/QTime>

#include <ldCore/Helpers/ldEnumHelper.h>
#include <ldCore/Helpers/BezierCurve/ldBezierCurveFrame.h>
#include <ldCore/Helpers/Color/ldColorEffects.h>
#include <ldCore/Render/ldRendererOpenlase.h>

ldBezierCurveDrawer::ldBezierCurveDrawer()
{
}

ldBezierCurveDrawer::~ldBezierCurveDrawer()
{
}

void ldBezierCurveDrawer::setMaxPoints(int maxPoints)
{
    m_maxPoints = maxPoints;
}

void ldBezierCurveDrawer::setBezierLengthCoeff(int bezierLengthCoeff)
{
    m_bezierLengthCoeff = bezierLengthCoeff;
}

std::vector<std::vector<OLPoint> > ldBezierCurveDrawer::getBezierData(const ldBezierCurveObject &object, bool isSafe) const
{
    std::vector<std::vector<OLPoint> > data;

    for (const ldBezierPath &bezierPath : object.data()) {
        std::vector<OLPoint> curvePoints;

        const std::vector<ldBezierCurve> &curves = bezierPath.data();
        uint32_t color = bezierPath.color();
        for (uint i = 0; i<curves.size(); i++)
        {
            const ldBezierCurve &b = curves[i];

            curvePoints.push_back(OLPoint{b.start().x, b.start().y, 0, color});
            curvePoints.push_back(OLPoint{b.control1().x, b.control1().y, 0, color});
            curvePoints.push_back(OLPoint{b.control2().x, b.control2().y, 0, color});
        }
        if(!curves.empty()) {
            const ldBezierCurve &b = curves[0];
            curvePoints.push_back(OLPoint{b.start().x, b.start().y, 0, color});
        }

        if(!curvePoints.empty())
            data.push_back(curvePoints);
    }

    if(isSafe)
        data = makeSafeDrawing(data);

    return data;
}

std::vector<std::vector<OLPoint> > ldBezierCurveDrawer::getDrawingData(const ldBezierCurveFrame &frame) const
{
    std::vector<std::vector<OLPoint> > data;

    for(const ldBezierCurveObject &object : frame.data()) {
        if(!object.isValidForLaserOutput())
            continue;

        std::vector<std::vector<OLPoint> > objData = getDrawingData(object, false);
        data.insert(std::end(data), std::begin(objData), std::end(objData));
    }

    return makeSafeDrawing(data);
}

std::vector<std::vector<OLPoint> > ldBezierCurveDrawer::getDrawingData(const ldBezierCurveObject &object, bool isSafe) const
{
    std::vector<std::vector<OLPoint> > data;

    for (const ldBezierPath &bezierPath : object.data()) {
        std::vector<OLPoint> curvePoints;
        uint32_t color = bezierPath.color();

        for (uint i = 0; i < bezierPath.data().size(); i++) {
            const ldBezierCurve &b = bezierPath.data()[i];
//                qDebug() << "b" << b.start.x << b.start.y << b.end.x << b.end.y;
            int bezierLengthPoints = (int) (m_bezierLengthCoeff*b.length());
            if (bezierLengthPoints<3) bezierLengthPoints = 3;
            if (bezierLengthPoints>m_maxPoints) bezierLengthPoints = m_maxPoints;
            // if (_firstFrame)  qDebug()<<(int) (100*b.length())<<"maxPoints"<<maxPoints;
            for (int j=0; j<bezierLengthPoints; j++)
            {
                //qDebug()<<"safeDrawing"<<safeDrawing;
                float slope = 1.0f*j/(bezierLengthPoints-1);
                ldVec2 p = b.getPoint(slope);
                // if (_firstFrame) qDebug()<<"p "<<p.x << "x" <<p.y;
                if(object.isUnitedCoordinates()) {
                    p = ldMaths::unitedToLaserCoords(p);
                }
                if (ldMaths::isValidLaserPoint(p)) {
                    if(bezierPath.gradient().isValid()) {
                        color = bezierPath.gradient().getColor(p.x, p.y);
                    }
//                        qDebug() << "p" << p.x << p.y;
                    curvePoints.push_back(OLPoint{p.x, p.y, 0, color});
                }
            }

        }

        if(!curvePoints.empty())
            data.push_back(curvePoints);
    }

    if(isSafe) {
        data = makeSafeDrawing(data);
    }
    return data;
}


std::vector<std::vector<OLPoint> > ldBezierCurveDrawer::makeSafeDrawing(const std::vector<std::vector<OLPoint> > &data) const
{
    std::vector<std::vector<OLPoint>> res;
    const int MAX_SAFE_POINTS = 4000;
    int safePoints = 0;
    for(const std::vector<OLPoint> &points : data) {
        std::vector<OLPoint> resPoints;
        for(const OLPoint &point : points) {
            resPoints.push_back(point);
            safePoints++;
            if(safePoints > MAX_SAFE_POINTS) {
                break;
            }
        }
        res.push_back(resPoints);

        if(safePoints > MAX_SAFE_POINTS) {
            break;
        }
    }

    return res;
}

