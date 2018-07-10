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

// ldMaths.cpp
//  Created by Eric Brugère on 4/12/16.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.

#include "ldMaths.h"
#include <QtCore/QDebug>
#include <QtCore/QTime>

// -------------------------- Vec2 ---------------------------------

Vec2 Vec2::zero = Vec2(0.0f, 0.0f);
Vec2 Vec2::one = Vec2(1.0f, 1.0f);
Vec2 Vec2::up = Vec2(0.0f, 1.0f);
Vec2 Vec2::down = Vec2(0.0f, -1.0f);
Vec2 Vec2::left = Vec2(-1.0f, 0.0f);
Vec2 Vec2::right = Vec2(1.0f, 0.0f);

Vec2::Vec2()
{
}

Vec2::Vec2(float p_x, float p_y)
    : x(p_x)
    , y(p_y)
{
}

bool Vec2::isNull() const
{
    return x == 0.f && y == 0.f;
}

float Vec2::magnitude() const
{
    return sqrt(pow(x, 2) + pow(y, 2));
}

Vec2 Vec2::normalize() const
{
    Vec2 normalized;

    if (magnitude() > 0) {
        normalized = Vec2(x / magnitude(), y / magnitude());
    }

    return normalized;
}

float Vec2::toRadians() const
{
    Vec2 normalized = normalize();
    float angle = atan2(normalized.y, normalized.x);
    angle = (normalized.x > 0 ? angle : 2 * M_PI + angle);

    return angle;
}

Vec2 Vec2::operator+ (const Vec2& other)
{
    return Vec2(x + other.x, y + other.y);
}

Vec2 Vec2::operator- (const Vec2& other)
{
    return Vec2(x - other.x, y - other.y);
}

bool Vec2::operator ==(const Vec2 &other)
{
    return cmpf(x, other.x) && cmpf(y, other.y);
}

bool Vec2::operator !=(const Vec2 &other)
{
    return !(*this == other);
}

// -------------------------- point3d ---------------------------------


point3d::point3d()
{
}

point3d::point3d(float p_x, float p_y, float p_z)
    : x(p_x)
    , y(p_y)
    , z(p_z)
{

}

bool point3d::isNull() const
{
    return x == 0.f && y == 0.f && z == 0.f;
}

point3d point3d::toLaserCoord() const
{
    point3d result;
    result.x = 2.0f * x - 1.0f;
    result.y = 2.0f * y - 1.0f;
//    result.z = 2.0f * z - 1.0f;
    return result;
}

void point3d::rotate(float p_x, float p_y, float p_z, point3d p_pivot)
{
    point3d r = *this;
    r = ldMaths::rotate3dAtPoint(r, p_x, ldMaths::x_vector(), p_pivot);
    r = ldMaths::rotate3dAtPoint(r, p_y, ldMaths::y_vector(), p_pivot);
    r = ldMaths::rotate3dAtPoint(r, p_z, ldMaths::z_vector(), p_pivot);
    *this = r;
}

// -------------------------- SvgDim ---------------------------------


bool SvgDim::isNull() const
{
    return bottom_left.isNull() && top_right.isNull();
}

float SvgDim::width() const
{
    return fabsf(bottom_left.x - top_right.x);
}

float SvgDim::height() const
{
    return fabsf(bottom_left.y - top_right.y);
}

float SvgDim::bottom() const
{
    return std::min(bottom_left.y, top_right.y);
}

float SvgDim::top() const
{
    return std::max(bottom_left.y, top_right.y);
}

float SvgDim::left() const
{
    return std::min(bottom_left.x, top_right.x);
}

float SvgDim::right() const
{
    return std::max(bottom_left.x, top_right.x);
}

Vec2 SvgDim::center() const
{
    return Vec2(left() + width()/2.f, bottom() + height()/2.f);
}

Vec2 SvgDim::size() const
{
    return Vec2(width(), height());
}


// -------------------------- Svg3dDim ---------------------------------

bool Svg3dDim::isNull() const
{
    return bottom_left.isNull() && top_right.isNull();
}

float Svg3dDim::width() const
{
    return fabsf(bottom_left.x - top_right.x);
}

float Svg3dDim::height() const
{
    return fabsf(bottom_left.y - top_right.y);
}

