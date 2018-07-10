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

// ldSvgReader.cpp
// Created by Eric Brugère on 10/12/16. With Fixed Scale version adaption by MEO 02/2017
// Copyright (c) 2016 Wicked Lasers. All rights reserved.

#include "ldSvgReader.h"

#include <string>

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QString>

#define NANOSVG_IMPLEMENTATION
#include <nanosvg-master/src/nanosvg.h>

#include "ldCore/Visualizations/util/ColorHelper/ldColorUtil.h"

// debugStatCurves
void ldSvgReader::debugStatCurves(svgBezierCurves bezierCurves)
{
    int pointCounter = 0;
    for (uint i=0; i<bezierCurves.size(); i++) {
        std::vector<BezierCurve> curve = bezierCurves[i];
        pointCounter += curve.size();
    }
    qDebug() << "nb curve: " << bezierCurves.size();
    qDebug() << "nb bezier points: " << pointCounter;
}

// totalPoints
int ldSvgReader::totalPoints(const svgBezierCurves &bezierCurves)
{
    int pointCounter = 0;
    for (const std::vector<BezierCurve> &curve : bezierCurves) {
        pointCounter += curve.size();
    }
    return pointCounter;
}

// totalPoints
int ldSvgReader::totalPoints(const svgBezier3dCurves &bezierCurves)
{
    int pointCounter = 0;
    for (const std::vector<Bezier3dCurve> &curve : bezierCurves) {
        pointCounter += curve.size();
    }
    return pointCounter;
}

// snapCurves
svgBezierCurves ldSvgReader::snapCurves(const svgBezierCurves &bezierCurves, float snapDistance)
{
    svgBezierCurves res;
    //debugStatCurves(bezierCurves);
    std::vector<BezierCurve> newOne;
    for (uint i=0; i<bezierCurves.size(); i++) {
        const std::vector<BezierCurve> &curve = bezierCurves[i];
        //
        for (uint c=0; c<curve.size(); c++) {
            const BezierCurve &a = curve[c];
            //qDebug() << "push_back a for c:" << c ;
            if (c == 0 && i>0) { // && i<bezierCurves.size()-1
                std::vector<BezierCurve> prevcurve = bezierCurves[i-1];
                BezierCurve b = prevcurve[prevcurve.size()-1];
                if (fabs(b.end.x - a.start.x)<snapDistance && fabs(b.end.y - a.start.y)<snapDistance) {
                    // snap -> don't create newone
                    //qDebug() << "b.end.x:" << b.end.x << " a.start.x" <<  a.start.x;
                } else {
                    // push previous new one
                    res.push_back(newOne);
                    newOne.clear();
                }
            }
            newOne.push_back(a);
        }
        // last one
        if (i==bezierCurves.size()-1) {
            res.push_back(newOne);
        }
    }
    //qDebug() << "snapCurves" ;
    //debugStatCurves(res);
    return res;
}

// loadSvgSequence
svgBezierCurvesSequence ldSvgReader::loadSvgSequence(int size, int masksize, QString qtbasefilename, SvgReadingType p_type, float snapDistance)
{
    svgBezierCurvesSequence res = svgBezierCurvesSequence();
    //
    for (int i=1;i<size+1;i++) {
        // qDebug() << "i:" << i;
        QString stringWork(qtbasefilename);
        // check power of ten between 1 and masksize
        bool done = false;
        for (int t=masksize-1; t>-1;t--) {
            float test = powf(10, t);
            if (done) continue;
            if (i<test && test>1) {
                stringWork.append("0");
               //  qDebug() << "    i<test add 0:" << stringWork->toStdString().c_str();
            } else {
                stringWork.append(QString::number(i));
               //  qDebug() << "    add i:" << stringWork->toStdString().c_str();
                done= true;
            }
        }
        stringWork.append(".svg");
        // qDebug() << " > i:" << i << "stringWork " << stringWork->toStdString().c_str();
        svgBezierCurves tmp = ldSvgReader::loadSvg(stringWork, p_type, snapDistance);
        res.push_back(tmp);
    }
    return res;
}

