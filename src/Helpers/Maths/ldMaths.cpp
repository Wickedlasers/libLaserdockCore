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

#include <cmath>

#include <QtCore/QDebug>
#include <QtCore/QTime>

double ldMaths::adjustToRange(double value, double sourceMin, double sourceMid, double sourceMax, double targetMin, double targetMid, double targetMax)
{
    double speedCoeff = 1.;

    if(value < sourceMid) {
        const double sourceSpeedRange = sourceMid - sourceMin;
        const double targetSpeedRange = targetMid - targetMin;
        speedCoeff = (value - sourceMin) * (targetSpeedRange / sourceSpeedRange) + targetMin;
    } else {
        const double sourceSpeedRange = sourceMax - sourceMid;
        const double targetSpeedRange = targetMax - targetMid;
        speedCoeff = (value - sourceMid) * (targetSpeedRange / sourceSpeedRange) + targetMid;
    }

    return speedCoeff;
}

float ldMaths::normalize(float value, float normalValue, float scale)
{
    float diff = value - normalValue;
    return normalValue + diff * scale;
}

float ldMaths::rndFloatBetween(float min, float max)
{
    float tmp;
    int precision=RAND_MAX;
    tmp = 1.0f * (rand() % precision);
    tmp *= 1.0f*(max-min)/(precision-1);
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
    if (cmpf(actual, min)) sign=1.0;
    else if (cmpf(actual,max)) sign=-1.0f;
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
        int xCount = static_cast<int> (ceilf(fabsf(x - max) / len));
        x -= xCount*len;
    } else if(x < min) {
        int xCount = static_cast<int> (ceilf(fabsf(x - min) / len));
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
        int xCount = static_cast<int> (ceil(fabs(x - max) / len));
        x -= xCount*len;
    } else if(x < min) {
        int xCount = static_cast<int> (ceil(fabs(x - min) / len));
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
    CCPoint res = CCPoint(m.x*cosf(M_2PIf*rotation)-m.y*sinf(M_2PIf*rotation),
                          m.x*sinf(M_2PIf*rotation)+m.y*cosf(M_2PIf*rotation));
    // translate
    res = CCPoint(res.x+translation.x, res.y+translation.y);
    //
    return res;
}

// dotProduct
float ldMaths::dotProduct(const ldVec3 &u, const ldVec3 &v)
{
    return u.x*v.x + u.y*v.y + u.z*v.z;
}


// distanceToPlan
float ldMaths::distanceToPlan(float a, float b, float c, float d, ldVec3 m)
{
    // where ax+by+cz+d=0 is the plan equation
    float L=a*a+b*b+c*c;
    if (cmpf(L,0.0)) return 0.0;
    return fabsf(a*m.x+b*m.y+c*m.z+d)/sqrtf(L);
}

float ldMaths::unitedToLaserCoords(float value)
{
    return 2.0f * value - 1.0f;
}

// unitedToLaserCoords
ldVec2 ldMaths::unitedToLaserCoords(const ldVec2 &p)
{
    ldVec2 r = ldVec2();
    r.x = unitedToLaserCoords(p.x);
    r.y = unitedToLaserCoords(p.y);
    return r;
}

float ldMaths::laserToUnitedCoords(float value)
{
    return 0.5f * (value + 1.0f);
}

// laserToUnitedCoords
ldVec2 ldMaths::laserToUnitedCoords(const ldVec2 &p)
{
    ldVec2 r = ldVec2();
    r.x = laserToUnitedCoords(p.x);
    r.y = laserToUnitedCoords(p.y);
    return r;
}

ldRect ldMaths::laserToUnitedCoords(const ldRect &dim)
{
    ldVec2 bottomLeft = ldMaths::laserToUnitedCoords(dim.bottom_left);
    ldVec2 topRight = ldMaths::laserToUnitedCoords(dim.top_right);
    return ldRect{bottomLeft, topRight};
}

// isValidLaserPoint
bool ldMaths::isValidLaserPoint(const ldVec2 &p)
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
float ldMaths::bezier3dLength(const ld3dBezierCurve &b, int maxPoints)
{
    float res = 0;
    for (int i=0; i<maxPoints-1; i++)
    {
        float slope_i = 1.0f*i/(maxPoints-1);
        float slope_ib = 1.0f*(i+1)/(maxPoints-1);
        ldVec3 p_i = b.getPoint(slope_i);
        ldVec3 p_ib = b.getPoint(slope_ib);

        res += sqrtf( (p_ib.x-p_i.x)*(p_ib.x-p_i.x) + (p_ib.y-p_i.y)*(p_ib.y-p_i.y) + (p_ib.z-p_i.z)*(p_ib.z-p_i.z) );
    }
    return res;
}

