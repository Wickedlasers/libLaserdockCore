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

#include "ldCore/Helpers/BezierCurve/ldBezierCurve.h"

#include <ldCore/Helpers/Maths/ldMaths.h>

ldVec2 ldBezierCurve::start() const
{
    return m_start;
}

void ldBezierCurve::setStart(const ldVec2 &value)
{
    m_start = value;

    m_length = -1.f;
}

ldVec2 ldBezierCurve::end() const
{
    return m_end;
}

void ldBezierCurve::setEnd(const ldVec2 &value)
{
    m_end = value;

    m_length = -1.f;
}

ldVec2 ldBezierCurve::control1() const
{
    return m_control1;
}

void ldBezierCurve::setControl1(const ldVec2 &value)
{
    m_control1 = value;

    m_length = -1.f;
}

ldVec2 ldBezierCurve::control2() const
{
    return m_control2;
}

void ldBezierCurve::setControl2(const ldVec2 &value)
{
    m_control2 = value;

    m_length = -1.f;
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
            ldVec2 p_i = getPoint(slope_i);
            ldVec2 p_ib = getPoint(slope_ib);
            m_length += sqrtf( (p_ib.x-p_i.x)*(p_ib.x-p_i.x) + (p_ib.y-p_i.y)*(p_ib.y-p_i.y) );
        }
    }
    return m_length;
}

ldVec2 ldBezierCurve::getPoint(float slope) const
{
    ldVec2 res;
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

void ldBezierCurve::translate(const ldVec2 &t)
{
    m_start += t;
    m_end += t;
    m_control1 += t;
    m_control2 += t;
}