// loadSvgSequenceFixedScale
svgBezierCurvesSequence ldSvgReader::loadSvgSequenceFixedScale(float fixedMinX, float fixedMaxX, float fixedMinY, float fixedMaxY, int size, int masksize, char const* qtbasefilename, SvgReadingType p_type, float snapDistance)
{
    svgBezierCurvesSequence res = svgBezierCurvesSequence();
    //
    for (int i=1;i<size+1;i++) {
        // qDebug() << "i:" << i;
        QString stringWork(qtbasefilename);
        // check power of ten between 1 and masksize
        bool done = false;
        for (int t=masksize-1; t>-1;t--) {
            float test = powf(10, t);
            if (done) continue;
            if (i<test && test>1) {
                stringWork.append("0");
               //  qDebug() << "    i<test add 0:" << stringWork->toStdString().c_str();
            } else {
                stringWork.append(QString::number(i));
               //  qDebug() << "    add i:" << stringWork->toStdString().c_str();
                done= true;
            }
        }
        stringWork.append(".svg");
        // qDebug() << " > i:" << i << "stringWork " << stringWork->toStdString().c_str();
        svgBezierCurves tmp = ldSvgReader::loadSvgFixedScale(fixedMinX, fixedMaxX, fixedMinY, fixedMaxY, stringWork.toStdString().c_str(), p_type, snapDistance);
        res.push_back(tmp);
    }
    return res;
}

