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

#ifndef LDSVGLETTER_H
#define LDSVGLETTER_H

#include <QtCore/QString>

#include "ldCore/Helpers/BezierCurve/ldBezierCurveObject.h"
#include "ldCore/Helpers/Maths/ldMaths.h"

class LDCORESHARED_EXPORT ldSvgLetter
{
public:
    explicit ldSvgLetter(const QString &svgPath);

    const ldBezierCurveObject &data() const;

    void translate(const Vec2 &v);
    void scale(float scale);

private:
    void resetObject();
    void updateObject() const;

    QString m_svgPath;

    Vec2 m_translateVec;
    float m_scale = 1.f;

    mutable ldBezierCurveObject m_originalObject;
    mutable ldBezierCurveObject m_object;
};

#endif // LDSVGLETTER_H