float ldMaths::bezier3dLengthFast(const ld3dBezierCurve &b)
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
    if (fabs(refValue) < 0.001f) {
        if (fabs(checkedValue) < 0.001f*percentAcceptable) return true;
        return false;
    }
    float percent = 100.f*fabsf(refValue - checkedValue)/fabsf(refValue);
    if (percent < percentAcceptable) return true;
    return false;
}

void ldMaths::translateSvgBezierCurves(ldBezierPaths &p_svgBC, const ldVec2 &p_t)
{
    for (ldBezierPath &path : p_svgBC) {
        path.translate(p_t);
    }
}

void ldMaths::rotateSvgBezierCurves(ldBezierPaths &p_svgBC, float rotateValue)
{
    for (ldBezierPath &path : p_svgBC) {
        path.rotate(rotateValue);
    }
}

// colorizeSvgBezierCurves
void ldMaths::colorizeSvgBezierCurves(ldBezierPaths &p_svgBC, uint32_t color)
{
    for (ldBezierPath &path : p_svgBC) {
        path.setColor(color);
    }
}

void ldMaths::scaleSvgBezierCurves(ldBezierPaths &p_svgBC, float p_s)
{
    for (ldBezierPath &path : p_svgBC) {
        path.scale(p_s);
    }
}

// svgBezierUnitedToLaserCoords
void ldMaths::svgBezierUnitedToLaserCoords(ldBezierPaths &p_svgData)
{
    for (ldBezierPath &path : p_svgData) {
        path.scale(2.f);
        path.translate(ldVec2::minus_one);
    }
}

void ldMaths::svgBezierLaserToUnitedCoords(ldBezierPaths &p_svgData)
{
    for (ldBezierPath &path : p_svgData) {
        path.translate(ldVec2::one);
        path.scale(0.5f);
    }
}

// svgBezierTo3dSvgBezierCurves
ld3dBezierCurves ldMaths::svgBezierTo3dSvgBezierCurves(const ldBezierPaths &p_svgData)
{
    ld3dBezierCurves res;
    //
    for (const ldBezierPath &path : p_svgData) {

        std::vector<ld3dBezierCurve> tmpVec3dCurve;

        for (const ldBezierCurve &bZ : path.data()) {
            ld3dBezierCurve b3Z;
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

            b3Z.color = path.color();

            tmpVec3dCurve.push_back(b3Z);
        }

        res.push_back(tmpVec3dCurve);
    }

    return res;
}

std::vector<std::vector<ldVec2>> ldMaths::svgBezierToPointLists(const ldBezierPaths &dataVect, float detail) {

    std::vector<std::vector<ldVec2>> pointcurves;
    if (dataVect.size() < 1) return pointcurves;


    {
        //
        for (const ldBezierPath &bezierPath : dataVect)
        {
            std::vector<ldVec2> pointcurve;

            for (const ldBezierCurve &b : bezierPath.data())
            {
                int maxPointsLocal = 10;
                float test = detail*b.length();
                if (test < 1.0f) test *= 2.0f;
                maxPointsLocal = static_cast<int>(test);
                if (maxPointsLocal<4) maxPointsLocal = 4;

                for (int i=0; i<maxPointsLocal; i++)
                {
                    float slope = 1.0f*i/(maxPointsLocal-1);
                    ldVec2 point = b.getPoint(slope);
                    pointcurve.push_back(point);
                }

            }
            pointcurves.push_back(pointcurve);
        }
    }

    return pointcurves;
}

// totalPointsIn3dSequence
int ldMaths::totalPointsIn3dSequence(const ld3dBezierCurvesSequence &sequence)
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

bool ldMaths::isPointInPolygon(const std::vector<ldVec2> &polygon, const ldVec2 &pointToCheck)
{
    bool contains = false;

    const ldVec2 *lastPoint = &polygon.back();

    for (const ldVec2 &currentPoint : polygon) {

        if ( ((currentPoint.y > pointToCheck.y) != (lastPoint->y > pointToCheck.y)) &&
             (pointToCheck.x < (lastPoint->x-currentPoint.x) * (pointToCheck.y-currentPoint.y) / (lastPoint->y-currentPoint.y) + currentPoint.x) ) {
            contains = !contains;
        }

        lastPoint = &currentPoint;
    }

    return contains;
}