// loadSvg
svgBezierCurves ldSvgReader::loadSvg(QString qtfilename, SvgReadingType p_type, float snapDistance)
{
    SvgReadingType type = p_type;

    svgBezierCurves res;

    //qDebug() << "ldSvgReader::loadSvg " << qtfilename;

    QFile file(qtfilename);
    if (!file.exists()) {
        qDebug() << "error::ldSvgReader::loadSvg: file does not exist" << qtfilename;
        return res;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "error::ldSvgReader::loadSvg: Qt file issue";
        return res;
    }

    QByteArray blob = file.readAll();

    NSVGimage* nsvgImage = nsvgParseFromData(blob.data(), blob.length(), "px", 96.0f);
    if (nsvgImage == NULL) {
        qDebug() << "nsvgImage == NULL";
        nsvgDelete(nsvgImage);
        return res;
    }

    //
    float maxValue = 100000000;
    float minX = maxValue;
    float minY = maxValue;
    float maxX = -maxValue;
    float maxY = -maxValue;
    float im_w = nsvgImage->width;
    float im_h = nsvgImage->height;

    // stats
    for (NSVGshape* shape = nsvgImage->shapes; shape != NULL; shape = shape->next)
    {
        //
        for (NSVGpath* path = shape->paths; path != NULL; path = path->next)
        {
            for (int i = 0; i < path->npts-1; i += 3)
            {
                // start
                if (path->pts[(i)*2]<minX) minX = path->pts[(i)*2];
                if (path->pts[(i)*2]>maxX) maxX = path->pts[(i)*2];
                if (path->pts[(i)*2+1]<minY) minY = path->pts[(i)*2+1];
                if (path->pts[(i)*2+1]>maxY) maxY = path->pts[(i)*2+1];
                // end
                if (path->pts[(i+3)*2]<minX) minX = path->pts[(i+3)*2];
                if (path->pts[(i+3)*2]>maxX) maxX = path->pts[(i+3)*2];
                if (path->pts[(i+3)*2+1]<minY) minY = path->pts[(i+3)*2+1];
                if (path->pts[(i+3)*2+1]>maxY) maxY = path->pts[(i+3)*2+1];
                // maybe do the same thing for control point if issue appears with some paths going outside [-1,1]
            }
        }
    }

    // should not append
    if (minX >= maxValue || minY >= maxValue || maxX<=-maxValue || maxY<=-maxValue)
    {
        nsvgDelete(nsvgImage);
        return res;
    }

    // some case where we change type auto for LD_SVG_READING_SVGFRAME
    if (type==LD_SVG_READING_SVGFRAME) {

        // erase type if some error in svg file
        if (minX < 0 || minY < 0) type=LD_SVG_READING_MAXIMIZE;
        if (maxX > im_w || maxY > im_h) type=LD_SVG_READING_MAXIMIZE;
        if (fabs(im_w) <= 1.0f/maxValue || fabs(im_h) <= 1.0f/maxValue) type=LD_SVG_READING_MAXIMIZE;
    }

    // LD_SVG_READING_DEV
    // important ! do after check
    // still LD_SVG_READING_SVGFRAME ?
    if (type == LD_SVG_READING_DEV || type==LD_SVG_READING_SVGFRAME) {
        // all clear -> we change the min max value to frame size
        minX = 0;
        maxX = im_w;
        minY = 0;
        maxY = im_h;
    }

    // test after
    float max_w = maxX - minX;
    float max_h = maxY - minY;
    // should not append
    if (fabs(max_w) <= 1.0f/maxValue || fabs(max_h) <= 1.0f/maxValue)
    {
        nsvgDelete(nsvgImage);
        return res;
    }

    // check landscape or portrait
    float dx=0;
    float dy=0;
    float ratio = max_w>max_h ? 1.0f/max_w : 1.0f/max_h;
    if (max_w>max_h) {
        dy=0.5f-0.5f*(2.0f*max_h*ratio-1.0f);
    } else {
        dx=0.5f-0.5f*(2.0f*max_w*ratio-1.0f);
    }

    int pointLimiter = 4000;
    int pointLimiterCount = 0;

    // cout << "nsvgImage->bounds[0]: " << nsvgImage->bounds[0] << endl;
    for (NSVGshape* shape = nsvgImage->shapes; shape != NULL; shape = shape->next)
    {
        for (NSVGpath* path = shape->paths; path != NULL; path = path->next)
        {
            std::vector<BezierCurve> tmpCurves;
            for (int i = 0; i < path->npts-1; i += 3) {
                if (pointLimiter<=pointLimiterCount) continue;
                //
                float sx,sy,ex,ey,c1x,c1y,c2x,c2y;
                sx = path->pts[(i)*2]-minX;
                sy = path->pts[(i)*2+1]-minY;
                ex = path->pts[(i+3)*2]-minX;
                ey = path->pts[(i+3)*2+1]-minY;
                c1x = path->pts[(i+1)*2]-minX;
                c1y = path->pts[(i+1)*2+1]-minY;
                c2x = path->pts[(i+2)*2]-minX;
                c2y = path->pts[(i+2)*2+1]-minY;

                //
                if (type!=LD_SVG_READING_DEV)
                {
                    // resize to -1/1 keeping framesize
                    sx = 2.0f*sx*ratio - 1.0f + dx;
                    sy = 2.0f*sy*ratio - 1.0f + dy;
                    ex = 2.0f*ex*ratio - 1.0f + dx;
                    ey = 2.0f*ey*ratio - 1.0f + dy;
                    c1x = 2.0f*c1x*ratio - 1.0f + dx;
                    c1y = 2.0f*c1y*ratio - 1.0f + dy;
                    c2x = 2.0f*c2x*ratio - 1.0f + dx;
                    c2y = 2.0f*c2y*ratio - 1.0f + dy;
                }

                // y is inverted in svg
                sy = -sy;
                ey = -ey;
                c1y = -c1y;
                c2y = -c2y;

                // reduce a little if maximizing
                if (type==LD_SVG_READING_MAXIMIZE) {
                    float coef = 0.97f;
                    sx = coef*sx;
                    sy = coef*sy;
                    ex = coef*ex;
                    ey = coef*ey;
                    c1x = coef*c1x;
                    c1y = coef*c1y;
                    c2x = coef*c2x;
                    c2y = coef*c2y;
                }

                BezierCurve bzrCurve;
                bzrCurve.start = Vec2(sx, sy);
                bzrCurve.end = Vec2(ex, ey);
                bzrCurve.control1 = Vec2(c1x, c1y);
                bzrCurve.control2 = Vec2(c2x, c2y);
                bzrCurve.color = ldColorUtil::brgToRgb(shape->stroke.color);
                tmpCurves.push_back(bzrCurve);
                pointLimiterCount++;

                //qDebug()<< "sx"<< sx<< "sy"<< sy<< "ex"<< ex<< "ey"<< ey<< "c1x"<< c1x<< "c1y"<< c1y<< "c2x"<< c2x<< "c2y"<< c2y;
            }
            //qDebug()<< "tmpCurves  size:"<< tmpCurves.size();
            if (pointLimiter>=pointLimiterCount && tmpCurves.size()>0) res.push_back(tmpCurves);
        }
    }
    //qDebug()<< "res  size:"<< res.size();

    nsvgDelete(nsvgImage);

    if (snapDistance>0) res = snapCurves(res, snapDistance);

    // fix for simple (see twitter.svg)
    int totalPoints = ldSvgReader::totalPoints(res);
    if (type!=LD_SVG_READING_DEV && totalPoints < max_number_points_for_svg_positionning_fix) {
        // interpolate curve
        SvgDim dim = ldSvgReader::svgDimByInterpolation(res);
        // center svg
        //Vec2 p = Vec2(-1-dim.bottom_left.x + 1 - fabs(dim.top_right.x-dim.bottom_left.x)/2.0, -1-dim.bottom_left.y + 1 - fabs(dim.top_right.y-dim.bottom_left.y)/2.0);
        Vec2 p = Vec2(-dim.bottom_left.x - dim.width()/2.0f, -dim.bottom_left.y - dim.height()/2.0f);
        res = ldMaths::translateSvgBezierCurves(res, p);
    }

    //qDebug() << "EVERY minX:" << minX << " maxX:" << maxX << " minY:" << minY << " maxY:" << maxY;

    return res;
}

