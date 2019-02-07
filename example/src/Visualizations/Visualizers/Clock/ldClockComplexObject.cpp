//
//  ldClockComplexObject.cpp
//  LaserdockVisualizer
//
//  Created by Eric Brugère on 12/05/16.
//  Copyright (c) 2016 Wicked Lasers. All rights reserved.
//

#include "ldClockComplexObject.h"

#include <QtCore/QDebug>

#include "ldCore/ldCore.h"
#include "ldCore/Helpers/Color/ldColorUtil.h"
#include "ldCore/Helpers/SVG/ldSvgReader.h"

// ldClockComplexObject
ldClockComplexObject::ldClockComplexObject()
    : _all_visible(true)
{
}

// ldClockComplexObject
ldClockComplexObject::~ldClockComplexObject()
{
}

// showAll
void ldClockComplexObject::showAll(bool b_all_visible)
{
    _all_visible = b_all_visible;
}

// innerDraw
void ldClockComplexObject::innerDraw(ldRendererOpenlase* p_renderer, const QTime &time)
{
    // init at each draw
    // safeDrawing = 0;
    // time
//    QTime time = QTime::currentTime();
//    QTime time = m_time;
    float second = time.second() + 0.001*time.msec();
    float minute = time.minute() + second/60.0;
    float hour = time.hour() + minute/60.0;
    //qDebug() << " second: "<< second;

    //second = minute = hour = 0;
    float sign = -1.0; // render is inversed...

    // 1-12
    for (int i=0; i<60; i++) {

        Vec2 p = Vec2();
        float l = 0.09f;
        bool doBigOne = true;
        if (i%5!=0) {
            doBigOne = false;
        } else if (i%5==0 && (i/5)%3!=0) {
            l/=1.5;
        }

        int pointsCount = 3;
        float step=0;
        float scale=1.00;
        float angle = (1.0*i/60) * M_PI*2.0;

        // color
        int c = ldColorUtil::colorForStep(1.0*i/(60));

        if (doBigOne) {
            p_renderer->begin(OL_LINESTRIP);
            for (int j = 0; j < pointsCount+1; j++) {
                step =l*(1.0*j/pointsCount);
                p.y = step*scale;
                p.y += (1.0-l);
                p.x = 0;
                p.rotate(angle);
                // add point to buffer
                //p = ldMaths::addVec2(p, Vec2(1, 1));
                p_renderer->vertex(p.x, p.y, c);
           }
           p_renderer->end();
       }

       if (!_all_visible) continue;

       // for seconds animation
       pointsCount = 2;
       step=0;
       l = 0.09f/3;
       // color
       //c = 0xFFFFFF;

       float inter = M_PIf*1.1f/6;
       float val1 = sign * second * M_2PIf / 60.0f ;
       val1 = ldMaths::periodIntervalKeeper(val1, 0, M_2PIf);

       float val2 = sign * minute * M_2PIf / 60.0 ;
       val2 = ldMaths::periodIntervalKeeper(val2, 0, M_2PIf);

       float val_angle = M_2PIf - sign * angle - inter/2;
       val_angle = ldMaths::periodIntervalKeeper(val_angle, 0, M_2PIf);

       val1 = val1-val_angle;
       val1 = ldMaths::periodIntervalKeeper(val1, 0, M_2PIf);

       //val2 = val2-val_angle;
       //val2 = ldMaths::periodIntervalKeeper(val2, 0, M_2PI);
       //qDebug() << " i" << i << " angle" << angle << "  sec" << val  << " fabs(val-val2)" << fabs(val-val2);
       //if (fabs(val1) > inter && fabs(val2) > inter) continue;
       if (fabs(val1) > inter) continue;

       p_renderer->begin(OL_LINESTRIP);
       for (int j = 0; j < pointsCount+1; j++) {
           step =l*(1.0*j/pointsCount);
           p.y = step*scale;
           p.y += (1.0-l-0.1);
           p.x = 0;
           p.rotate(angle);
           // add point to buffer
           //p = ldMaths::addVec2(p, Vec2(1, 1));
           p_renderer->vertex(p.x, p.y, c);
        }
        p_renderer->end();
    }

    if (!_all_visible) return;

    int color = ldColorUtil::colorForStep(sign * second/60.0);

    if(_svgHour.empty()) _svgHour = ldSvgReader::loadSvg(ldCore::instance()->resourceDir() + "/svg/clock/hour.svg", ldSvgReader::Type::Dev);
    if(_svgMin.empty()) _svgMin = ldSvgReader::loadSvg(ldCore::instance()->resourceDir() + "/svg/clock/min.svg", ldSvgReader::Type::Dev);

    drawDataBezierAsLinestrip(p_renderer, _svgHour, sign*hour * M_2PIf / 12.0, color);
    drawDataBezierAsLinestrip(p_renderer, _svgMin, sign*minute * M_2PIf / 60.0, color);
    //drawDataBezierAsLinestrip(svgSec, sign*second * M_2PIf / 60.0);

    // draw second, just a line
    p_renderer->begin(OL_LINESTRIP);
    int pointsCount = 10;
    Vec2 p = Vec2();
    for (int j = 0; j < pointsCount+1; j++) {
        // advance angle to next point
        float step = 0.9*(1.0*j/pointsCount);
        p.x = 0;
        p.y = step;
        p.rotate(sign*second * M_2PIf / 60.0);
        // color
        int c = ldColorUtil::lerpInt(0xFF0000,0xFF9933, step);
        // add point to buffer
        //p = ldMaths::addVec2(p, Vec2(1, 1));
        p_renderer->vertex(p.x, p.y, c);
   }
   p_renderer->end();
}

// drawDataBezierAsLinestrip
void ldClockComplexObject::drawDataBezierAsLinestrip(ldRendererOpenlase* p_renderer, svgBezierCurves &dataVect, float rotation, int color)
{
    for (std::vector<ldBezierCurve> &bezierTab : dataVect)
    {
        p_renderer->begin(OL_LINESTRIP);
        for (const ldBezierCurve &b : bezierTab)
        {
            int maxPoints = (int) (0.5*b.length());
            //qDebug()<<"maxPoints"<<maxPoints;
            if (maxPoints<3) maxPoints = 3;
            if (maxPoints>15) maxPoints = 15;
            //qDebug()<<"maxPoints"<<maxPoints;
            for (int j=0; j<maxPoints; j++)
            {
                float slope = 1.0*j/(maxPoints-1);
                Vec2 p = b.getPoint(slope);
                p.y = p.y + 100;
                p.x = 2 * p.x / 100.0  - 1;
                p.y = 2 * p.y / 100.0  - 1;
                p.rotate(rotation);
                p_renderer->vertex(p.x, p.y, color);
            }
        }
        p_renderer->end();
    }
}

