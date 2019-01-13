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

// ldSvgReader.h
// Created by Eric Brugère on 10/12/16.
// Copyright (c) 2016 Wicked Lasers. All rights reserved.

#ifndef ldSvgReader_H
#define ldSvgReader_H

#include <vector>

#include "ldCore/Helpers/BezierCurve/ldBezierCurveObject.h"
#include "ldCore/Helpers/Maths/ldMaths.h"

class LDCORESHARED_EXPORT ldSvgReader
{
public:
    enum class Type {
        // no resize, no data change -> developper mode: be sure of what you are doing
        Dev,
        // maximize to [-1,1]x[-1,1] regarding min and max values
        Maximize,
        // keep the svg size as the space to resize to [-1,1]x[-1,1].
        // auto re-align if portrait/landscape
        // If a point is outside of the svg frame, ldSvgReader::Type::Maximize mode is auto-activated
        SvgFrame
    };

    static int totalPoints(const svgBezierCurves &bezierCurves);
    static int totalPoints(const svgBezier3dCurves &bezierCurves);
    static svgBezierCurves loadSvg(QString qtfilename, Type type = Type::Maximize, float snapDistance = -1, bool isTranslate = true);
    static svgBezierCurves loadSvgFixedScale(float fixedMinX, float fixedMaxX, float fixedMinY, float fixedMaxY, const QString &qtfilename, Type p_type = Type::Maximize, float snapDistance = -1);
    static svgBezierCurvesSequence loadSvgSequence(const QString &dirPath, Type p_type = Type::Maximize, float snapDistance = -1, const QString &filePrefix = "", int masksize = -1);
    static svgBezierCurvesSequence loadSvgSequenceFixedScale(float fixedMinX, float fixedMaxX, float fixedMinY, float fixedMaxY, int size, int masksize, char const* qtbasefilename, Type p_type = Type::Maximize, float snapDistance = -1);
    static svgBezierCurvesSequence loadFromJSArrayText(const char* text);
    static SvgDim svgDim(const svgBezierCurves &bezierCurves);
    static Svg3dDim svgDim(const svgBezier3dCurves &bezierCurves);
    static SvgDim svgDimSequence(const svgBezierCurvesSequence &bezierCurves);
    static SvgDim svgDimByInterpolation(const svgBezierCurves &bezierCurves, int p_interpolate = 100);
    static Svg3dDim svgDimByInterpolation(const svgBezier3dCurves &bezierCurves, int p_interpolate = 100);

private:
    static void debugStatCurves(svgBezierCurves bezierCurves);
    static svgBezierCurves snapCurves(const svgBezierCurves &bezierCurves, float snapDistance);
};

#endif // ldSvgReader_H

