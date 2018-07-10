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

#include "ldCore/Shape/ldParticleGeometry.h"

#define M_PIf ((float) M_PI)
#define M_PI_2f ((float) M_PI_2)
#define M_PI_4f ((float) M_PI_4)
#define M_2PI (2.0*M_PI)
#define M_2PIf (float) (2.0*M_PI)

#ifdef _MSC_VER
// we want to use words far and near in our code
#undef far
#undef near
#endif

using namespace std;

LDCORESHARED_EXPORT bool cmpf(float a, float b, float epsilon = 0.005f);

//point2d
struct LDCORESHARED_EXPORT point2d {
    float x = 0.f;
    float y = 0.f;
};

//point3d
struct LDCORESHARED_EXPORT point3d {
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

    explicit point3d();
    explicit point3d(float p_x, float p_y, float p_z);
    bool isNull() const;

    point3d toLaserCoord() const;

    void rotate(float p_x, float p_y, float p_z, point3d p_pivot);
};

// animatedAngle
struct LDCORESHARED_EXPORT animatedAngle {
    float value;
    float decay;
    float sign;
};

//
class LDCORESHARED_EXPORT Vec2 {
public:
    static Vec2 zero;
    static Vec2 one;
    static Vec2 up;
    static Vec2 down;
    static Vec2 left;
    static Vec2 right;

    float x = 0.f;
    float y = 0.f;

    Vec2();
    Vec2(float p_x, float p_y);

    bool isNull() const;
    float magnitude() const;
    Vec2 normalize() const;
    float toRadians() const;

    Vec2 operator+ (const Vec2& other);
    Vec2 operator- (const Vec2& other);
    bool operator == (const Vec2& other);
    bool operator != (const Vec2& other);
};

/// SvgDim
/// Try to have the same API as QRectF. QRectF is based on qreal primitive but we need to use float
struct LDCORESHARED_EXPORT SvgDim {
    Vec2 bottom_left;
    Vec2 top_right;

    bool isNull() const;

    float width() const;
    float height() const;

    float bottom() const;
    float top() const;
    float left() const;
    float right() const;

    Vec2 center() const;

    Vec2 size() const;
};

/// SvgDim
/// Try to have the same API as QRectF. QRectF is based on qreal primitive but we need to use float
struct LDCORESHARED_EXPORT Svg3dDim {
    point3d bottom_left;
    point3d top_right;

    bool isNull() const;

    float width() const;
    float height() const;
    float depth() const;

    float bottom() const;
    float top() const;
    float left() const;
    float right() const;
    float far() const;
    float near() const;

    point3d center() const;
};

//
struct LDCORESHARED_EXPORT BezierCurve {
    Vec2 start;
    Vec2 end;
    Vec2 control1;
    Vec2 control2;
    uint32_t color = 0;

    void scale(float s);
    void scaleX(float s);
    void scaleY(float s);
    void rotate(float rotation);
    void translate(const Vec2 t);
    void lerp(BezierCurve b1, BezierCurve b2, float f);
};

//
struct LDCORESHARED_EXPORT Bezier3dCurve {
    point3d start;
    point3d end;
    point3d control1;
    point3d control2;
    uint32_t color = 0;
    point3d pivot;

    point3d cubicBezier(float slope) const;
};


// alias
typedef std::vector< std::vector<BezierCurve> > svgBezierCurves;
typedef std::vector< svgBezierCurves> svgBezierCurvesSequence;

typedef std::vector< std::vector<Bezier3dCurve> > svgBezier3dCurves;
typedef std::vector< svgBezier3dCurves> svgBezier3dCurvesSequence;

class LDCORESHARED_EXPORT ldMaths
{
public:
    static float rndFloatBetween(float min, float max);
    static float rndFloatBetweenWithInterval(float min, float max, float actualValue, float pourcentLimitLow, float pourcentLimitUp);
    static float rndSign();
    static float periodIntervalKeeper(float x, float min, float max);
    static int periodIntervalKeeperInt(int x, int min, int max);
    static float hypothenuse(float sideA, float sideB);
    static float normLog(float v, float power);
    static float normExp(float v, float power);
    static CCPoint changeCoords(const CCPoint &m, float rotation, const CCPoint &translation);
    static Vec2 changeCoordsVec2(const Vec2 &m, float rotation, const Vec2 &translation = Vec2());
    static Vec2 addVec2(const Vec2 &m, const Vec2 &n);
    static point3d rotate3d(const point3d &p, float angle, const point3d &axis);
    static point3d rotate3dAtPoint(const point3d &p, float angle, const point3d &axis, const point3d &point);
    static float distance3d(const point3d &m, const point3d &n);
    static point3d norm3d(const point3d &m);
    static point3d add3d(const point3d &m, const point3d &n);
    static point3d diff3d(const point3d &m, const point3d &n);
    static point3d mult3d(const point3d &m, float coef);
    static point3d vectProduct(const point3d &u, const point3d &v);
    static float dotProduct(const point3d &u, const point3d &v);
    static float distanceToPlan(float a, float b, float c, float d, point3d m);
    static point3d x_vector();
    static point3d y_vector();
    static point3d z_vector();
    static point3d x_neg_vector();
    static point3d y_neg_vector();
    static point3d z_neg_vector();
    static point3d zero_vector();
    static Vec2 unitedToLaserCoords(const Vec2 &p);
    static Vec2 laserToUnitedCoords(const Vec2 &p);
    static SvgDim laserToUnitedCoords(const SvgDim &dim);
    static bool isValidLaserPoint(const Vec2 &p);
    static float cubicBezier(float t, float start, float c1, float c2, float end);
    static float bezierLength(const BezierCurve &b, int maxPoints = 100);
    static float bezier3dLength(const Bezier3dCurve &b, int maxPoints = 100);
    static float bezier3dLengthFast(const Bezier3dCurve &b);
    static bool isValueNearFrom(float refValue, float checkedValue, float percentAcceptable);
    static svgBezierCurves translateSvgBezierCurves(const svgBezierCurves &p_svgBC, const Vec2 &p_t);
    static svgBezierCurves rotateSvgBezierCurves(const svgBezierCurves &p_svgBC, float rotateValue);
    static svgBezierCurves colorizeSvgBezierCurves(const svgBezierCurves &p_svgBC, uint32_t color);
    static svgBezierCurves scaleSvgBezierCurves(svgBezierCurves p_svgBC, float p_s);
    static svgBezierCurves svgBezierUnitedToLaserCoords(const svgBezierCurves &p_svgData);
    static svgBezierCurves svgBezierLaserToUnitedCoords(const svgBezierCurves &p_svgData);
    static svgBezier3dCurves svgBezierTo3dSvgBezierCurves(const svgBezierCurves &p_svgData);
    static std::vector<std::vector<point2d>> svgBezierToPointLists(const svgBezierCurves &p_svgData, float detail = 10.0f);
    static int totalPointsIn3dSequence(const svgBezier3dCurvesSequence &sequence);
    static float map(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp); //MEO

    static bool isPointInPolygon(const std::vector<Vec2> &polygon, const Vec2 &pointToCheck);

    static QTime timeFromMs(int millis);
};


#endif // LDMATHS_H