// loadSvgFixedScale
std::vector<std::vector<BezierCurve>> ldSvgReader::loadSvgFixedScale(float fixedMinX, float fixedMaxX, float fixedMinY, float fixedMaxY, char const* qtfilename, SvgReadingType p_type, float snapDistance)
{
    SvgReadingType type = p_type;

    std::vector<std::vector<BezierCurve>> res = std::vector<std::vector<BezierCurve>>();

    QFile file(qtfilename);
    if (!file.exists()) {
        qDebug() << "error::ldSvgReader::loadSvg: file does not exist" << qtfilename;
        return res;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "error::ldSvgReader::loadSvg: Qt file issue";
        return res;
    }

    QByteArray blob = file.readAll();

    NSVGimage* nsvgImage = nsvgParseFromData(blob.data(), blob.length(), "px", 96.0f);
    if (nsvgImage == NULL) {
        qDebug() << "nsvgImage == NULL";
        nsvgDelete(nsvgImage);
        return res;
    }

    //
    float maxValue = 100000000;
    float im_w = nsvgImage->width;
    float im_h = nsvgImage->height;

    // stats
    /*for (NSVGshape* shape = nsvgImage->shapes; shape != NULL; shape = shape->next)
    {
        //
        for (NSVGpath* path = shape->paths; path != NULL; path = path->next)
        {
            for (int i = 0; i < path->npts-1; i += 3)
            {
                // start
                //if (path->pts[(i)*2]<minX) minX = path->pts[(i)*2];
                //if (path->pts[(i)*2]>maxX) maxX = path->pts[(i)*2];
                //if (path->pts[(i)*2+1]<minY) minY = path->pts[(i)*2+1];
                //if (path->pts[(i)*2+1]>maxY) maxY = path->pts[(i)*2+1];
                // end
                //if (path->pts[(i+3)*2]<minX) minX = path->pts[(i+3)*2];
                //if (path->pts[(i+3)*2]>maxX) maxX = path->pts[(i+3)*2];
                //if (path->pts[(i+3)*2+1]<minY) minY = path->pts[(i+3)*2+1];
                //if (path->pts[(i+3)*2+1]>maxY) maxY = path->pts[(i+3)*2+1];
                // maybe do the same thing for control point if issue appears with some paths going outside [-1,1]
            }
        }
    }*/

    // should not append
    if (fixedMinX >= maxValue || fixedMinY >= maxValue || fixedMaxX<=-maxValue || fixedMaxY<=-maxValue)
    {
        nsvgDelete(nsvgImage);
        return res;
    }

    // some case where we change type auto for LD_SVG_READING_SVGFRAME
    if (type==LD_SVG_READING_SVGFRAME) {

        // erase type if some error in svg file
        if (fixedMinX < 0 || fixedMinY < 0) type=LD_SVG_READING_MAXIMIZE;
        if (fixedMaxX > im_w || fixedMaxY > im_h) type=LD_SVG_READING_MAXIMIZE;
        if (fabs(im_w) <= 1.0f/maxValue || fabs(im_h) <= 1.0f/maxValue) type=LD_SVG_READING_MAXIMIZE;
    }

    // LD_SVG_READING_DEV
    // important ! do after check
    // still LD_SVG_READING_SVGFRAME ?
    /*if (type == LD_SVG_READING_DEV || type==LD_SVG_READING_SVGFRAME) {
        // all clear -> we change the min max value to frame size
        minX = 0;
        maxX = im_w;
        minY = 0;
        maxY = im_h;
    }*/

    // test after
    float max_w = fixedMaxX - fixedMinX;
    float max_h = fixedMaxY - fixedMinY;
    // should not append
    if (fabs(max_w) <= 1.0f/maxValue || fabs(max_h) <= 1.0f/maxValue)
    {
        nsvgDelete(nsvgImage);
        return res;
    }

    // check landscape or portrait
    float dx=0;
    float dy=0;
    float ratio = max_w>max_h ? 1.0f/max_w : 1.0f/max_h;
    if (max_w>max_h) {
        dy=0.5f-0.5f*(2.0f*max_h*ratio-1.0f);
    } else {
        dx=0.5f-0.5f*(2.0f*max_w*ratio-1.0f);
    }

    int pointLimiter = 4000;
    int pointLimiterCount = 0;

    for (NSVGshape* shape = nsvgImage->shapes; shape != NULL; shape = shape->next)
    {
        for (NSVGpath* path = shape->paths; path != NULL; path = path->next)
        {
            std::vector<BezierCurve> tmpCurves;
            for (int i = 0; i < path->npts-1; i += 3) {
                if (pointLimiter<=pointLimiterCount) continue;
                //
                float sx,sy,ex,ey,c1x,c1y,c2x,c2y;
                sx = path->pts[(i)*2]-fixedMinX;
                sy = path->pts[(i)*2+1]-fixedMinY;
                ex = path->pts[(i+3)*2]-fixedMinX;
                ey = path->pts[(i+3)*2+1]-fixedMinY;
                c1x = path->pts[(i+1)*2]-fixedMinX;
                c1y = path->pts[(i+1)*2+1]-fixedMinY;
                c2x = path->pts[(i+2)*2]-fixedMinX;
                c2y = path->pts[(i+2)*2+1]-fixedMinY;

                //
                if (type!=LD_SVG_READING_DEV)
                {
                    // resize to -1/1 keeping framesize
                    sx = 2.0f*sx*ratio - 1.0f + dx;
                    sy = 2.0f*sy*ratio - 1.0f + dy;
                    ex = 2.0f*ex*ratio - 1.0f + dx;
                    ey = 2.0f*ey*ratio - 1.0f + dy;
                    c1x = 2.0f*c1x*ratio - 1.0f + dx;
                    c1y = 2.0f*c1y*ratio - 1.0f + dy;
                    c2x = 2.0f*c2x*ratio - 1.0f + dx;
                    c2y = 2.0f*c2y*ratio - 1.0f + dy;
                }

                // y is inverted in svg
                sy = -sy;
                ey = -ey;
                c1y = -c1y;
                c2y = -c2y;

                // reduce a little if maximizing
                if (type==LD_SVG_READING_MAXIMIZE) {
                    float coef = 0.97f;
                    sx = coef*sx;
                    sy = coef*sy;
                    ex = coef*ex;
                    ey = coef*ey;
                    c1x = coef*c1x;
                    c1y = coef*c1y;
                    c2x = coef*c2x;
                    c2y = coef*c2y;
                }

                BezierCurve bzrCurve;
                bzrCurve.start = Vec2(sx, sy);
                bzrCurve.end = Vec2(ex, ey);
                bzrCurve.control1 = Vec2(c1x, c1y);
                bzrCurve.control2 = Vec2(c2x, c2y);
                bzrCurve.color = ldColorUtil::brgToRgb(shape->stroke.color);
                tmpCurves.push_back(bzrCurve);
                pointLimiterCount++;
            }
            if (pointLimiter>=pointLimiterCount && tmpCurves.size()>0) res.push_back(tmpCurves);
        }
    }
    nsvgDelete(nsvgImage);

    if (snapDistance>0) res = snapCurves(res, snapDistance);

    // fix for simple (see twitter.svg)
    int totalPoints = ldSvgReader::totalPoints(res);
    if (type!=LD_SVG_READING_DEV && totalPoints < max_number_points_for_svg_positionning_fix) {
        // interpolate curve
        SvgDim dim = ldSvgReader::svgDimByInterpolation(res);
        // center svg
        Vec2 p = Vec2(-dim.bottom_left.x - dim.width()/2.0f, -dim.bottom_left.y - dim.height()/2.0f);
        res = ldMaths::translateSvgBezierCurves(res, p);
    }
    return res;
}


