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

#include "ldCore/Helpers/Color/ldColorUtil.h"
#include <ldCore/Helpers/SimpleCrypt/ldSimpleCrypt.h>
#include "ldCore/Helpers/SVG/ldSvgDir.h"
#include "ldCore/Render/ldRendererOpenlase.h"

#include <nanosvg-master/src/nanosvg.h>

namespace {
    const int MAX_NUMBER_POINTS_FOR_SVG_POSITIONING_FIX = 200;
}


// totalPoints
int ldSvgReader::totalPoints(const ldBezierPaths &bezierPaths)
{
    int pointCounter = 0;
    for (const ldBezierPath &path : bezierPaths) {
        pointCounter += path.data().size();
    }
    return pointCounter;
}

// totalPoints
int ldSvgReader::totalPoints(const ld3dBezierCurves &bezierCurves)
{
    int pointCounter = 0;
    for (const std::vector<ld3dBezierCurve> &curve : bezierCurves) {
        pointCounter += curve.size();
    }
    return pointCounter;
}

ldBezierPathsSequence ldSvgReader::loadSvgSequence(const QString &dirPath, Type p_type, float snapDistance, const QString &filePrefix, int masksize)
{
    ldBezierPathsSequence res;

    ldSvgDir svgDir(dirPath);
    if(masksize != -1)
        svgDir.setMaskSize(masksize);
    if(!filePrefix.isEmpty())
        svgDir.setFilePrefix(filePrefix);

    QStringList svgFiles = svgDir.getSvgFiles();
    for(const QString &svgFile : svgFiles) {
        res.push_back(ldSvgReader::loadSvg(dirPath + "/" + svgFile, p_type, snapDistance, false).data());
    }
    return res;
}

// loadSvgSequenceFixedScale
ldBezierPathsSequence ldSvgReader::loadSvgSequenceFixedScale(float fixedMinX, float fixedMaxX, float fixedMinY, float fixedMaxY, int size, int masksize, char const* qtbasefilename, Type p_type, float snapDistance)
{
    ldBezierPathsSequence res = ldBezierPathsSequence();
    //
    for (int i=1;i<size+1;i++) {
        // qDebug() << "i:" << i;
        QString stringWork(qtbasefilename);
        stringWork += QString::number(i).rightJustified(masksize, '0');
        stringWork.append(".svg");
        // qDebug() << " > i:" << i << "stringWork " << stringWork->toStdString().c_str();
        ldBezierPaths tmp = ldSvgReader::loadSvg(stringWork, p_type, snapDistance, true, fixedMinX, fixedMaxX, fixedMinY, fixedMaxY).data();
        res.push_back(tmp);
    }
    return res;
}

