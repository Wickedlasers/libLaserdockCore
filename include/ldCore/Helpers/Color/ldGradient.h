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

#ifndef LDGRADIENT_H
#define LDGRADIENT_H

#include <ldCore/ldCore_global.h>

#include <ldCore/Helpers/Maths/ldVec2.h>

// ldGradientStop

class LDCORESHARED_EXPORT ldGradientStop {
public:
    ldGradientStop(float offset = -1, uint32_t color = 0);

    float offset() const;
    uint32_t color() const;

    bool isValid() const;

private:
    float m_offset;
    uint32_t m_color;
};

// ldGradient

class LDCORESHARED_EXPORT ldGradient {

public:
    ldGradient(const std::vector<ldGradientStop> &stops = std::vector<ldGradientStop>());

    void add(const ldGradientStop &stop);

    float x1() const;
    void setX1(float value);

    float y1() const;
    void setY1(float value);

    float x2() const;
    void setX2(float value);

    float y2() const;
    void setY2(float value);

    std::vector<ldGradientStop> stops() const;

    bool isValid() const;

    uint32_t getColor(float x, float y) const;

    void rotate(float value);
    void scale(float value);
    void translate(const ldVec2 &v);

private:
    uint32_t mixColors(uint32_t color1, uint32_t color2, float offset) const;
    float getOffset(float y, float x) const;

    ldVec2 m_v1;
    ldVec2 m_v2;

    std::vector<ldGradientStop> m_stops;
};

#endif // LDGRADIENT_H