// svgDim
SvgDim ldSvgReader::svgDim(const svgBezierCurves &bezierCurves)
{
    // if too few points
    if (ldSvgReader::totalPoints(bezierCurves) < max_number_points_for_svg_positionning_fix)
    {
        // interpolate curve
        return ldSvgReader::svgDimByInterpolation(bezierCurves);
    }

    SvgDim res;
    bool init = false;
    for (const std::vector<BezierCurve> &curve : bezierCurves) {
        for (const BezierCurve &b : curve) {
            if (!init) {
                // very first point
                res.bottom_left = b.start;
                res.top_right = b.end;
                init = true;
                continue;
            }
            // bottom left versus start
            res.bottom_left.x = std::min(res.bottom_left.x, b.start.x);
            res.bottom_left.y = std::min(res.bottom_left.y, b.start.y);
            // bottom left versus end
            res.bottom_left.x = std::min(res.bottom_left.x, b.end.x);
            res.bottom_left.y = std::min(res.bottom_left.y, b.end.y);

            // top right versus start
            res.top_right.x = std::max(res.top_right.x, b.start.x);
            res.top_right.y = std::max(res.top_right.y, b.start.y);
            // top right versus end
            res.top_right.x = std::max(res.top_right.x, b.end.x);
            res.top_right.y = std::max(res.top_right.y, b.end.y);
        }
    }
    return res;
}