// loadSvg
ldBezierCurveObject ldSvgReader::loadSvg(QString qtfilename, Type type, float snapDistance, bool isTranslate,
                                         float fixedMinX, float fixedMaxX, float fixedMinY, float fixedMaxY)
{
    ldBezierCurveObject res;

    //qDebug() << "ldSvgReader::loadSvg " << qtfilename;

    QByteArray blob = readFile(qtfilename);
    if(blob.isEmpty())
        return res;

    std::unique_ptr<NSVGimage, void(*)(NSVGimage*)> nsvgImage(nsvgParse(blob.data(), "px", 96.0f), nsvgDelete);
    if (nsvgImage == NULL) {
        qDebug() << "nsvgImage == NULL";
        return res;
    }

    //
    float maxValue = 100000000;
    float im_w = nsvgImage->width;
    float im_h = nsvgImage->height;

    // stats
    float minX = fixedMinX;
    float minY = fixedMinY;
    float maxX = fixedMaxX;
    float maxY = fixedMaxY;

    bool isNotFixed = (minX == -1.f && minY == -1.f && maxX == -1.f&& maxY == -1.f);
    if(isNotFixed) {
        minX = maxValue ;
        minY = maxValue;
        maxX = -maxValue;
        maxY = -maxValue;

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

    }

    // should not append
    if (minX >= maxValue || minY >= maxValue || maxX<=-maxValue || maxY<=-maxValue)
    {
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
    if ((type == Type::Dev || type==Type::SvgFrame) && isNotFixed) {
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

    // resize to -1/1 keeping framesize
    auto resizeFrameSize = [&](float value, float d) -> float {
      return  2.0f*value*ratio - 1.0f + d;
    };

    const float MAX_COEFF = 0.97f;
    auto transformXCoord = [&](float value) {
        value = value - minX;
        if (type!=Type::Dev) {
            value = resizeFrameSize(value, dx);
        }
        if (type==Type::Maximize) {
            value = MAX_COEFF*value;
        }
        return value;
    };

    auto transformYCoord = [&](float value) {
        value = value - minY;
        if (type!=Type::Dev) {
            value = resizeFrameSize(value, dy);
        }
        value = -value;
        if (type==Type::Maximize) {
            value = MAX_COEFF*value;
        }
        return value;
    };

    ldBezierCurve bzrCurve;
    bool isColored = true;
    for (NSVGshape* shape = nsvgImage->shapes; shape != NULL; shape = shape->next)
    {
        for (NSVGpath* path = shape->paths; path != NULL; path = path->next)
        {
            ldBezierPath bezierPath;
            switch (shape->stroke.type) {
            case NSVG_PAINT_NONE:
                isColored = false;
                bezierPath.setColor(C_WHITE);
                break;
            case NSVG_PAINT_COLOR:
                bezierPath.setColor(ldColorUtil::bgrToRgb(shape->stroke.color));
                break;
            case NSVG_PAINT_LINEAR_GRADIENT: {
                NSVGgradient *gradient = shape->stroke.gradient;

                // get svg x1, y1, x2, y2 positions
                std::vector<float> t(6, 0);
                nsvg__xformInverse(t.data(), gradient->xform);
                float x1 = t[4];
                float y1 = t[5];
                float x2 = t[4] + t[2];
                float y2 = t[5] + t[3];
                x1 = transformXCoord(x1);
                y1 = transformYCoord(y1);
                x2 = transformXCoord(x2);
                y2 = transformYCoord(y2);

                ldGradient bzrGradient;
                bzrGradient.setX1(x1);
                bzrGradient.setY1(y1);
                bzrGradient.setX2(x2);
                bzrGradient.setY2(y2);

                for(int stopIndex = 0; stopIndex < gradient->nstops; stopIndex++) {
                    const NSVGgradientStop &nsvgStop = gradient->stops[stopIndex];
                    bzrGradient.add(ldGradientStop(nsvgStop.offset, ldColorUtil::abgrToArgb(nsvgStop.color)));
                }
                bezierPath.setGradient(bzrGradient);
            }
                break;
            case NSVG_PAINT_RADIAL_GRADIENT:
                qWarning() << "NSVG_PAINT_RADIAL_GRADIENT IS NOT SUPPORTED";
                break;
            }

            for (int i = 0; i < path->npts-1; i += 3) {
                if (pointLimiterCount >= MAX_POINTS) continue;
                //
                float sx = path->pts[(i)*2];
                float sy = path->pts[(i)*2+1];
                float ex = path->pts[(i+3)*2];
                float ey = path->pts[(i+3)*2+1];
                float c1x = path->pts[(i+1)*2];
                float c1y = path->pts[(i+1)*2+1];
                float c2x = path->pts[(i+2)*2];
                float c2y = path->pts[(i+2)*2+1];

                sx = transformXCoord(sx);
                sy = transformYCoord(sy);
                ex = transformXCoord(ex);
                ey = transformYCoord(ey);
                c1x = transformXCoord(c1x);
                c1y = transformYCoord(c1y);
                c2x = transformXCoord(c2x);
                c2y = transformYCoord(c2y);

                bzrCurve.setStart(ldVec2(sx, sy));
                bzrCurve.setEnd(ldVec2(ex, ey));
                bzrCurve.setControl1(ldVec2(c1x, c1y));
                bzrCurve.setControl2(ldVec2(c2x, c2y));

                bezierPath.add(bzrCurve);
                pointLimiterCount++;

                //qDebug()<< "sx"<< sx<< "sy"<< sy<< "ex"<< ex<< "ey"<< ey<< "c1x"<< c1x<< "c1y"<< c1y<< "c2x"<< c2x<< "c2y"<< c2y;
            }
            //qDebug()<< "tmpCurves  size:"<< tmpCurves.size();
            if (pointLimiterCount < MAX_POINTS && bezierPath.size()>0)
                res.add(bezierPath);
        }
    }
    //qDebug()<< "res  size:"<< res.size();

    if (snapDistance>0 && !isColored) res = ldBezierCurveObject(snapCurves(res.data(), snapDistance));

    // fix for simple (see twitter.svg)
    int totalPoints = res.totalPoints();
    if (type!=Type::Dev && totalPoints < MAX_NUMBER_POINTS_FOR_SVG_POSITIONING_FIX && isTranslate) {
        // interpolate curve
        ldRect dim = res.dim();
        // center svg
        //Vec2 p = Vec2(-1-dim.bottom_left.x + 1 - fabs(dim.top_right.x-dim.bottom_left.x)/2.0, -1-dim.bottom_left.y + 1 - fabs(dim.top_right.y-dim.bottom_left.y)/2.0);
        ldVec2 p = ldVec2(-dim.bottom_left.x - dim.width()/2.0f, -dim.bottom_left.y - dim.height()/2.0f);
        res.translate(p);
    }

    //qDebug() << "EVERY minX:" << minX << " maxX:" << maxX << " minY:" << minY << " maxY:" << maxY;

    return res;
}

// svgDim
ldRect ldSvgReader::svgDim(const ldBezierPaths &bezierPaths)
{
    // if too few points
    if (ldSvgReader::totalPoints(bezierPaths) < MAX_NUMBER_POINTS_FOR_SVG_POSITIONING_FIX)
    {
        // interpolate curve
        return ldSvgReader::svgDimByInterpolation(bezierPaths);
    }

    ldRect res;
    bool init = false;
    for (const ldBezierPath &path : bezierPaths) {
        for (const ldBezierCurve &b : path.data()) {
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


ldRect3 ldSvgReader::svgDim(const ld3dBezierCurves &bezierCurves)
{
    // if too few points
    if (ldSvgReader::totalPoints(bezierCurves) < MAX_NUMBER_POINTS_FOR_SVG_POSITIONING_FIX)
    {
        // interpolate curve
        return ldSvgReader::svgDimByInterpolation(bezierCurves);
    }

    ldRect3 res;
    bool init = false;
    for (const std::vector<ld3dBezierCurve> &curve : bezierCurves) {
        for (const ld3dBezierCurve &b : curve) {
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
ldRect ldSvgReader::svgDimByInterpolation(const ldBezierPaths &bezierPaths, int p_interpolate)
{
    ldRect res;
    bool init = false;
    for (const ldBezierPath &path : bezierPaths) {
        for (const ldBezierCurve &b : path.data()) {
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
                ldVec2 p = b.getPoint(slope);

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


ldRect3 ldSvgReader::svgDimByInterpolation(const ld3dBezierCurves &bezierCurves, int p_interpolate)
{
    ldRect3 res;
    bool init = false;
    for (const std::vector<ld3dBezierCurve> &curve : bezierCurves) {
        for (const ld3dBezierCurve &b : curve) {
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
                ldVec3 p = b.getPoint(slope);

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
ldRect ldSvgReader::svgDimSequence(const ldBezierPathsSequence &bezierCurvesSequence)
{
    //qDebug()<<"svgDimSequence " ;

    ldRect res;
    res.bottom_left = ldVec2();
    res.top_right = ldVec2();
    bool init = false;
    //
    for (const ldBezierPaths &curves : bezierCurvesSequence) {
        if (!init) {
            res = ldSvgReader::svgDim(curves);
//            qDebug()<<"res_tmp .bottom_left.x "<< res.bottom_left.x ;
//            qDebug()<<"res_tmp .top_right "<< res.top_right.x ;
//
            init = true;
            continue;
        }
        ldRect res_tmp = ldSvgReader::svgDim(curves);


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

ldBezierPathsSequence ldSvgReader::loadFromJSArrayText(const char* text) {

    ldBezierPathsSequence curves;

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
        ldBezierPaths b;
        if (d.size() > maxstrokes) maxstrokes = d.size();
        for (size_t j = 0; j < d.size(); j++) {
            //qDebug() << "line " << j;
            tline l = d[j];
            if (l.size() == 4) {
                ldBezierCurve t;
                t.setStart(ldVec2(l[0].first, l[0].second));
                t.setControl1(ldVec2(l[1].first, l[1].second));
                t.setControl2(ldVec2(l[2].first, l[2].second));
                t.setEnd(ldVec2(l[3].first, l[3].second));
                ldBezierPath stroke;
                stroke.setColor(0xffffffff);
                stroke.add(t);
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
void ldSvgReader::debugStatCurves(ldBezierPaths bezierPaths)
{
    int pointCounter = 0;
    for (uint i=0; i< bezierPaths.size(); i++) {
        std::vector<ldBezierCurve> curve = bezierPaths[i].data();
        pointCounter += curve.size();
    }
    qDebug() << "nb curve: " << bezierPaths.size();
    qDebug() << "nb bezier points: " << pointCounter;
}

// snapCurves
ldBezierPaths ldSvgReader::snapCurves(const ldBezierPaths &bezierPaths, float snapDistance)
{
    ldBezierPaths res;
    //debugStatCurves(bezierCurves);
    ldBezierPath newOne;
    for (uint i=0; i<bezierPaths.size(); i++) {
        const ldBezierPath &path = bezierPaths[i];
        //
        for (uint c=0; c<path.size(); c++) {
            newOne.setColor(path.color());
            newOne.setGradient(path.gradient());
            const ldBezierCurve &a = path.data()[c];
            //qDebug() << "push_back a for c:" << c ;
            if (c == 0 && i>0) { // && i<bezierCurves.size()-1
                const std::vector<ldBezierCurve> &prevcurve = bezierPaths[i-1].data();
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
            newOne.add(a);
        }
        // last one
        if (i==bezierPaths.size()-1) {
            res.push_back(newOne);
        }
    }
    //qDebug() << "snapCurves" ;
    //debugStatCurves(res);
    return res;
}

QByteArray ldSvgReader::readFile(const QString &filePath)
{
    if(filePath.endsWith(ldSimpleCrypt::LDS_EXTENSION)) {
        if(QFile::exists(filePath)) {
            return ldSimpleCrypt::instance()->decrypt(filePath);
        } else {
            qWarning() << __FUNCTION__ << "file does not exist" << filePath;
            return QByteArray();
        }
    }

    QFile file(filePath);
    if (!file.exists()) {
        // check for secured file
        QString filePathSecured = filePath + ldSimpleCrypt::LDS_EXTENSION;
        if(QFile::exists(filePathSecured)) {
            return ldSimpleCrypt::instance()->decrypt(filePathSecured);
        }

        qWarning() << __FUNCTION__ << "file does not exist" << filePath;
        return QByteArray();
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << __FUNCTION__ << "Qt file issue";
        return QByteArray();
    }

    return file.readAll();
}
