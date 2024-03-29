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

#include "ldCore/Helpers/Text/ldSvgLetter.h"

#include <QtCore/QtDebug>
#include <QtCore/QHash>

#include "ldCore/Helpers/SVG/ldSvgReader.h"

QMap<QString, ldBezierCurveObject> SVG_LETTERS_CACHE;

ldSvgLetter::ldSvgLetter(const QString &svgPath, const QChar &charValue)
    : m_svgPath(svgPath)
    , m_charValue(charValue)
{
    m_originalObject.setUnitedCoordinates(true);
}

QChar ldSvgLetter::charValue() const
{
    return m_charValue;
}

const ldBezierCurveObject &ldSvgLetter::data() const
{
    if(m_object.isEmpty()) {
        updateObject();
    }

    return m_object;
}

void ldSvgLetter::translate(const ldVec2 &v)
{
    m_translateVec += v;

    resetObject();
}

void ldSvgLetter::scale(float scale)
{
    m_scale *= scale;

    resetObject();
}

void ldSvgLetter::resetObject()
{
    m_object.clear();
}

void ldSvgLetter::updateObject() const
{
    if(m_originalObject.isEmpty()) {
        if(SVG_LETTERS_CACHE.contains(m_svgPath)) {
            m_originalObject = SVG_LETTERS_CACHE.value(m_svgPath);
        } else {
            ldBezierCurveObject curveObject = ldSvgReader::loadSvg(m_svgPath, ldSvgReader::Type::Dev, 0.01f);

            m_originalObject.clear();
            m_originalObject.add(curveObject.data());
            m_originalObject.translate(ldVec2(-m_originalObject.dim().bottom_left.x, 0));

            SVG_LETTERS_CACHE.insert(m_svgPath, m_originalObject);
        }
    }


    m_object = m_originalObject;

    if(!m_translateVec.isNull()) {
        m_object.translate(m_translateVec);
    }

    if(m_scale != 1.f) {
        m_object.scale(m_scale);
    }
}