float Svg3dDim::depth() const
{
    return fabsf(bottom_left.z - top_right.z);
}

float Svg3dDim::bottom() const
{
    return std::min(bottom_left.y, top_right.y);
}

float Svg3dDim::top() const
{
    return std::max(bottom_left.y, top_right.y);
}

float Svg3dDim::left() const
{
    return std::min(bottom_left.x, top_right.x);
}

float Svg3dDim::right() const
{
    return std::max(bottom_left.x, top_right.x);
}

float Svg3dDim::far() const
{
    return std::max(bottom_left.z, top_right.z);
}

float Svg3dDim::near() const
{
    return std::min(bottom_left.z, top_right.z);
}

point3d Svg3dDim::center() const
{
    return point3d(left() + width()/2.f, bottom() + height()/2.f, near() + depth()/2.f);
}


// -------------------------- ldMaths ---------------------------------

// rndFloatBetween
float ldMaths::rndFloatBetween(float min, float max)
{
    float tmp;
    int precision=RAND_MAX;
    tmp = 1.0f * (rand() % precision);
    tmp *= 1.0*(max-min)/(precision-1);
    tmp += min;
    tmp = clampf(tmp, min, max);
    return tmp;
}

// rndFloatBetweenWithInterval
float ldMaths::rndFloatBetweenWithInterval(float min, float max, float actual, float pourcentVarLow, float pourcentVarHigh)
{

    //if (actual==min) return max;
    //else return min;

    float minVar=(max-min)*pourcentVarLow/100.0f;
    float maxVar=(max-min)*pourcentVarHigh/100.0f;
    float var=rndFloatBetween(minVar, maxVar);
    float sign;
    if (actual==min) sign=1.0;
    else if (actual==max) sign=-1.0f;
    else sign=rndSign();
    float res=actual+sign*var;
    //qDebug() << res;
    /*
     float diff=fabs(newValue-actualValue);
     float sign=fabs(newValue-actualValue);
     if (diff>minVar)
     float minDiff=actual*pourcentLimitLow/100.0;
     float maxDiff=actual*pourcentLimitUp/100.0;
     if (fabs(newValue-actualValue)>maxDiff) {
     if (newValue<actualValue) res=actualValue-maxDiff;
     else res=actualValue+maxDiff;
     } else if (fabs(newValue-actualValue)<minDiff) {
     if (newValue<actualValue) res=actualValue-minDiff;
     else res=actualValue+minDiff;
     }
     //qDebug() << "minDiff" << minDiff << "maxDiff" << maxDiff << "res" << res;
     */
    res = clampf(res, min, max);
    //qDebug() << " min" << min << "max"<< max << "actual"<< actual << "minVar" << minVar << "maxVar" << maxVar << "res" << res;
    return res;
}

// rndSign
float ldMaths::rndSign()
{
    int rnd=(rand() % 2);
    if (rnd) return 1.0;
    return -1.0f;
}

// periodIntervalKeeper
float ldMaths::periodIntervalKeeper(float x, float min, float max)
{
    //
    float len=max-min;
    while (x>max) x=x-len;
    while (x<min) x=x+len;
    return x;
}

// periodIntervalKeeperInt
int ldMaths::periodIntervalKeeperInt(int x, int min, int max)
{
    //
    int len=max-min;
    while (x>max) x=x-len;
    while (x<min) x=x+len;
    return x;
}


// normLog
float ldMaths::normLog(float v, float power)
{
    v = logf(v + 1)/logf(2);
    return powf(v, power);
}

// normExp
float ldMaths::normExp(float v, float power)
{
    v = (expf(v) - expf(0)) / (expf(1) - expf(0));
    return powf(v, power);
}

// hypothenuse
float ldMaths::hypothenuse(float sideA, float sideB)
{
    return sqrtf(sideA*sideA+sideB*sideB);
}

// changeCoords (angle in radians/M_2PI)
CCPoint ldMaths::changeCoords(const CCPoint &m, float rotation, const CCPoint &translation)
{
    // rotate
    // m = ccp(m.x*cosf(2.0*M_PI*rotation)+m.y*sinf(2.0*M_PI*rotation), m.x*sinf(2.0*M_PI*rotation)-m.y*cosf(2.0*M_PI*rotation));
    CCPoint res = CCPoint(m.x*cosf(M_2PI*rotation)-m.y*sinf(M_2PI*rotation),
                          m.x*sinf(M_2PI*rotation)+m.y*cosf(M_2PI*rotation));
    // translate
    res = CCPoint(res.x+translation.x, res.y+translation.y);
    //
    return res;
}

