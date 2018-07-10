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

//
//  ldParticleGeometry.cpp
//  Laserdock
//
//  Created by rock on 10/18/13.
//  Copyright (c) 2013 Jake Huang. All rights reserved.
//

#include "ldCore/Shape/ldParticleGeometry.h"
#include "math.h"

CCPoint::CCPoint(void)
{
    setPoint(0.0f, 0.0f);
}

CCPoint::CCPoint(float xArg, float yArg)
{
    setPoint(xArg, yArg);
}

CCPoint::CCPoint(const CCPoint& other)
{
    setPoint(other.x, other.y);
}

CCPoint& CCPoint::operator= (const CCPoint& other)
{
    setPoint(other.x, other.y);
    return *this;
}

void CCPoint::setPoint(float xArg, float yArg)
{
    this->x = xArg;
    this->y = yArg;
}

bool CCPoint::equals(const CCPoint& target) const
{
    return ((x == target.x) && (y == target.y));
}


// implementation of CCSize

CCSize::CCSize(void)
{
    setSize(0.0f, 0.0f);
}

CCSize::CCSize(float widthArg, float heightArg)
{
    setSize(widthArg, heightArg);
}

CCSize::CCSize(const CCSize& other)
{
    setSize(other.width, other.height);
}

CCSize& CCSize::operator= (const CCSize& other)
{
    setSize(other.width, other.height);
    return *this;
}

void CCSize::setSize(float widthArg, float heightArg)
{
    this->width = widthArg;
    this->height = heightArg;
}

bool CCSize::equals(const CCSize& target) const
{
    return ((width == target.width) && (height == target.height));
}



float
ccpLength(const CCPoint& v)
{
    return sqrtf(ccpLengthSQ(v));
}

float
ccpDistance(const CCPoint& v1, const CCPoint& v2)
{
    return ccpLength(ccpSub(v1, v2));
}

CCPoint
ccpNormalize(const CCPoint& v)
{
    return ccpMult(v, 1.0f/ccpLength(v));
}

CCPoint
ccpForAngle(const float a)
{
    return ccp(cosf(a), sinf(a));
}

float
ccpToAngle(const CCPoint& v)
{
    return atan2f(v.y, v.x);
}

CCPoint ccpLerp(const CCPoint& a, const CCPoint& b, float alpha)
{
    return ccpAdd(ccpMult(a, 1.f - alpha), ccpMult(b, alpha));
}

float clampf(float value, float min_inclusive, float max_inclusive)
{
    if (min_inclusive > max_inclusive) {
        CC_SWAP(min_inclusive, max_inclusive, float);
    }
    return value < min_inclusive ? min_inclusive : value < max_inclusive? value : max_inclusive;
}

CCPoint ccpClamp(const CCPoint& p, const CCPoint& min_inclusive, const CCPoint& max_inclusive)
{
    return ccp(clampf(p.x,min_inclusive.x,max_inclusive.x), clampf(p.y, min_inclusive.y, max_inclusive.y));
}

CCPoint ccpFromSize(const CCSize& s)
{
    return ccp(s.width, s.height);
}

CCPoint ccpCompOp(const CCPoint& p, float (*opFunc)(float))
{
    return ccp(opFunc(p.x), opFunc(p.y));
}

bool ccpFuzzyEqual(const CCPoint& a, const CCPoint& b, float var)
{
    if(a.x - var <= b.x && b.x <= a.x + var)
        if(a.y - var <= b.y && b.y <= a.y + var)
            return true;
    return false;
}

CCPoint ccpCompMult(const CCPoint& a, const CCPoint& b)
{
    return ccp(a.x * b.x, a.y * b.y);
}

float ccpAngleSigned(const CCPoint& a, const CCPoint& b)
{
    CCPoint a2 = ccpNormalize(a);
    CCPoint b2 = ccpNormalize(b);
    float angle = atan2f(a2.x * b2.y - a2.y * b2.x, ccpDot(a2, b2));
    if( fabs(angle) < kCCPointEpsilon ) return 0.f;
    return angle;
}

CCPoint ccpRotateByAngle(const CCPoint& v, const CCPoint& pivot, float angle)
{
    CCPoint r = ccpSub(v, pivot);
    float cosa = cosf(angle), sina = sinf(angle);
    float t = r.x;
    r.x = t*cosa - r.y*sina + pivot.x;
    r.y = t*sina + r.y*cosa + pivot.y;
    return r;
}


bool ccpSegmentIntersect(const CCPoint& A, const CCPoint& B, const CCPoint& C, const CCPoint& D)
{
    float S, T;
    
    if( ccpLineIntersect(A, B, C, D, &S, &T )
       && (S >= 0.0f && S <= 1.0f && T >= 0.0f && T <= 1.0f) )
        return true;
    
    return false;
}

CCPoint ccpIntersectPoint(const CCPoint& A, const CCPoint& B, const CCPoint& C, const CCPoint& D)
{
    float S, T;
    
    if( ccpLineIntersect(A, B, C, D, &S, &T) )
    {
        // Point of intersection
        CCPoint P;
        P.x = A.x + S * (B.x - A.x);
        P.y = A.y + S * (B.y - A.y);
        return P;
    }
    
    return CCPointZero;
}

bool ccpLineIntersect(const CCPoint& A, const CCPoint& B,
                      const CCPoint& C, const CCPoint& D,
                      float *S, float *T)
{
    // FAIL: Line undefined
    if ( (A.x==B.x && A.y==B.y) || (C.x==D.x && C.y==D.y) )
    {
        return false;
    }
    const float BAx = B.x - A.x;
    const float BAy = B.y - A.y;
    const float DCx = D.x - C.x;
    const float DCy = D.y - C.y;
    const float ACx = A.x - C.x;
    const float ACy = A.y - C.y;
    
    const float denom = DCy*BAx - DCx*BAy;
    
    *S = DCx*ACy - DCy*ACx;
    *T = BAx*ACy - BAy*ACx;
    
    if (denom == 0)
    {
        if (*S == 0 || *T == 0)
        {
            // Lines incident
            return true;
        }
        // Lines parallel and not incident
        return false;
    }
    
    *S = *S / denom;
    *T = *T / denom;
    
    // Point of intersection
    // CGPoint P;
    // P.x = A.x + *S * (B.x - A.x);
    // P.y = A.y + *S * (B.y - A.y);
    
    return true;
}

float ccpAngle(const CCPoint& a, const CCPoint& b)
{
    float angle = acosf(ccpDot(ccpNormalize(a), ccpNormalize(b)));
    if( fabs(angle) < kCCPointEpsilon ) return 0.f;
    return angle;
}
