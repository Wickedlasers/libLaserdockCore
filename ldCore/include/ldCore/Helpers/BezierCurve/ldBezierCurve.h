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

#ifndef LDBEZIERCURVE_H
#define LDBEZIERCURVE_H

#include "ldCore/Helpers/Maths/ldVec2.h"

class LDCORESHARED_EXPORT ldBezierCurve
{
public:
    static ldBezierCurve lerp(const ldBezierCurve &b1, const ldBezierCurve &b2, float f);

    ldVec2 start() const;
    void setStart(const ldVec2 &value);

    ldVec2 end() const;
    void setEnd(const ldVec2 &value);

    ldVec2 control1() const;
    void setControl1(const ldVec2 &value);

    ldVec2 control2() const;
    void setControl2(const ldVec2 &value);

    float length(int maxPoints = 100) const;
    ldVec2 getPoint(float slope) const;

    void scale(float s);
    void scaleX(float s);
    void scaleY(float s);
    void rotate(float rotation);
    void translate(const ldVec2 &t);

private:
    ldVec2 m_start;
    ldVec2 m_end;
    ldVec2 m_control1;
    ldVec2 m_control2;

    mutable float m_length = -1.f;
};

#endif // LDBEZIERCURVE_H