// changeCoordsVec2 (angle in radians)
Vec2 ldMaths::changeCoordsVec2(const Vec2 &m, float rotation, const Vec2 &translation)
{
    Vec2 r = Vec2();
    // rotate
    r.x = m.x*cosf(rotation)-m.y*sinf(rotation);
    r.y = m.x*sinf(rotation)+m.y*cosf(rotation);
    // translate
    r.x += translation.x;
    r.y += translation.y;
    //
    return r;
}

//  addVec2
Vec2 ldMaths::addVec2(const Vec2 &m, const Vec2 &n)
{
    return Vec2(m.x+n.x, m.y+n.y);
}

// rotate3d
point3d ldMaths::rotate3d(const point3d &p, float angle, const point3d &axis)
{
    //
    point3d output;
    output.x=p.x;
    output.y=p.y;
    output.z=p.z;

    //
    angle = periodIntervalKeeper(angle, 0.0, M_2PIf);
    float rotationMatrix[3][3];
    float inputMatrix[3][1] = {{0.0}, {0.0}, {0.0}};
    float outputMatrix[3][1] = {{0.0}, {0.0}, {0.0}};
    outputMatrix[0][0] = 0.0;
    outputMatrix[1][0] = 0.0;
    outputMatrix[2][0] = 0.0;

    float u=axis.x;
    float v=axis.y;
    float w=axis.z;
    float u2 = u * u;
    float v2 = v * v;
    float w2 = w * w;
    float L = (u2 + v2 + w2);
    if (L<1.0e-4) return p;

    // rotationMatrix
    rotationMatrix[0][0] = (u2 + (v2 + w2) * cosf(angle)) / L;
    rotationMatrix[0][1] = (u * v * (1.0f - cosf(angle)) - w * sqrtf(L) * sinf(angle)) / L;
    rotationMatrix[0][2] = (u * w * (1.0f - cosf(angle)) + v * sqrtf(L) * sinf(angle)) / L;

    rotationMatrix[1][0] = (u * v * (1.0f - cosf(angle)) + w * sqrtf(L) * sinf(angle)) / L;
    rotationMatrix[1][1] = (v2 + (u2 + w2) * cosf(angle)) / L;
    rotationMatrix[1][2] = (v * w * (1.0f - cosf(angle)) - u * sqrtf(L) * sinf(angle)) / L;

    rotationMatrix[2][0] = (u * w * (1.0f - cosf(angle)) - v * sqrtf(L) * sinf(angle)) / L;
    rotationMatrix[2][1] = (v * w * (1.0f - cosf(angle)) + u * sqrtf(L) * sinf(angle)) / L;
    rotationMatrix[2][2] = (w2 + (u2 + v2) * cosf(angle)) / L;

    //
    inputMatrix[0][0] = p.x;
    inputMatrix[1][0] = p.y;
    inputMatrix[2][0] = p.z;

    // rotate
    for(int i = 0; i < 3; i++ )
    {
        for (int j = 0; j < 1; j++){
            outputMatrix[i][j] = 0;
            for (int k = 0; k < 3; k++)
            {
                outputMatrix[i][j] += rotationMatrix[i][k] * inputMatrix[k][j];
            }
        }
    }

    // output
    output.x=outputMatrix[0][0];
    output.y=outputMatrix[1][0];
    output.z=outputMatrix[2][0];
    return output;
}

// rotate3dAtPoint
point3d ldMaths::rotate3dAtPoint(const point3d &p, float angle, const point3d &axis, const point3d &point)
{
    point3d tmp=diff3d(p,point);
    tmp=rotate3d(tmp, angle, axis);
    tmp=add3d(tmp,point);
    return tmp;
}

// distance3d
float ldMaths::distance3d(const point3d &m, const point3d &n)
{
    return sqrtf( (m.x-n.x)*(m.x-n.x) + (m.y-n.y)*(m.y-n.y) + (m.z-n.z)*(m.z-n.z) );
}

