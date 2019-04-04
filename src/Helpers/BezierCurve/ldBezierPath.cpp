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

#include "ldCore/Helpers/BezierCurve/ldBezierPath.h"


ldBezierPath::ldBezierPath(const std::vector<ldBezierCurve> &curves)
    : m_curves(curves)
{
}

void ldBezierPath::add(const ldBezierCurve &curve)
{
    m_curves.push_back(curve);
}

const std::vector<ldBezierCurve> &ldBezierPath::data() const
{
    return m_curves;
}

std::vector<ldBezierCurve> &ldBezierPath::data()
{
    return m_curves;
}

uint32_t ldBezierPath::color() const
{
    return m_color;
}

void ldBezierPath::setColor(const uint32_t &value)
{
    m_color = value;
}

void ldBezierPath::resize(uint size)
{
    m_curves.resize(size);
}

uint ldBezierPath::size() const
{
    return m_curves.size();
}

bool ldBezierPath::empty() const
{
    return m_curves.empty();
}

void ldBezierPath::clear()
{
    m_curves.clear();
    m_color = 0xFFFFFFFF;
    m_gradient = ldGradient();
}

ldBezierCurve &ldBezierPath::operator [](int index)
{
    return m_curves[index];
}

const ldBezierCurve &ldBezierPath::operator [](int index) const
{
    return m_curves[index];
}

const ldGradient &ldBezierPath::gradient() const
{
    return m_gradient;
}

void ldBezierPath::setGradient(const ldGradient &gradient)
{
    m_gradient = gradient;
}

void ldBezierPath::rotate(float value)
{
    for (ldBezierCurve &bZ : m_curves) {
        bZ.rotate(value);
    }

    if(m_gradient.isValid())
        m_gradient.rotate(value);
}

void ldBezierPath::scale(float value)
{
    for (ldBezierCurve &bZ : m_curves) {
        bZ.scale(value);
    }

    if(m_gradient.isValid())
        m_gradient.scale(value);
}

void ldBezierPath::translate(const ldVec2 &v)
{
    for (ldBezierCurve &bZ : m_curves) {
        bZ.translate(v);
    }

    if(m_gradient.isValid())
        m_gradient.translate(v);
}


