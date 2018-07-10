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

#include "ldBezierCurveDrawer.h"

#include <QtCore/QTime>

#include <ldCore/Render/ldRendererOpenlase.h>

#include "ldCore/Visualizations/util/BezierCurveHelper/ldBezierCurveFrame.h"
#include "ldCore/Visualizations/util/ColorHelper/ldColorUtil.h"

ldBezierCurveDrawer::ldBezierCurveDrawer()
{
}

ldBezierCurveDrawer::~ldBezierCurveDrawer()
{
}

void ldBezierCurveDrawer::setColorEffect(const ldBezierCurveDrawer::ColorEffect &mode)
{
    switch(mode) {
    case ColorEffect::No:
        m_colorEffect.reset();
        break;
    case ColorEffect::One:
        m_colorEffect.reset(new ldColorEffectOne);
        break;
    case ColorEffect::Two:
        m_colorEffect.reset(new ldColorEffectTwo);
        break;
    case ColorEffect::Three:
        m_colorEffect.reset(new ldColorEffectThree);
        break;
    }

    if(m_colorEffect) {
        m_colorEffect->setBaseColorDecay(m_baseColorDecay);
    }
}


void ldBezierCurveDrawer::setColorEffectBaseColorDecay(int baseColorDecay)
{
    if(m_baseColorDecay == baseColorDecay) {
        return;
    }

    m_baseColorDecay = baseColorDecay;

    if(m_colorEffect) m_colorEffect->setBaseColorDecay(m_baseColorDecay);
}

void ldBezierCurveDrawer::setMaxPoints(int maxPoints)
{
    m_maxPoints = maxPoints;
}

void ldBezierCurveDrawer::setBezierLengthCoeff(int bezierLengthCoeff)
{
    m_bezierLengthCoeff = bezierLengthCoeff;
}

void ldBezierCurveDrawer::innerDraw(ldRendererOpenlase *renderer, const ldBezierCurveFrame &dataVect)
{
    if(m_colorEffect) m_colorEffect->updateColor();

    m_safeDrawing = 0;

    // calculate dim in united coords for later usage
    SvgDim dimInUnited = dataVect.dim();
    if(!dataVect.data().empty() && !dataVect.data()[0].isUnitedCoordinates()) {
        dimInUnited = ldMaths::laserToUnitedCoords(dataVect.dim());
    }

    for(const ldBezierCurveObject &object : dataVect.data()) {
        draw(renderer, object, dimInUnited);
    }
}

void ldBezierCurveDrawer::innerDraw(ldRendererOpenlase *renderer, const ldBezierCurveObject &dataVect)
{
    if(m_colorEffect) m_colorEffect->updateColor();

    m_safeDrawing = 0;

    // calculate dim in united coords for later usage
    SvgDim dimInUnited = dataVect.dim();
    if(!dataVect.isUnitedCoordinates()) {
        dimInUnited = ldMaths::laserToUnitedCoords(dataVect.dim());
    }

    draw(renderer, dataVect, dimInUnited);
}

std::vector<std::vector<OLPoint> > ldBezierCurveDrawer::getDrawingData(const ldBezierCurveFrame &frame) const
{
    std::vector<std::vector<OLPoint> > data;

    m_safeDrawing = 0;

    int drawingMaxPoints = 1000;

    for(const ldBezierCurveObject &object : frame.data()) {
        for (const std::vector<BezierCurve> &bezierTab : object.data()) {
            std::vector<OLPoint> curvePoints;
            for (const BezierCurve &b : bezierTab) {
                int bezierLengthPoints = (int) (m_bezierLengthCoeff*ldMaths::bezierLength(b));
                if (bezierLengthPoints<3) bezierLengthPoints = 3;
                if (bezierLengthPoints>m_maxPoints) bezierLengthPoints = m_maxPoints;
                // if (_firstFrame)  qDebug()<<(int) (100*ldMaths::bezierLength(b))<<"maxPoints"<<maxPoints;
                for (int j=0; j<bezierLengthPoints; j++)
                {
                    //qDebug()<<"safeDrawing"<<safeDrawing;
                    float slope = 1.0f*j/(bezierLengthPoints-1);
                    Vec2 p;
                    p.x = ldMaths::cubicBezier(slope, b.start.x, b.control1.x, b.control2.x, b.end.x);
                    p.y = ldMaths::cubicBezier(slope, b.start.y, b.control1.y, b.control2.y, b.end.y);
                    // if (_firstFrame) qDebug()<<"p "<<p.x << "x" <<p.y;
                    if(object.isUnitedCoordinates()) {
                        p = ldMaths::unitedToLaserCoords(p);
                    }
                    if (m_safeDrawing <= drawingMaxPoints && ldMaths::isValidLaserPoint(p)) {
                        curvePoints.push_back(OLPoint{p.x, p.y, 0, b.color});
                    }
                }

            }
            data.push_back(curvePoints);
        }
    }

    return data;
}

void ldBezierCurveDrawer::draw(ldRendererOpenlase *renderer, const ldBezierCurveObject &dataVect, const SvgDim &dimInUnited)
{
    ldBezierCurveFrame frame(std::vector<ldBezierCurveObject>{dataVect});
    std::vector<std::vector<OLPoint> > drawingData = getDrawingData(frame);
    for(const std::vector<OLPoint> &pointVector : drawingData) {
        renderer->begin(OL_LINESTRIP);
        for(const OLPoint &p: pointVector) {
            uint32_t color = m_colorEffect ? m_colorEffect->getColor(Vec2(p.x, p.y), dimInUnited) :  p.color;
            renderer->vertex(p.x, p.y, color, 1);
            m_safeDrawing++;
        }
        renderer->end();
   }


// ------------- old drawing, check for regressions ------------------

    //
//    int drawingMaxPoints = 1000;
//    if (m_safeDrawing>drawingMaxPoints) return;
//    // qDebug()<<"safeDrawing::"<<safeDrawing;
//    for (const std::vector<BezierCurve> &bezierTab : dataVect.data())
//    {
//        renderer->begin(OL_LINESTRIP);
//        for (const BezierCurve &b : bezierTab)
//        {
//            int bezierLengthPoints = (int) (m_bezierLengthCoeff*ldMaths::bezierLength(b));
//            if (bezierLengthPoints<3) bezierLengthPoints = 3;
//            if (bezierLengthPoints>m_maxPoints) bezierLengthPoints = m_maxPoints;
//            // if (_firstFrame)  qDebug()<<(int) (100*ldMaths::bezierLength(b))<<"maxPoints"<<maxPoints;
//            for (int j=0; j<bezierLengthPoints; j++)
//            {
//                //qDebug()<<"safeDrawing"<<safeDrawing;
//                float slope = 1.0f*j/(bezierLengthPoints-1);
//                Vec2 p;
//                p.x = ldMaths::cubicBezier(slope, b.start.x, b.control1.x, b.control2.x, b.end.x);
//                p.y = ldMaths::cubicBezier(slope, b.start.y, b.control1.y, b.control2.y, b.end.y);
//                // if (_firstFrame) qDebug()<<"p "<<p.x << "x" <<p.y;
//                if(dataVect.isUnitedCoordinates()) {
//                    p = ldMaths::unitedToLaserCoords(p);
//                }
//                //
//                if (m_safeDrawing <= drawingMaxPoints && ldMaths::isValidLaserPoint(p)) {
//                    uint32_t color = m_colorEffect ? m_colorEffect->getColor(p, dimInUnited) :  b.color;
//                    renderer->vertex(p.x, p.y, color);
//                    m_safeDrawing++;
//                }

//            }
//        }
//        renderer->end();
//    }
}