// norm3d
point3d ldMaths::norm3d(const point3d &m)
{
    point3d zero;
    zero.x=0;
    zero.y=0;
    zero.z=0;
    float d=distance3d(m, zero);
    if (d==0) return zero;
    zero.x=m.x/d;
    zero.y=m.y/d;
    zero.z=m.z/d;
    return zero;
}

// add3d
point3d ldMaths::add3d(const point3d &m, const point3d &n)
{
    point3d res;
    res.x=n.x+m.x;
    res.y=n.y+m.y;
    res.z=n.z+m.z;
    return res;
}

// diff3d
point3d ldMaths::diff3d(const point3d &m, const point3d &n)
{
    point3d res;
    res.x=m.x-n.x;
    res.y=m.y-n.y;
    res.z=m.z-n.z;
    return res;
}

// mult3d
point3d ldMaths::mult3d(const point3d &m, float coef)
{
    point3d res;
    res.x=coef*m.x;
    res.y=coef*m.y;
    res.z=coef*m.z;
    return res;
}

// vectProduct
point3d ldMaths::vectProduct(const point3d &u, const point3d &v)
{
    point3d res;
    res.x=u.y*v.z-u.z*v.y;
    res.y=u.z*v.x-u.x*v.z;
    res.z=u.x*v.y-u.y*v.x;
    return res;
}

// dotProduct
float ldMaths::dotProduct(const point3d &u, const point3d &v)
{
    return u.x*v.x + u.y*v.y + u.z*v.z;
}


// distanceToPlan
float ldMaths::distanceToPlan(float a, float b, float c, float d, point3d m)
{
    // where ax+by+cz+d=0 is the plan equation
    float L=a*a+b*b+c*c;
    if (L==0.0) return 0.0;
    return fabsf(a*m.x+b*m.y+c*m.z+d)/sqrtf(L);
}

// x_vector
point3d ldMaths::x_vector()
{
    point3d v;
    v.x=1.0;
    v.y=0.0;
    v.z=0.0;
    return v;
}

// y_vector
point3d ldMaths::y_vector()
{
    point3d v;
    v.x=0.0;
    v.y=1.0;
    v.z=0.0;
    return v;
}

// z_vector
point3d ldMaths::z_vector()
{
    point3d v;
    v.x=0.0;
    v.y=0.0;
    v.z=1.0;
    return v;
}

// x_neg_vector
point3d ldMaths::x_neg_vector()
{
    point3d v;
    v.x=-1.0f;
    v.y=0.0;
    v.z=0.0;
    return v;
}

// y_neg_vector
point3d ldMaths::y_neg_vector()
{
    point3d v;
    v.x=0.0;
    v.y=-1.0f;
    v.z=0.0;
    return v;
}

// z_neg_vector
point3d ldMaths::z_neg_vector()
{
    point3d v;
    v.x=0.0;
    v.y=0.0;
    v.z=-1.0f;
    return v;
}

// zero_vector
point3d ldMaths::zero_vector()
{
    point3d v;
    v.x=0.0;
    v.y=0.0;
    v.z=0.0;
    return v;
}

// unitedToLaserCoords
Vec2 ldMaths::unitedToLaserCoords(const Vec2 &p)
{
    Vec2 r = Vec2();
    r.x = 2.0f * p.x - 1.0f;
    r.y = 2.0f * p.y - 1.0f;
    return r;
}

// laserToUnitedCoords
Vec2 ldMaths::laserToUnitedCoords(const Vec2 &p)
{
    Vec2 r = Vec2();
    r.x = 0.5f * (p.x + 1.0f);
    r.y = 0.5f * (p.y + 1.0f);
    return r;
}

// isValidLaserPoint
bool ldMaths::isValidLaserPoint(const Vec2 &p)
{
    float limit = 1.05f;
    if (p.x > limit || p.x < -limit) return false;
    if (p.y > limit || p.y < -limit) return false;
    return true;
}

// cubicBezier
float ldMaths::cubicBezier(float t, float start, float c1, float c2, float end)
{
    float t_ = (1.0f - t);
    float tt_ = t_ * t_;
    float ttt_ = tt_ * t_;
    float tt = t * t;
    float ttt = tt * t;
    return start * ttt_ + 3.0f *  c1 * tt_ * t + 3.0f *  c2 * t_ * tt + end * ttt;
}

