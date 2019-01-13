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

#include "ldCore/Helpers/SVG/ldSvgReader.h"

#include <string>

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QString>

#define NANOSVG_IMPLEMENTATION
#include <nanosvg-master/src/nanosvg.h>

#include "ldCore/Helpers/Color/ldColorUtil.h"
#include "ldCore/Helpers/SVG/ldSvgDir.h"

namespace {
    const int MAX_NUMBER_POINTS_FOR_SVG_POSITIONING_FIX = 200;
}


// totalPoints
int ldSvgReader::totalPoints(const svgBezierCurves &bezierCurves)
{
    int pointCounter = 0;
    for (const std::vector<ldBezierCurve> &curve : bezierCurves) {
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

svgBezierCurvesSequence ldSvgReader::loadSvgSequence(const QString &dirPath, Type p_type, float snapDistance, const QString &filePrefix, int masksize)
{
    svgBezierCurvesSequence res;

    ldSvgDir svgDir(dirPath);
    if(masksize != -1)
        svgDir.setMaskSize(masksize);
    if(!filePrefix.isEmpty())
        svgDir.setFilePrefix(filePrefix);

    QStringList svgFiles = svgDir.getSvgFiles();
    for(const QString &svgFile : svgFiles) {
        res.push_back(ldSvgReader::loadSvg(dirPath + "/" + svgFile, p_type, snapDistance, false));
    }
    return res;
}

// loadSvgSequenceFixedScale
svgBezierCurvesSequence ldSvgReader::loadSvgSequenceFixedScale(float fixedMinX, float fixedMaxX, float fixedMinY, float fixedMaxY, int size, int masksize, char const* qtbasefilename, Type p_type, float snapDistance)
{
    svgBezierCurvesSequence res = svgBezierCurvesSequence();
    //
    for (int i=1;i<size+1;i++) {
        // qDebug() << "i:" << i;
        QString stringWork(qtbasefilename);
        stringWork += QString::number(i).rightJustified(masksize, '0');
        stringWork.append(".svg");
        // qDebug() << " > i:" << i << "stringWork " << stringWork->toStdString().c_str();
        svgBezierCurves tmp = ldSvgReader::loadSvgFixedScale(fixedMinX, fixedMaxX, fixedMinY, fixedMaxY, stringWork, p_type, snapDistance);
        res.push_back(tmp);
    }
    return res;
}

// loadSvg
svgBezierCurves ldSvgReader::loadSvg(QString qtfilename, Type type, float snapDistance, bool isTranslate)
{
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

    // some case where we change type auto for ldSvgReader::Type::SvgFrame
    if (type==Type::SvgFrame) {

        // erase type if some error in svg file
        if (minX < 0 || minY < 0) type=Type::Maximize;
        if (maxX > im_w || maxY > im_h) type=Type::Maximize;
        if (fabs(im_w) <= 1.0f/maxValue || fabs(im_h) <= 1.0f/maxValue) type=Type::Maximize;
    }

    // ldSvgReader::Type::Dev
    // important ! do after check
    // still ldSvgReader::Type::SvgFrame ?
    if (type == Type::Dev || type==Type::SvgFrame) {
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

    const int MAX_POINTS = 4000;
    int pointLimiterCount = 0;

    // cout << "nsvgImage->bounds[0]: " << nsvgImage->bounds[0] << endl;
    ldBezierCurve bzrCurve;
    for (NSVGshape* shape = nsvgImage->shapes; shape != NULL; shape = shape->next)
    {
        for (NSVGpath* path = shape->paths; path != NULL; path = path->next)
        {
            std::vector<ldBezierCurve> tmpCurves;
            for (int i = 0; i < path->npts-1; i += 3) {
                if (pointLimiterCount >= MAX_POINTS) continue;
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
                if (type!=Type::Dev)
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
                if (type==Type::Maximize) {
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

                bzrCurve.setStart(Vec2(sx, sy));
                bzrCurve.setEnd(Vec2(ex, ey));
                bzrCurve.setControl1(Vec2(c1x, c1y));
                bzrCurve.setControl2(Vec2(c2x, c2y));
                bzrCurve.setColor(ldColorUtil::brgToRgb(shape->stroke.color));
                tmpCurves.push_back(bzrCurve);
                pointLimiterCount++;

                //qDebug()<< "sx"<< sx<< "sy"<< sy<< "ex"<< ex<< "ey"<< ey<< "c1x"<< c1x<< "c1y"<< c1y<< "c2x"<< c2x<< "c2y"<< c2y;
            }
            //qDebug()<< "tmpCurves  size:"<< tmpCurves.size();
            if (pointLimiterCount < MAX_POINTS && tmpCurves.size()>0) res.push_back(tmpCurves);
        }
    }
    //qDebug()<< "res  size:"<< res.size();

    nsvgDelete(nsvgImage);

    if (snapDistance>0) res = snapCurves(res, snapDistance);

    // fix for simple (see twitter.svg)
    int totalPoints = ldSvgReader::totalPoints(res);
    if (type!=Type::Dev && totalPoints < MAX_NUMBER_POINTS_FOR_SVG_POSITIONING_FIX && isTranslate) {
        // interpolate curve
        SvgDim dim = ldSvgReader::svgDimByInterpolation(res);
        // center svg
        //Vec2 p = Vec2(-1-dim.bottom_left.x + 1 - fabs(dim.top_right.x-dim.bottom_left.x)/2.0, -1-dim.bottom_left.y + 1 - fabs(dim.top_right.y-dim.bottom_left.y)/2.0);
        Vec2 p = Vec2(-dim.bottom_left.x - dim.width()/2.0f, -dim.bottom_left.y - dim.height()/2.0f);
        ldMaths::translateSvgBezierCurves(res, p);
    }

    //qDebug() << "EVERY minX:" << minX << " maxX:" << maxX << " minY:" << minY << " maxY:" << maxY;

    return res;
}

// loadSvgFixedScale
std::vector<std::vector<ldBezierCurve>> ldSvgReader::loadSvgFixedScale(float fixedMinX, float fixedMaxX, float fixedMinY, float fixedMaxY, const QString &qtfilename, Type p_type, float snapDistance)
{
    Type type = p_type;

    std::vector<std::vector<ldBezierCurve>> res = std::vector<std::vector<ldBezierCurve>>();

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

    // some case where we change type auto for ldSvgReader::Type::SvgFrame
    if (type==Type::SvgFrame) {

        // erase type if some error in svg file
        if (fixedMinX < 0 || fixedMinY < 0) type=Type::Maximize;
        if (fixedMaxX > im_w || fixedMaxY > im_h) type=Type::Maximize;
        if (fabs(im_w) <= 1.0f/maxValue || fabs(im_h) <= 1.0f/maxValue) type=Type::Maximize;
    }

    // ldSvgReader::Type::Dev
    // important ! do after check
    // still ldSvgReader::Type::SvgFrame ?
    /*if (type == ldSvgReader::Type::Dev || type==ldSvgReader::Type::SvgFrame) {
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
            std::vector<ldBezierCurve> tmpCurves;
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
                if (type!=Type::Dev)
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
                if (type==Type::Maximize) {
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

                ldBezierCurve bzrCurve;
                bzrCurve.setStart(Vec2(sx, sy));
                bzrCurve.setEnd(Vec2(ex, ey));
                bzrCurve.setControl1(Vec2(c1x, c1y));
                bzrCurve.setControl2(Vec2(c2x, c2y));
                bzrCurve.setColor(ldColorUtil::brgToRgb(shape->stroke.color));
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
    if (type!=Type::Dev && totalPoints < MAX_NUMBER_POINTS_FOR_SVG_POSITIONING_FIX) {
        // interpolate curve
        SvgDim dim = ldSvgReader::svgDimByInterpolation(res);
        // center svg
        Vec2 p = Vec2(-dim.bottom_left.x - dim.width()/2.0f, -dim.bottom_left.y - dim.height()/2.0f);
        ldMaths::translateSvgBezierCurves(res, p);
    }
    return res;
}


// svgDim
SvgDim ldSvgReader::svgDim(const svgBezierCurves &bezierCurves)
{
    // if too few points
    if (ldSvgReader::totalPoints(bezierCurves) < MAX_NUMBER_POINTS_FOR_SVG_POSITIONING_FIX)
    {
        // interpolate curve
        return ldSvgReader::svgDimByInterpolation(bezierCurves);
    }

    SvgDim res;
    bool init = false;
    for (const std::vector<ldBezierCurve> &curve : bezierCurves) {
        for (const ldBezierCurve &b : curve) {
            if (!init) {
                // very first point
                res.bottom_left = b.start();
                res.top_right = b.end();
                init = true;
                continue;
            }
            // bottom left versus start
            res.bottom_left.x = std::min(res.bottom_left.x, b.start().x);
            res.bottom_left.y = std::min(res.bottom_left.y, b.start().y);
            // bottom left versus end
            res.bottom_left.x = std::min(res.bottom_left.x, b.end().x);
            res.bottom_left.y = std::min(res.bottom_left.y, b.end().y);

            // top right versus start
            res.top_right.x = std::max(res.top_right.x, b.start().x);
            res.top_right.y = std::max(res.top_right.y, b.start().y);
            // top right versus end
            res.top_right.x = std::max(res.top_right.x, b.end().x);
            res.top_right.y = std::max(res.top_right.y, b.end().y);
        }
    }
    return res;
}


Svg3dDim ldSvgReader::svgDim(const svgBezier3dCurves &bezierCurves)
{
    // if too few points
    if (ldSvgReader::totalPoints(bezierCurves) < MAX_NUMBER_POINTS_FOR_SVG_POSITIONING_FIX)
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
    for (const std::vector<ldBezierCurve> &curve : bezierCurves) {
        for (const ldBezierCurve &b : curve) {
            if (!init) {
                // very first point
                res.bottom_left = b.start();
                res.top_right = b.end();
                init = true;
            }

            //
            for (int k=0; k<p_interpolate; k++)
            {
                float slope = 1.0f*k/(p_interpolate-1);
                Vec2 p = b.getPoint(slope);

                // bottom left versus start
                if (res.bottom_left.x >= p.x) res.bottom_left.x = p.x;
                if (res.bottom_left.y >= p.y) res.bottom_left.y = p.y;

                // top right versus start
                if (res.top_right.x <= p.x) res.top_right.x = p.x;
                if (res.top_right.y <= p.y) res.top_right.y = p.y;
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
            for (int i=0; i<p_interpolate; i++)
            {
                float slope = 1.0f*i/(p_interpolate-1);
                point3d p = b.getPoint(slope);

                // bottom left versus start
                if (res.bottom_left.x >= p.x) res.bottom_left.x = p.x;
                if (res.bottom_left.y >= p.y) res.bottom_left.y = p.y;
                if (res.bottom_left.z >= p.z) res.bottom_left.z = p.z;

                // top right versus start
                if (res.top_right.x <= p.x) res.top_right.x = p.x;
                if (res.top_right.y <= p.y) res.top_right.y = p.y;
                if (res.top_right.z <= p.z) res.top_right.z = p.z;
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
                ldBezierCurve t;
                t.setStart(Vec2(l[0].first, l[0].second));
                t.setControl1(Vec2(l[1].first, l[1].second));
                t.setControl2(Vec2(l[2].first, l[2].second));
                t.setEnd(Vec2(l[3].first, l[3].second));
                t.setColor(0xffffffff);
                std::vector<ldBezierCurve> stroke;
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

// debugStatCurves
void ldSvgReader::debugStatCurves(svgBezierCurves bezierCurves)
{
    int pointCounter = 0;
    for (uint i=0; i<bezierCurves.size(); i++) {
        std::vector<ldBezierCurve> curve = bezierCurves[i];
        pointCounter += curve.size();
    }
    qDebug() << "nb curve: " << bezierCurves.size();
    qDebug() << "nb bezier points: " << pointCounter;
}

// snapCurves
svgBezierCurves ldSvgReader::snapCurves(const svgBezierCurves &bezierCurves, float snapDistance)
{
    svgBezierCurves res;
    //debugStatCurves(bezierCurves);
    std::vector<ldBezierCurve> newOne;
    for (uint i=0; i<bezierCurves.size(); i++) {
        const std::vector<ldBezierCurve> &curve = bezierCurves[i];
        //
        for (uint c=0; c<curve.size(); c++) {
            const ldBezierCurve &a = curve[c];
            //qDebug() << "push_back a for c:" << c ;
            if (c == 0 && i>0) { // && i<bezierCurves.size()-1
                const std::vector<ldBezierCurve> &prevcurve = bezierCurves[i-1];
                const ldBezierCurve &b = prevcurve[prevcurve.size()-1];
                if (fabs(b.end().x - a.start().x)<snapDistance && fabs(b.end().y - a.start().y)<snapDistance) {
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
