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

#include "ldCore/Visualizations/util/BezierCurveHelper/ldBezierCurveObject.h"
#include "ldCore/Visualizations/util/MathsHelper/ldMaths.h"

enum SvgReadingType {

    // LD_SVG_READING_DEV
    // no resize, no data change -> developper mode: be sure of what you are doing
    LD_SVG_READING_DEV = 0,

    // LD_SVG_READING_MAXIMIZE
    // maximize to [-1,1]x[-1,1] regarding min and max values
    LD_SVG_READING_MAXIMIZE = 1,

    // LD_SVG_READING_SVGFRAME
    // keep the svg size as the space to resize to [-1,1]x[-1,1].
    // auto re-align if portrait/landscape
    // If a point is outside of the svg frame, LD_SVG_READING_MAXIMIZE mode is auto-activated
    LD_SVG_READING_SVGFRAME = 2,
};

#define max_number_points_for_svg_positionning_fix 200

class LDCORESHARED_EXPORT ldSvgReader
{
public:
    static int totalPoints(const svgBezierCurves &bezierCurves);
    static int totalPoints(const svgBezier3dCurves &bezierCurves);
    static void debugStatCurves(svgBezierCurves bezierCurves);
    static svgBezierCurves snapCurves(const svgBezierCurves &bezierCurves, float snapDistance);
    static svgBezierCurves loadSvg(QString qtfilename, SvgReadingType p_type = LD_SVG_READING_MAXIMIZE, float snapDistance = -1);
    static svgBezierCurves loadSvgFixedScale(float fixedMinX, float fixedMaxX, float fixedMinY, float fixedMaxY, char const* qtfilename, SvgReadingType p_type = LD_SVG_READING_MAXIMIZE, float snapDistance = -1);
    static svgBezierCurvesSequence loadSvgSequence(int size, int masksize, QString qtbasefilename, SvgReadingType p_type = LD_SVG_READING_MAXIMIZE, float snapDistance = -1);
    static svgBezierCurvesSequence loadSvgSequenceFixedScale(float fixedMinX, float fixedMaxX, float fixedMinY, float fixedMaxY, int size, int masksize, char const* qtbasefilename, SvgReadingType p_type = LD_SVG_READING_MAXIMIZE, float snapDistance = -1);
    static svgBezierCurvesSequence loadFromJSArrayText(const char* text);
    static SvgDim svgDim(const svgBezierCurves &bezierCurves);
    static Svg3dDim svgDim(const svgBezier3dCurves &bezierCurves);
    static SvgDim svgDimSequence(const svgBezierCurvesSequence &bezierCurves);
    static SvgDim svgDimByInterpolation(const svgBezierCurves &bezierCurves, int p_interpolate = 100);
    static Svg3dDim svgDimByInterpolation(const svgBezier3dCurves &bezierCurves, int p_interpolate = 100);
};

#endif // ldSvgReader_H