Svg3dDim ldSvgReader::svgDim(const svgBezier3dCurves &bezierCurves)
{
    // if too few points
    if (ldSvgReader::totalPoints(bezierCurves) < max_number_points_for_svg_positionning_fix)
    {
        // interpolate curve
        return ldSvgReader::svgDimByInterpolation(bezierCurves);
    }

    Svg3dDim res;
    bool init = false;
    for (const std::vector<Bezier3dCurve> &curve : bezierCurves) {
        for (const Bezier3dCurve &b : curve) {
            if (!init) {
                // very first point
                res.bottom_left = b.start;
                res.top_right = b.end;
                init = true;
                continue;
            }
            // bottom left versus start
            res.bottom_left.x = std::min(res.bottom_left.x, b.start.x);
            res.bottom_left.y = std::min(res.bottom_left.y, b.start.y);
            res.bottom_left.z = std::min(res.bottom_left.z, b.start.z);
            // bottom left versus end
            res.bottom_left.x = std::min(res.bottom_left.x, b.end.x);
            res.bottom_left.y = std::min(res.bottom_left.y, b.end.y);
            res.bottom_left.z = std::min(res.bottom_left.z, b.end.z);

            // top right versus start
            res.top_right.x = std::max(res.top_right.x, b.start.x);
            res.top_right.y = std::max(res.top_right.y, b.start.y);
            res.top_right.z = std::max(res.top_right.z, b.start.z);
            // top right versus end
            res.top_right.x = std::max(res.top_right.x, b.end.x);
            res.top_right.y = std::max(res.top_right.y, b.end.y);
            res.top_right.z = std::max(res.top_right.z, b.end.z);
        }
    }
    return res;}

