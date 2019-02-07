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

#include "ldCore/Helpers/Maths/ldMaths.h"

#include <QtCore/QDebug>
#include <QtCore/QTime>

// -------------------------- Vec2 ---------------------------------

const Vec2 Vec2::zero = Vec2(0.0f, 0.0f);
const Vec2 Vec2::one = Vec2(1.0f, 1.0f);
const Vec2 Vec2::up = Vec2(0.0f, 1.0f);
const Vec2 Vec2::down = Vec2(0.0f, -1.0f);
const Vec2 Vec2::left = Vec2(-1.0f, 0.0f);
const Vec2 Vec2::right = Vec2(1.0f, 0.0f);

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
    return cmpf(x, 0.f) && cmpf(y, 0.f);
}

float Vec2::distance(const Vec2 &n) const
{
    return sqrtf(powf((x-n.x), 2) + powf((y-n.y), 2));
}

float Vec2::magnitude() const
{
    return sqrt(pow(x, 2) + pow(y, 2));
}

Vec2 Vec2::normalize() const
{
    Vec2 normalized;

    float m = magnitude();
    if (m > 0) {
        normalized = Vec2(x / m, y / m);
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

void Vec2::rotate(float rotation)
{
    float oldX = x;
    x = x*cosf(rotation)-y*sinf(rotation);
    y = oldX*sinf(rotation)+y*cosf(rotation);
}

Vec2 Vec2::operator+ (const Vec2& other) const
{
    Vec2 res = *this;
    res += other;
    return res;
}

Vec2 &Vec2::operator+=(const Vec2 &other)
{
    x += other.x;
    y += other.y;

    return *this;
}


Vec2 Vec2::operator- (const Vec2& other) const
{
    Vec2 res = *this;
    res -= other;
    return res;
}

Vec2& Vec2::operator-=(const Vec2 &other)
{
    x -= other.x;
    y -= other.y;

    return *this;
}

Vec2 &Vec2::operator*=(float s)
{
    x *= s;
    y *= s;
    return *this;
}

Vec2 &Vec2::operator*=(const Vec2 &other)
{
    x *= other.x;
    y *= other.y;
    return *this;
}

Vec2 &Vec2::operator/=(float s)
{
    x /= s;
    y /= s;
    return *this;
}

Vec2 &Vec2::operator/=(const Vec2 &other)
{
    x /= other.x;
    y /= other.y;
    return *this;
}

bool Vec2::operator ==(const Vec2 &other) const
{
    return cmpf(x, other.x) && cmpf(y, other.y);
}

bool Vec2::operator !=(const Vec2 &other) const
{
    return !(*this == other);
}

// -------------------------- point3d ---------------------------------


const point3d point3d::X_VECTOR = point3d(1.0f, 0.0f, 0.f);
const point3d point3d::Y_VECTOR = point3d(0.0f, 1.0f, 0.f);
const point3d point3d::Z_VECTOR = point3d(0.0f, 0.0f, 1.f);
const point3d point3d::X_NEG_VECTOR = point3d(-1.0f, 0.0f, 0.f);
const point3d point3d::Y_NEG_VECTOR = point3d(0.0f, -1.0f, 0.f);
const point3d point3d::Z_NEG_VECTOR = point3d(0.0f, 0.0f, -1.f);
const point3d point3d::ZERO_VECTOR = point3d(0.0f, 0.0f, 0.f);

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

float point3d::distance(const point3d &n) const
{
    return sqrtf(powf((x-n.x), 2) + powf((y-n.y), 2) + powf((z-n.z), 2));
}

void point3d::norm()
{
    float d = distance(ZERO_VECTOR);
    if(d == 0)
        return;

    *this /= d;
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
    r = ldMaths::rotate3dAtPoint(r, p_x, point3d::X_VECTOR, p_pivot);
    r = ldMaths::rotate3dAtPoint(r, p_y, point3d::Y_VECTOR, p_pivot);
    r = ldMaths::rotate3dAtPoint(r, p_z, point3d::Z_VECTOR, p_pivot);
    *this = r;
}

point3d point3d::operator+(const point3d &other) const
{
    point3d res = *this;
    res += other;
    return res;
}

point3d &point3d::operator+=(const point3d &other)
{
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
}

point3d point3d::operator-(const point3d &other) const
{
    point3d res = *this;
    res -= other;
    return res;
}

point3d &point3d::operator-=(const point3d &other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;
}

point3d point3d::operator*(float coef) const
{
    point3d res = *this;
    res *= coef;
    return res;
}

point3d &point3d::operator*=(float coef)
{
    x *= coef;
    y *= coef;
    z *= coef;

    return *this;
}

point3d point3d::operator/(float coef) const
{
    point3d res = *this;
    res /= coef;
    return res;
}

point3d &point3d::operator/=(float coef)
{
    x /= coef;
    y /= coef;
    z /= coef;

    return *this;
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

//sign
float ldMaths::sign(float p_value)
{
    return ( ( (p_value) < 0 )  ?  -1.0f : ( (p_value) > 0 ) );
}

// periodIntervalKeeper
float ldMaths::periodIntervalKeeper(float x, float min, float max)
{
    Q_ASSERT(min < max);

    float len = max - min;
    if(x > max) {
        int xCount = ceil(fabsf(x - max) / len);
        x -= xCount*len;
    } else if(x < min) {
        int xCount = ceil(fabsf(x - min) / len);
        x += xCount*len;
    }

    return x;
}

// periodIntervalKeeperInt
int ldMaths::periodIntervalKeeperInt(int x, int min, int max)
{
    Q_ASSERT(min < max);

    double len = max - min;
    if(x > max) {
        int xCount = ceil(fabs((double) x - max) / len);
        x -= xCount*len;
    } else if(x < min) {
        int xCount = ceil(fabs((double) x - min) / len);
        x += xCount*len;
    }

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
    point3d tmp = p - point;
    tmp = rotate3d(tmp, angle, axis);
    tmp += point;
    return tmp;
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

// bezier3dLength
float ldMaths::bezier3dLength(const Bezier3dCurve &b, int maxPoints)
{
    float res = 0;
    for (int i=0; i<maxPoints-1; i++)
    {
        float slope_i = 1.0f*i/(maxPoints-1);
        float slope_ib = 1.0f*(i+1)/(maxPoints-1);
        point3d p_i = b.getPoint(slope_i);
        point3d p_ib = b.getPoint(slope_ib);

        res += sqrtf( (p_ib.x-p_i.x)*(p_ib.x-p_i.x) + (p_ib.y-p_i.y)*(p_ib.y-p_i.y) + (p_ib.z-p_i.z)*(p_ib.z-p_i.z) );
    }
    return res;
}

float ldMaths::bezier3dLengthFast(const Bezier3dCurve &b)
{
    float chord = b.end.distance(b.start);

    float cont_net = b.start.distance(b.control1)
            + b.control2.distance(b.control1)
            + b.end.distance(b.control2);

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

void ldMaths::translateSvgBezierCurves(svgBezierCurves &p_svgBC, const Vec2 &p_t)
{
    for (std::vector<ldBezierCurve> &curve : p_svgBC) {
        for (ldBezierCurve &bZ : curve) {
            bZ.translate(p_t);
        }
    }
}

void ldMaths::rotateSvgBezierCurves(svgBezierCurves &p_svgBC, float rotateValue)
{
    for (std::vector<ldBezierCurve> &curve : p_svgBC) {
        for (ldBezierCurve &bZ : curve) {
            bZ.rotate(rotateValue);
        }
    }
}

// colorizeSvgBezierCurves
void ldMaths::colorizeSvgBezierCurves(svgBezierCurves &p_svgBC, uint32_t color)
{
    for (std::vector<ldBezierCurve> &curves : p_svgBC) {
        for (ldBezierCurve &curve : curves) {
            curve.setColor(color);
        }
    }
}

void ldMaths::scaleSvgBezierCurves(svgBezierCurves &p_svgBC, float p_s)
{
    for (std::vector<ldBezierCurve> &curve : p_svgBC) {
        for (ldBezierCurve &bZ : curve) {
            bZ.scale(p_s);
        }
    }
}

// svgBezierUnitedToLaserCoords
void ldMaths::svgBezierUnitedToLaserCoords(svgBezierCurves &p_svgData)
{
    for (std::vector<ldBezierCurve> &curve : p_svgData) {
        for (ldBezierCurve &bZ : curve) {
            bZ.scale(2.0f);
            bZ.translate(Vec2(-1,-1));
        }
    }
}

void ldMaths::svgBezierLaserToUnitedCoords(svgBezierCurves &p_svgData)
{
    for (std::vector<ldBezierCurve> &curve : p_svgData) {
        for (ldBezierCurve &bZ : curve) {
            bZ.translate(Vec2(1,1));
            bZ.scale(0.5f);
        }
    }
}

// svgBezierTo3dSvgBezierCurves
svgBezier3dCurves ldMaths::svgBezierTo3dSvgBezierCurves(const svgBezierCurves &p_svgData)
{
    svgBezier3dCurves res;
    //
    for (const std::vector<ldBezierCurve> &curve : p_svgData) {

        std::vector<Bezier3dCurve> tmpVec3dCurve;

        for (const ldBezierCurve &bZ : curve) {
            Bezier3dCurve b3Z;
            //
            b3Z.start.x = bZ.start().x;
            b3Z.start.y = bZ.start().y;
            b3Z.start.z = 0;

            //
            b3Z.end.x = bZ.end().x;
            b3Z.end.y = bZ.end().y;
            b3Z.end.z = 0;

            //
            b3Z.control1.x = bZ.control1().x;
            b3Z.control1.y = bZ.control1().y;
            b3Z.control1.z = 0;

            //
            b3Z.control2.x = bZ.control2().x;
            b3Z.control2.y = bZ.control2().y;
            b3Z.control2.z = 0;

            b3Z.color = bZ.color();

            tmpVec3dCurve.push_back(b3Z);
        }

        res.push_back(tmpVec3dCurve);
    }

    return res;
}

std::vector<std::vector<Vec2>> ldMaths::svgBezierToPointLists(const svgBezierCurves &dataVect, float detail) {

    std::vector<std::vector<Vec2>> pointcurves;
    if (dataVect.size() < 1) return pointcurves;


    {
        //
        for (const std::vector<ldBezierCurve> &bezierTab : dataVect)
        {
            std::vector<Vec2> pointcurve;

            for (const ldBezierCurve &b : bezierTab)
            {
                int maxPointsLocal = 10;
                float test = detail*b.length();
                if (test < 1.0) test *= 2.0;
                maxPointsLocal = (int) (test);
                if (maxPointsLocal<4) maxPointsLocal = 4;

                for (int i=0; i<maxPointsLocal; i++)
                {
                    float slope = 1.0*i/(maxPointsLocal-1);
                    Vec2 point = b.getPoint(slope);
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

point3d Bezier3dCurve::getPoint(float slope) const
{
    point3d r;
    r.x = ldMaths::cubicBezier(slope, start.x, control1.x, control2.x, end.x);
    r.y = ldMaths::cubicBezier(slope, start.y, control1.y, control2.y, end.y);
    r.z = ldMaths::cubicBezier(slope, start.z, control1.z, control2.z, end.z);
    return r;
}

Vec2 ldBezierCurve::start() const
{
    return m_start;
}

void ldBezierCurve::setStart(const Vec2 &value)
{
    m_start = value;

    m_length = -1.f;
}

Vec2 ldBezierCurve::end() const
{
    return m_end;
}

void ldBezierCurve::setEnd(const Vec2 &value)
{
    m_end = value;

    m_length = -1.f;
}

Vec2 ldBezierCurve::control1() const
{
    return m_control1;
}

void ldBezierCurve::setControl1(const Vec2 &value)
{
    m_control1 = value;

    m_length = -1.f;
}

Vec2 ldBezierCurve::control2() const
{
    return m_control2;
}

void ldBezierCurve::setControl2(const Vec2 &value)
{
    m_control2 = value;

    m_length = -1.f;
}

uint32_t ldBezierCurve::color() const
{
    return m_color;
}

void ldBezierCurve::setColor(const uint32_t &value)
{
    m_color = value;
}

ldBezierCurve ldBezierCurve::lerp(const ldBezierCurve &b1, const ldBezierCurve &b2, float f) {
    ldBezierCurve res;
    if (f < 0) f = 0;
    if (f > 1) f = 1;
    float g = 1-f;
    res.m_start.x = g*b1.m_start.x + f*b2.m_start.x;
    res.m_start.y = g*b1.m_start.y + f*b2.m_start.y;
    res.m_end.x = g*b1.m_end.x + f*b2.m_end.x;
    res.m_end.y = g*b1.m_end.y + f*b2.m_end.y;
    res.m_control1.x = g*b1.m_control1.x + f*b2.m_control1.x;
    res.m_control1.y = g*b1.m_control1.y + f*b2.m_control1.y;
    res.m_control2.x = g*b1.m_control2.x + f*b2.m_control2.x;
    res.m_control2.y = g*b1.m_control2.y + f*b2.m_control2.y;

    return res;
}

float ldBezierCurve::length(int maxPoints) const
{
    if(cmpf(m_length, -1)) {
        m_length = 0;
        for (int i=0; i<maxPoints-1; i++) {
            float slope_i = 1.0f*i/(maxPoints-1);
            float slope_ib = 1.0f*(i+1)/(maxPoints-1);
            Vec2 p_i = getPoint(slope_i);
            Vec2 p_ib = getPoint(slope_ib);
            m_length += sqrtf( (p_ib.x-p_i.x)*(p_ib.x-p_i.x) + (p_ib.y-p_i.y)*(p_ib.y-p_i.y) );
        }
    }
    return m_length;
}

Vec2 ldBezierCurve::getPoint(float slope) const
{
    Vec2 res;
    res.x = ldMaths::cubicBezier(slope, m_start.x, m_control1.x, m_control2.x, m_end.x);
    res.y = ldMaths::cubicBezier(slope, m_start.y, m_control1.y, m_control2.y, m_end.y);
    return res;
}

void ldBezierCurve::scale(float s)
{
    scaleX(s);
    scaleY(s);
}

void ldBezierCurve::scaleX(float s)
{
    m_start.x= s*m_start.x;
    m_end.x = s*m_end.x;
    m_control1.x = s*m_control1.x;
    m_control2.x = s*m_control2.x;

    m_length = -1.f;
}

void ldBezierCurve::scaleY(float s)
{
    m_start.y = s*m_start.y;
    m_end.y = s*m_end.y;
    m_control1.y = s*m_control1.y;
    m_control2.y = s*m_control2.y;

    m_length = -1.f;
}

void ldBezierCurve::rotate(float rotation)
{
    m_start.rotate(rotation);
    m_end.rotate(rotation);
    m_control1.rotate(rotation);
    m_control2.rotate(rotation);
}

void ldBezierCurve::translate(const Vec2 &t)
{
    m_start += t;
    m_end += t;
    m_control1 += t;
    m_control2 += t;
}


bool cmpf(float a, float b, float epsilon)
{
    return (fabsf(a - b) < epsilon);
}
