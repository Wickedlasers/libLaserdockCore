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

#include <ldCore/Helpers/Color/ldGradient.h>

#include <cmath>

#include <QtCore/QtDebug>
#include <QtGui/QColor>

ldGradientStop::ldGradientStop(float offset, uint32_t color)
    : m_offset(offset)
    , m_color(color)
{
}

float ldGradientStop::offset() const
{
    return m_offset;
}

uint32_t ldGradientStop::color() const
{
    return m_color;
}

bool ldGradientStop::isValid() const
{
    return m_offset != -1;
}

ldGradient::ldGradient(const std::vector<ldGradientStop> &stops)
    : m_stops(stops)
    , m_v1(ldVec2(-1.f, -0.5f))
    , m_v2(ldVec2(-1.f, -0.5f))
{

}

void ldGradient::add(const ldGradientStop &stop)
{
    m_stops.push_back(stop);
}

float ldGradient::x1() const
{
    return m_v1.x;
}

void ldGradient::setX1(float value)
{
    m_v1.x = value;
}

float ldGradient::y1() const
{
    return m_v1.y;
}

void ldGradient::setY1(float value)
{
    m_v1.y = value;
}

float ldGradient::x2() const
{
    return m_v2.x;
}

void ldGradient::setX2(float value)
{
    m_v2.x = value;
}

float ldGradient::y2() const
{
    return m_v2.y;
}

void ldGradient::setY2(float value)
{
    m_v2.y = value;
}

std::vector<ldGradientStop> ldGradient::stops() const
{
    return m_stops;
}

bool ldGradient::isValid() const
{
    return !m_stops.empty();
}

uint32_t ldGradient::getColor(float x, float y) const
{
    // to united coords
    float offset = getOffset(y, x);

    // find gradient stops for this point
    ldGradientStop s1;
    ldGradientStop s2;

    for(const ldGradientStop &stop : m_stops) {
        if(stop.offset() != 1.f && offset >= stop.offset())
            s1 = stop;
    }

    if(!s1.isValid()) {
        s1 = m_stops[0];
    }

    for(const ldGradientStop &stop : m_stops) {
        if(stop.offset() != 0.f && offset <= stop.offset() && !s2.isValid())
            s2 = stop;
    }

    if(!s2.isValid()) {
        s2 = m_stops[m_stops.size() - 1];
    }

//    qDebug() << "stops" << this << offset << s1.offset() << s1.color() << s2.offset() << s2.color();

    return mixColors(s1.color(), s2.color(), offset);
}

void ldGradient::rotate(float value)
{
    m_v1.rotate(value);
    m_v2.rotate(value);
}

void ldGradient::scale(float value)
{
    m_v1 *= value;
    m_v2 *= value;
}

void ldGradient::translate(const ldVec2 &v)
{
    m_v1 += v;
    m_v2 += v;
}

float ldGradient::getOffset(float y, float x) const
{
    auto unitedCoord = [](float coord) ->float {
            return (coord+1.f)*0.5f;
    };

//    qDebug() << this << "g" << this->x1() << this->y1() << this->x2() << this->y2();
//    qDebug() << this << "p" << x << y;

    x = unitedCoord(x);
    y = unitedCoord(y);
    float x1 = unitedCoord(this->x1());
    float y1 = unitedCoord(this->y1());
    float x2 = unitedCoord(this->x2());
    float y2 = unitedCoord(this->y2());

    // find slope for current point
    float offsetX = (x2 != x1)
            ? (x - x1) / (x2 - x1)
            : -1.f;
    float offsetY = (y2 != y1)
            ? (y - y1) / (y2 - y1)
            : -1.f;

    float offset = 0;
    if(offsetX != -1.f && offsetY != -1.f) {
        offset = (offsetX + offsetY) / 2.f;
    } else if(offsetX == -1.f) {
        offset = offsetY;
    } else if(offsetY == -1.f) {
        offset = offsetX;
    }

    offset = std::min(offset, 1.f);
    offset = std::max(offset, 0.f);

//    qDebug() << this << "g" << x1 << y1 << x2 << y2;
//    qDebug() << this << "p" << x << y;
//    qDebug() << this << "s" << offset << offsetX << offsetY;

    return offset;
}

uint32_t ldGradient::mixColors(uint32_t color1, uint32_t color2, float offset) const
{
    QColor c1(QRgba64::fromArgb32(color1));
    QColor c2(QRgba64::fromArgb32(color2));


    float r = c1.redF() + (c2.redF() - c1.redF()) * offset;
    float g = c1.greenF() + (c2.greenF() - c1.greenF()) * offset;
    float b = c1.blueF() + (c2.blueF() - c1.blueF()) * offset;

    r = std::min(1.f, r);
    g = std::min(1.f, g);
    b = std::min(1.f, b);

//    qDebug() << "c1" << c1.redF() << c1.greenF() << c1.blueF();
//    qDebug() << "c2" << c2.redF() << c2.greenF() << c2.blueF();
//    qDebug() << "res" << color1 << color2 << offset << r << g << b;

    // b g r
    return QColor::fromRgbF(r, g, b).rgb();
}
