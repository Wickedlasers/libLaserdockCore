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

const float M_PIf = static_cast<float> (M_PI);
const float M_PI_2f = static_cast<float> (M_PI_2);
const float M_PI_4f = static_cast<float> (M_PI_4);
const double M_2PI  = 2.0*M_PI;
const float M_2PIf = static_cast<float> (M_2PI);

#ifdef _MSC_VER
// we want to use words far and near in our code
#undef far
#undef near
#endif

LDCORESHARED_EXPORT bool cmpf(float a, float b, float epsilon = 0.005f);

//point3d
struct LDCORESHARED_EXPORT point3d {
    const static point3d X_VECTOR;
    const static point3d Y_VECTOR;
    const static point3d Z_VECTOR;
    const static point3d X_NEG_VECTOR;
    const static point3d Y_NEG_VECTOR;
    const static point3d Z_NEG_VECTOR;
    const static point3d ZERO_VECTOR;

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

    explicit point3d();
    explicit point3d(float p_x, float p_y, float p_z);
    bool isNull() const;

    float distance(const point3d &n) const;
    void norm();

    point3d toLaserCoord() const;

    void rotate(float p_x, float p_y, float p_z, point3d p_pivot);

    point3d operator+ (const point3d& other) const;
    point3d& operator+= (const point3d& other);
    point3d operator- (const point3d& other) const;
    point3d& operator-= (const point3d& other);
    point3d operator* (float coef) const;
    point3d& operator*= (float coef);
    point3d operator/ (float coef) const;
    point3d& operator/= (float coef);
};

// animatedAngle
struct LDCORESHARED_EXPORT ldAnimatedAngle {
    float value;
    float decay;
    float sign;
};

//
class LDCORESHARED_EXPORT Vec2 {
public:
    const static Vec2 zero;
    const static Vec2 one;
    const static Vec2 up;
    const static Vec2 down;
    const static Vec2 left;
    const static Vec2 right;

    float x = 0.f;
    float y = 0.f;

    Vec2();
    Vec2(float p_x, float p_y);

    bool isNull() const;
    float distance(const Vec2 &n) const;
    float magnitude() const;
    Vec2 normalize() const;
    float toRadians() const;
    void rotate(float rotation); // (angle in radians)

    Vec2 operator+ (const Vec2& other) const;
    Vec2& operator+= (const Vec2& other);
    Vec2 operator- (const Vec2& other) const;
    Vec2& operator-= (const Vec2& other);
    Vec2& operator*= (float s);
    Vec2& operator*= (const Vec2& other);
    Vec2& operator/= (float s);
    Vec2& operator/= (const Vec2& other);
    bool operator == (const Vec2& other) const;
    bool operator != (const Vec2& other) const;
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
class LDCORESHARED_EXPORT ldBezierCurve {
public:
    static ldBezierCurve lerp(const ldBezierCurve &b1, const ldBezierCurve &b2, float f);

    Vec2 start() const;
    void setStart(const Vec2 &value);

    Vec2 end() const;
    void setEnd(const Vec2 &value);

    Vec2 control1() const;
    void setControl1(const Vec2 &value);

    Vec2 control2() const;
    void setControl2(const Vec2 &value);

    uint32_t color() const;
    void setColor(const uint32_t &value);

    float length(int maxPoints = 100) const;
    Vec2 getPoint(float slope) const;

    void scale(float s);
    void scaleX(float s);
    void scaleY(float s);
    void rotate(float rotation);
    void translate(const Vec2 &t);

private:
    Vec2 m_start;
    Vec2 m_end;
    Vec2 m_control1;
    Vec2 m_control2;
    uint32_t m_color = 0;

    mutable float m_length = -1.f;
};

//
struct LDCORESHARED_EXPORT Bezier3dCurve {
    point3d start;
    point3d end;
    point3d control1;
    point3d control2;
    uint32_t color = 0;
    point3d pivot;

    point3d getPoint(float slope) const;
};


// alias
typedef std::vector< std::vector<ldBezierCurve> > svgBezierCurves;
typedef std::vector< svgBezierCurves> svgBezierCurvesSequence;

typedef std::vector< std::vector<Bezier3dCurve> > svgBezier3dCurves;
typedef std::vector< svgBezier3dCurves> svgBezier3dCurvesSequence;

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
    static point3d rotate3d(const point3d &p, float angle, const point3d &axis);
    static point3d rotate3dAtPoint(const point3d &p, float angle, const point3d &axis, const point3d &point);
    static point3d vectProduct(const point3d &u, const point3d &v);
    static float dotProduct(const point3d &u, const point3d &v);
    static float distanceToPlan(float a, float b, float c, float d, point3d m);
    static Vec2 unitedToLaserCoords(const Vec2 &p);
    static Vec2 laserToUnitedCoords(const Vec2 &p);
    static SvgDim laserToUnitedCoords(const SvgDim &dim);
    static bool isValidLaserPoint(const Vec2 &p);
    static float cubicBezier(float t, float start, float c1, float c2, float end);
    static float bezier3dLength(const Bezier3dCurve &b, int maxPoints = 100);
    static float bezier3dLengthFast(const Bezier3dCurve &b);
    static bool isValueNearFrom(float refValue, float checkedValue, float percentAcceptable);
    static void translateSvgBezierCurves(svgBezierCurves &p_svgBC, const Vec2 &p_t);
    static void rotateSvgBezierCurves(svgBezierCurves &p_svgBC, float rotateValue);
    static void colorizeSvgBezierCurves(svgBezierCurves &p_svgBC, uint32_t color);
    static void scaleSvgBezierCurves(svgBezierCurves &p_svgBC, float p_s);
    static void svgBezierUnitedToLaserCoords(svgBezierCurves &p_svgData);
    static void svgBezierLaserToUnitedCoords(svgBezierCurves &p_svgData);
    static svgBezier3dCurves svgBezierTo3dSvgBezierCurves(const svgBezierCurves &p_svgData);
    static std::vector<std::vector<Vec2>> svgBezierToPointLists(const svgBezierCurves &p_svgData, float detail = 10.0f);
    static int totalPointsIn3dSequence(const svgBezier3dCurvesSequence &sequence);
    static float map(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp); //MEO

    static bool isPointInPolygon(const std::vector<Vec2> &polygon, const Vec2 &pointToCheck);

    static QTime timeFromMs(int millis);
};


#endif // LDMATHS_H