// bezierLength
float ldMaths::bezierLength(const BezierCurve &b, int maxPoints)
{
    float res = 0;
    for (int i=0; i<maxPoints-1; i++)
    {
        float slope_i = 1.0f*i/(maxPoints-1);
        float slope_ib = 1.0f*(i+1)/(maxPoints-1);
        float xi = ldMaths::cubicBezier(slope_i, b.start.x, b.control1.x, b.control2.x, b.end.x);
        float yi = ldMaths::cubicBezier(slope_i, b.start.y, b.control1.y, b.control2.y, b.end.y);
        float xib = ldMaths::cubicBezier(slope_ib, b.start.x, b.control1.x, b.control2.x, b.end.x);
        float yib = ldMaths::cubicBezier(slope_ib, b.start.y, b.control1.y, b.control2.y, b.end.y);
        res += sqrtf( (xib-xi)*(xib-xi) + (yib-yi)*(yib-yi) );
    }
    return res;
}

// bezier3dLength
float ldMaths::bezier3dLength(const Bezier3dCurve &b, int maxPoints)
{
    float res = 0;
    for (int i=0; i<maxPoints-1; i++)
    {
        float slope_i = 1.0f*i/(maxPoints-1);
        float slope_ib = 1.0f*(i+1)/(maxPoints-1);
        float xi = ldMaths::cubicBezier(slope_i, b.start.x, b.control1.x, b.control2.x, b.end.x);
        float yi = ldMaths::cubicBezier(slope_i, b.start.y, b.control1.y, b.control2.y, b.end.y);
        float zi = ldMaths::cubicBezier(slope_i, b.start.z, b.control1.z, b.control2.z, b.end.z);
        float xib = ldMaths::cubicBezier(slope_ib, b.start.x, b.control1.x, b.control2.x, b.end.x);
        float yib = ldMaths::cubicBezier(slope_ib, b.start.y, b.control1.y, b.control2.y, b.end.y);
        float zib = ldMaths::cubicBezier(slope_ib, b.start.z, b.control1.z, b.control2.z, b.end.z);
        res += sqrtf( (xib-xi)*(xib-xi) + (yib-yi)*(yib-yi) + (zib-zi)*(zib-zi) );
    }
    return res;
}

float ldMaths::bezier3dLengthFast(const Bezier3dCurve &b)
{
    float chord = distance3d(b.end, b.start);

    float cont_net = distance3d(b.start, b.control1)
            + distance3d(b.control2, b.control1)
            + distance3d(b.end, b.control2);

    float app_arc_length = (cont_net + chord) / 2;
    return app_arc_length;
}

// isValueNearFrom
bool ldMaths::isValueNearFrom(float refValue, float checkedValue, float percentAcceptable)
{
    // some bad trick for value near zero
    if (fabs(refValue) < 0.001) {
        if (fabs(checkedValue) < 0.001f*percentAcceptable) return true;
        return false;
    }
    float percent = 100.f*fabsf(refValue - checkedValue)/fabsf(refValue);
    if (percent < percentAcceptable) return true;
    return false;
}

// translateSvgBezierCurves
svgBezierCurves ldMaths::translateSvgBezierCurves(const svgBezierCurves &p_svgBC, const Vec2 &p_t)
{
    svgBezierCurves res = p_svgBC;
    for (std::vector<BezierCurve> &curve : res) {
        for (BezierCurve &bZ : curve) {
            bZ.translate(p_t);
        }
    }
    return res;
}

// rotateSvgBezierCurves
svgBezierCurves ldMaths::rotateSvgBezierCurves(const svgBezierCurves &p_svgBC, float rotateValue)
{
    svgBezierCurves res = p_svgBC;
    for (std::vector<BezierCurve> &curve : res) {
        for (BezierCurve &bZ : curve) {
            bZ.rotate(rotateValue);
        }
    }
    return res;
}
// colorizeSvgBezierCurves
svgBezierCurves ldMaths::colorizeSvgBezierCurves(const svgBezierCurves &p_svgBC, uint32_t color)
{
    svgBezierCurves res = p_svgBC;
    for (std::vector<BezierCurve> &curves : res) {
        for (BezierCurve &curve : curves) {
            curve.color = color;
        }
    }
    return res;
}

