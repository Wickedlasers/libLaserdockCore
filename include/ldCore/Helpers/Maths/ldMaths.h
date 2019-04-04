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

// ldMaths.h
//  Created by Eric Brugère on 4/12/16.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.

#ifndef LDMATHS_H
#define LDMATHS_H

#include <math.h>

#include "ldCore/Helpers/BezierCurve/ld3dBezierCurve.h"
#include "ldCore/Helpers/BezierCurve/ldBezierCurve.h"
#include "ldCore/Helpers/BezierCurve/ldBezierPath.h"
#include "ldCore/Shape/ldParticleGeometry.h"

#include "ldAnimatedAngle.h"
#include "ldRect.h"
#include "ldRect3.h"
#include "ldVec2.h"
#include "ldVec3.h"

const float M_PIf = static_cast<float> (M_PI);
const float M_PI_2f = static_cast<float> (M_PI_2);
const float M_PI_4f = static_cast<float> (M_PI_4);
const double M_2PI  = 2.0*M_PI;
const float M_2PIf = static_cast<float> (M_2PI);

LDCORESHARED_EXPORT bool cmpf(float a, float b, float epsilon = 0.005f);

class LDCORESHARED_EXPORT ldMaths
{
public:
    static float rndFloatBetween(float min, float max);
    static float rndFloatBetweenWithInterval(float min, float max, float actualValue, float pourcentLimitLow, float pourcentLimitUp);
    static float rndSign();
    static float sign(float p_value);
    static float periodIntervalKeeper(float x, float min, float max);
    static int periodIntervalKeeperInt(int x, int min, int max);
    static float hypothenuse(float sideA, float sideB);
    static float normLog(float v, float power);
    static float normExp(float v, float power);
    static CCPoint changeCoords(const CCPoint &m, float rotation, const CCPoint &translation);
    static float dotProduct(const ldVec3 &u, const ldVec3 &v);
    static float distanceToPlan(float a, float b, float c, float d, ldVec3 m);
    static ldVec2 unitedToLaserCoords(const ldVec2 &p);
    static ldVec2 laserToUnitedCoords(const ldVec2 &p);
    static ldRect laserToUnitedCoords(const ldRect &dim);
    static bool isValidLaserPoint(const ldVec2 &p);
    static float cubicBezier(float t, float start, float c1, float c2, float end);
    static float bezier3dLength(const ld3dBezierCurve &b, int maxPoints = 100);
    static float bezier3dLengthFast(const ld3dBezierCurve &b);
    static bool isValueNearFrom(float refValue, float checkedValue, float percentAcceptable);
    static void translateSvgBezierCurves(ldBezierPaths &p_svgBC, const ldVec2 &p_t);
    static void rotateSvgBezierCurves(ldBezierPaths &p_svgBC, float rotateValue);
    static void colorizeSvgBezierCurves(ldBezierPaths &p_svgBC, uint32_t color);
    static void scaleSvgBezierCurves(ldBezierPaths &p_svgBC, float p_s);
    static void svgBezierUnitedToLaserCoords(ldBezierPaths &p_svgData);
    static void svgBezierLaserToUnitedCoords(ldBezierPaths &p_svgData);
    static ld3dBezierCurves svgBezierTo3dSvgBezierCurves(const ldBezierPaths &p_svgData);
    static std::vector<std::vector<ldVec2>> svgBezierToPointLists(const ldBezierPaths &p_svgData, float detail = 10.0f);
    static int totalPointsIn3dSequence(const ld3dBezierCurvesSequence &sequence);
    static float map(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp); //MEO

    static bool isPointInPolygon(const std::vector<ldVec2> &polygon, const ldVec2 &pointToCheck);

    static QTime timeFromMs(int millis);
};


#endif // LDMATHS_H