// svgDimByInterpolation
SvgDim ldSvgReader::svgDimByInterpolation(const svgBezierCurves &bezierCurves, int p_interpolate)
{
    SvgDim res;
    bool init = false;
    for (const std::vector<BezierCurve> &curve : bezierCurves) {
        for (const BezierCurve &b : curve) {
            if (!init) {
                // very first point
                res.bottom_left = b.start;
                res.top_right = b.end;
                init = true;
            }

            //
            for (int k=0; k<p_interpolate; k++)
            {
                float slope = 1.0f*k/(p_interpolate-1);
                float x = ldMaths::cubicBezier(slope, b.start.x, b.control1.x, b.control2.x, b.end.x);
                float y = ldMaths::cubicBezier(slope, b.start.y, b.control1.y, b.control2.y, b.end.y);

                // bottom left versus start
                if (res.bottom_left.x >= x) res.bottom_left.x = x;
                if (res.bottom_left.y >= y) res.bottom_left.y = y;

                // top right versus start
                if (res.top_right.x <= x) res.top_right.x = x;
                if (res.top_right.y <= y) res.top_right.y = y;
            }
        }
    }
    return res;
}


Svg3dDim ldSvgReader::svgDimByInterpolation(const svgBezier3dCurves &bezierCurves, int p_interpolate)
{
    Svg3dDim res;
    bool init = false;
    for (const std::vector<Bezier3dCurve> &curve : bezierCurves) {
        for (const Bezier3dCurve &b : curve) {
            if (!init) {
                // very first point
                res.bottom_left = b.start;
                res.top_right = b.end;
                init = true;
            }

            //
            for (int k=0; k<p_interpolate; k++)
            {
                float slope = 1.0f*k/(p_interpolate-1);
                float x = ldMaths::cubicBezier(slope, b.start.x, b.control1.x, b.control2.x, b.end.x);
                float y = ldMaths::cubicBezier(slope, b.start.y, b.control1.y, b.control2.y, b.end.y);
                float z = ldMaths::cubicBezier(slope, b.start.z, b.control1.z, b.control2.z, b.end.z);

                // bottom left versus start
                if (res.bottom_left.x >= x) res.bottom_left.x = x;
                if (res.bottom_left.y >= y) res.bottom_left.y = y;
                if (res.bottom_left.z >= z) res.bottom_left.z = z;

                // top right versus start
                if (res.top_right.x <= x) res.top_right.x = x;
                if (res.top_right.y <= y) res.top_right.y = y;
                if (res.top_right.z <= z) res.top_right.z = z;
            }
        }
    }
    return res;
}

// svgDimSequence
SvgDim ldSvgReader::svgDimSequence(const svgBezierCurvesSequence &bezierCurvesSequence)
{
    //qDebug()<<"svgDimSequence " ;

    SvgDim res;
    res.bottom_left = Vec2();
    res.top_right = Vec2();
    bool init = false;
    //
    for (const svgBezierCurves &curves : bezierCurvesSequence) {
        if (!init) {
            res = ldSvgReader::svgDim(curves);
//            qDebug()<<"res_tmp .bottom_left.x "<< res.bottom_left.x ;
//            qDebug()<<"res_tmp .top_right "<< res.top_right.x ;
//
            init = true;
            continue;
        }
        SvgDim res_tmp = ldSvgReader::svgDim(curves);


//        qDebug()<<"res_tmp .bottom_left.x "<< res_tmp.bottom_left.x ;
//        qDebug()<<"res_tmp .top_right "<< res_tmp.top_right.x ;
//

        // bottom left
        if (res_tmp.bottom_left.x <= res.bottom_left.x) res.bottom_left.x = res_tmp.bottom_left.x;
        if (res_tmp.bottom_left.y <= res.bottom_left.y) res.bottom_left.y = res_tmp.bottom_left.y;

        if (res_tmp.top_right.x <= res.bottom_left.x) res.bottom_left.x = res_tmp.top_right.x;
        if (res_tmp.top_right.y <= res.bottom_left.y) res.bottom_left.y = res_tmp.top_right.y;

        // top right
        if (res_tmp.top_right.x >= res.top_right.x) res.top_right.x = res_tmp.top_right.x;
        if (res_tmp.top_right.y >= res.top_right.y) res.top_right.y = res_tmp.top_right.y;

        if (res_tmp.bottom_left.x >= res.top_right.x) res.top_right.x = res_tmp.bottom_left.x;
        if (res_tmp.bottom_left.y >= res.top_right.y) res.top_right.y = res_tmp.bottom_left.y;
    }

    return res;
}