// scaleSvgBezierCurves
svgBezierCurves ldMaths::scaleSvgBezierCurves(svgBezierCurves p_svgBC, float p_s)
{
    svgBezierCurves res = p_svgBC;
    //
    for (std::vector<BezierCurve> &curve : res) {
        for (BezierCurve &bZ : curve) {
            bZ.scale(p_s);
        }
    }
    return res;
}

// svgBezierUnitedToLaserCoords
svgBezierCurves ldMaths::svgBezierUnitedToLaserCoords(const svgBezierCurves &p_svgData)
{
    svgBezierCurves res = p_svgData;

    for (std::vector<BezierCurve> &curve : res) {
        for (BezierCurve &bZ : curve) {
            bZ.scale(2.0f);
            bZ.translate(Vec2(-1,-1));
        }
    }

    return res;
}

svgBezierCurves ldMaths::svgBezierLaserToUnitedCoords(const svgBezierCurves &p_svgData)
{
    svgBezierCurves res = p_svgData;

    for (std::vector<BezierCurve> &curve : res) {
        for (BezierCurve &bZ : curve) {
            bZ.translate(Vec2(1,1));
            bZ.scale(0.5f);
        }
    }

    return res;
}

// svgBezierTo3dSvgBezierCurves
svgBezier3dCurves ldMaths::svgBezierTo3dSvgBezierCurves(const svgBezierCurves &p_svgData)
{
    svgBezier3dCurves res;
    //
    for (const std::vector<BezierCurve> &curve : p_svgData) {

        std::vector<Bezier3dCurve> tmpVec3dCurve;

        for (const BezierCurve &bZ : curve) {
            Bezier3dCurve b3Z;
            //
            b3Z.start.x = bZ.start.x;
            b3Z.start.y = bZ.start.y;
            b3Z.start.z = 0;

            //
            b3Z.end.x = bZ.end.x;
            b3Z.end.y = bZ.end.y;
            b3Z.end.z = 0;

            //
            b3Z.control1.x = bZ.control1.x;
            b3Z.control1.y = bZ.control1.y;
            b3Z.control1.z = 0;

            //
            b3Z.control2.x = bZ.control2.x;
            b3Z.control2.y = bZ.control2.y;
            b3Z.control2.z = 0;

            b3Z.color = bZ.color;

            tmpVec3dCurve.push_back(b3Z);
        }

        res.push_back(tmpVec3dCurve);
    }

    return res;
}

std::vector<std::vector<point2d>> ldMaths::svgBezierToPointLists(const svgBezierCurves &dataVect, float detail) {

    std::vector<std::vector<point2d>> pointcurves;
    if (dataVect.size() < 1) return pointcurves;


    {
        //
        for (uint p=0; p<dataVect.size(); p++)
        {
            std::vector<point2d> pointcurve;
            std::vector<BezierCurve> bezierTab = dataVect.at(p);

            for (uint i=0; i<bezierTab.size(); i++)
            {
                point2d point;
                BezierCurve b = bezierTab.at(i);

                int maxPointsLocal = 10;
                float test = detail*ldMaths::bezierLength(b);
                if (test < 1.0) test *= 2.0;
                maxPointsLocal = (int) (test);
                if (maxPointsLocal<4) maxPointsLocal = 4;

                for (int j=0; j<maxPointsLocal; j++)
                {
                    float slope = 1.0*j/(maxPointsLocal-1);
                    point.x = ldMaths::cubicBezier(slope, b.start.x, b.control1.x, b.control2.x, b.end.x);
                    point.y = ldMaths::cubicBezier(slope, b.start.y, b.control1.y, b.control2.y, b.end.y);
                    pointcurve.push_back(point);
                }

            }
            pointcurves.push_back(pointcurve);
        }
    }

    return pointcurves;
}

// totalPointsIn3dSequence
int ldMaths::totalPointsIn3dSequence(const svgBezier3dCurvesSequence &sequence)
{
    int pointCounter = 0;
    for (auto _svgBezier3dCurves : sequence)
    {
        for (auto _bezier3dCurve : _svgBezier3dCurves)
        {
            pointCounter += _bezier3dCurve.size();
        }
    }
    return pointCounter;
}

