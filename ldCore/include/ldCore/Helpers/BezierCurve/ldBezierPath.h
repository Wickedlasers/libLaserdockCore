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

#ifndef LDBEZIERPATH_H
#define LDBEZIERPATH_H

#include "ldCore/Helpers/BezierCurve/ldBezierCurve.h"
#include "ldCore/Helpers/Color/ldGradient.h"

class LDCORESHARED_EXPORT ldBezierPath {
public:
    explicit ldBezierPath(const std::vector<ldBezierCurve> &curves = std::vector<ldBezierCurve>());

    void add(const ldBezierCurve &curve);
    void setCurve(uint index, const ldBezierCurve &curve);

    const std::vector<ldBezierCurve> &data() const;

    uint32_t color() const;
    void setColor(const uint32_t &value);

    const ldGradient &gradient() const;
    void setGradient(const ldGradient &gradient);

    void rotate(float value);
    void scale(float value);
    void scale(float x, float y);
    void translate(const ldVec2 &v);

    size_t size() const;
    bool empty() const;
    void clear();

private:
    std::vector<ldBezierCurve> m_curves;

    uint32_t m_color = 0;
    ldGradient m_gradient;
};

typedef std::vector< ldBezierPath > ldBezierPaths;
typedef std::vector< ldBezierPaths> ldBezierPathsSequence;

#endif // LDBEZIERPATH_H