svgBezierCurvesSequence ldSvgReader::loadFromJSArrayText(const char* text) {

    svgBezierCurvesSequence curves;

    typedef std::pair<float, float> tpoint;
    typedef std::vector<tpoint> tline;
    typedef std::vector<tline> tdigit;
    typedef std::vector<tdigit> tfont;

    int numindex = -1;
    int charindex = 0;
    tfont font;
    tdigit digit;
    tline line;
    tpoint point;
    int levels = 0;
    bool aftercomma = false;
    while (text[charindex] != 0) {
        char c = text[charindex];
        {

            bool isdigit = c == '.' || (c >= '0' && c <= '9');
            if (isdigit && numindex == -1) {
                // mark start of number
                numindex = charindex;
            }
            if (numindex != -1 && !isdigit) {
                // read number
                float f = atof(text + numindex);
                numindex = -1;
                if (aftercomma) point.second = f; else point.first = f;
            }
            if (c == ',' && levels == 4) {
                // determines x or y value
                aftercomma = true;
            }
        }
        {
            // start new point (or line, etc)
            if (c == '[') {
                levels++;
                if (levels == 1) {
                    font.resize(0);
                }
                if (levels == 2) {
                    digit.resize(0);
                }
                if (levels == 3) {
                    line.resize(0);
                }
                if (levels == 4) {
                    aftercomma = false;
                    point.second = point.first = 0;
                }
            }
            // end current point (or line, etc) and add to line (or digit, etc)
            if (c == ']') {
                levels--;
                if (levels == 3) {
                    line.push_back(point);
                }
                if (levels == 2) {
                    digit.push_back(line);
                }
                if (levels == 1) {
                    font.push_back(digit);
                }
            }
        }
        charindex++;
    }


    size_t maxstrokes = 0;
    for (size_t i = 0; i < font.size(); i++) {
        tdigit d = font[i];
        //qDebug() << "digit " << i;
        svgBezierCurves b;
        if (d.size() > maxstrokes) maxstrokes = d.size();
        for (size_t j = 0; j < d.size(); j++) {
            //qDebug() << "line " << j;
            tline l = d[j];
            if (l.size() == 4) {
                BezierCurve t;
                t.start.x =    l[0].first;
                t.start.y =    l[0].second;
                t.end.x =      l[3].first;
                t.end.y =      l[3].second;
                t.control1.x = l[1].first;
                t.control1.y = l[1].second;
                t.control2.x = l[2].first;
                t.control2.y = l[2].second;
                t.color = 0xffffffff;
                std::vector<BezierCurve> stroke;
                stroke.push_back(t);
                b.push_back(stroke);
            }
        }
        curves.push_back(b);
    }

    // fill in
    for (size_t i = 0; i < curves.size(); i++) {
        for (size_t j = 0; j < maxstrokes; j++) {
            if (j >= curves[i].size()) curves[i].push_back(curves[i][curves[i].size()-1]);
        }
    }

    for (size_t i = 0; i < curves.size(); i++) {
        // center svg
        //curves[i] = ldMaths::scaleSvgBezierCurves(curves[i], 1.0f/500.0f);
        //SvgDim dim = ldSvgReader::svgDimByInterpolation(curves[i]);
        //Vec2 p = Vec2(-dim.bottom_left.x - dim.width()/2.0f, -dim.bottom_left.y - dim.height()/2.0f);
        //curves[i] = ldMaths::translateSvgBezierCurves(curves[i], p);
    }

    return curves;

}