QTime ldMaths::timeFromMs(int millis)
{
    int ms = millis % 1000;
    int sec = (millis / 1000) % 60;
    int min = (millis / 1000 / 60) % 60;
    int h = (millis / 1000 / 60 / 60) % 24;

    return QTime(h, min, sec, ms);;
}

SvgDim ldMaths::laserToUnitedCoords(const SvgDim &dim)
{
    Vec2 bottomLeft = ldMaths::laserToUnitedCoords(dim.bottom_left);
    Vec2 topRight = ldMaths::laserToUnitedCoords(dim.top_right);
    return SvgDim{bottomLeft, topRight};
}

//MEO 2016-03-14 - maps value from input range to output range
float ldMaths::map(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp) {
    if (fabs(inputMin - inputMax) < FLT_EPSILON){
        return outputMin;
    } else {
        float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
        if( clamp ){
            if(outputMax < outputMin){
                if( outVal < outputMax )outVal = outputMax;
                else if( outVal > outputMin )outVal = outputMin;
            }else{
                if( outVal > outputMax )outVal = outputMax;
                else if( outVal < outputMin )outVal = outputMin;
            }
        }
        return outVal;
    }
}

bool ldMaths::isPointInPolygon(const std::vector<Vec2> &polygon, const Vec2 &pointToCheck)
{
    bool contains = false;

    const Vec2 *lastPoint = &polygon.back();

    for (const Vec2 &currentPoint : polygon) {

        if ( ((currentPoint.y > pointToCheck.y) != (lastPoint->y > pointToCheck.y)) &&
             (pointToCheck.x < (lastPoint->x-currentPoint.x) * (pointToCheck.y-currentPoint.y) / (lastPoint->y-currentPoint.y) + currentPoint.x) ) {
            contains = !contains;
        }

        lastPoint = &currentPoint;
    }

    return contains;
}

point3d Bezier3dCurve::cubicBezier(float slope) const
{
    point3d r;
    r.x = ldMaths::cubicBezier(slope, start.x, control1.x, control2.x, end.x);
    r.y = ldMaths::cubicBezier(slope, start.y, control1.y, control2.y, end.y);
    r.z = ldMaths::cubicBezier(slope, start.z, control1.z, control2.z, end.z);
    return r;
}

void BezierCurve::scale(float s)
{
    scaleX(s);
    scaleY(s);
}

void BezierCurve::scaleX(float s)
{
    start.x= s*start.x;
    end.x = s*end.x;
    control1.x = s*control1.x;
    control2.x = s*control2.x;
}

void BezierCurve::scaleY(float s)
{
    start.y = s*start.y;
    end.y = s*end.y;
    control1.y = s*control1.y;
    control2.y = s*control2.y;
}

void BezierCurve::rotate(float rotation)
{
    start = ldMaths::changeCoordsVec2(start, rotation, Vec2());
    end = ldMaths::changeCoordsVec2(end, rotation, Vec2());
    control1 = ldMaths::changeCoordsVec2(control1, rotation, Vec2());
    control2 = ldMaths::changeCoordsVec2(control2, rotation, Vec2());
}

void BezierCurve::translate(const Vec2 t)
{
    start.x = start.x + t.x;
    start.y = start.y + t.y;
    end.x = end.x +  t.x;
    end.y = end.y + t.y;
    control1.x = control1.x + t.x;
    control1.y = control1.y + t.y;
    control2.x = control2.x + t.x;
    control2.y = control2.y + t.y;
}

void BezierCurve::lerp(BezierCurve b1, BezierCurve b2, float f) {
    if (f < 0) f = 0;
    if (f > 1) f = 1;
    float g = 1-f;
    start.x = g*b1.start.x + f*b2.start.x;
    start.y = g*b1.start.y + f*b2.start.y;
    end.x = g*b1.end.x + f*b2.end.x;
    end.y = g*b1.end.y + f*b2.end.y;
    control1.x = g*b1.control1.x + f*b2.control1.x;
    control1.y = g*b1.control1.y + f*b2.control1.y;
    control2.x = g*b1.control2.x + f*b2.control2.x;
    control2.y = g*b1.control2.y + f*b2.control2.y;
}

bool cmpf(float a, float b, float epsilon)
{
    return (fabsf(a - b) < epsilon);
}
