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
//  ldOLPointObject.cpp
//
//  Created by Sergey Gavrushkin on 19/02/17.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#include <ldCore/Helpers/Openlase/ldOLPointObject.h>

#include <ldCore/Helpers/Maths/ldMaths.h>
#include <QtQuick/QQuickItem>

ldOLPointObject::ldOLPointObject(const std::vector<PointVector> &pointVectors)
    : m_pointVectors(pointVectors)
{
}

bool ldOLPointObject::isEmpty() const
{
    return m_pointVectors.empty();
}


void ldOLPointObject::clear()
{
    m_pointVectors.clear();

    resetCache();
}

const std::vector<PointVector> &ldOLPointObject::data() const
{
    return m_pointVectors;
}

std::vector<PointVector> &ldOLPointObject::data()
{
    return m_pointVectors;
}

const ldRect &ldOLPointObject::dim() const
{
    if(m_cachedDim.isNull()) {

        ldRect res;
        bool init = false;
        for (const PointVector &pv : m_pointVectors) {
            for (const OLPoint &p : pv) {
                if (!init) {
                    // very first point
                    res.bottom_left = ldVec2(p.x, p.y);
                    res.top_right = ldVec2(p.x, p.y);
                    init = true;
                    continue;
                }
                // bottom left versus start
                res.bottom_left.x = std::min(res.bottom_left.x, p.x);
                res.bottom_left.y = std::min(res.bottom_left.y, p.y);

                // top right versus start
                res.top_right.x = std::max(res.top_right.x, p.x);
                res.top_right.y = std::max(res.top_right.y, p.y);
            }
        }

        m_cachedDim = res;
    }

    return m_cachedDim;
}

void ldOLPointObject::translate(const ldVec2 &vec2)
{
    for(PointVector &pv : m_pointVectors) {
        for(OLPoint &p : pv) {
            p.x += vec2.x;
            p.y += vec2.y;
        }
    }

    resetCache();
}

// TODO: instead of absolute delta size use multiplication to size
void ldOLPointObject::transform(const ldVec2 &v, int alignment)
{
//    qDebug() << __FUNCTION__ << v << transformAlignment;

    ldRect rect = dim();

    for(PointVector &pv : m_pointVectors) {
        for(OLPoint &p : pv) {
            Qt::Alignment to = Qt::Alignment(alignment);
            if(to.testFlag(Qt::AlignTop)) {
                float relative = (p.y - rect.bottom())/rect.height();
                p.y += relative*v.y;
            }
            if(to.testFlag(Qt::AlignBottom)) {
                float relative = (rect.top() - p.y)/rect.height();
                p.y += relative*v.y;
            }
            if(to.testFlag(Qt::AlignLeft)) {
                float relative = (rect.right() - p.x)/rect.width();
                p.x += relative*v.x;
            }
            if(to.testFlag(Qt::AlignRight)) {
                float relative = (p.x - rect.left())/rect.width();
                p.x += relative*v.x;
            }
        }
    }

    resetCache();
}

void ldOLPointObject::rotate(float angle)
{
    for(PointVector &pv : m_pointVectors) {
        for(OLPoint &p : pv) {
            float x = ldMaths::laserToUnitedCoords(p.x);
            float y = ldMaths::laserToUnitedCoords(p.y);
            float oldX = x;
            x = x*cosf(angle)-y*sinf(angle);
            y = oldX*sinf(angle)+y*cosf(angle);
            p.x = ldMaths::unitedToLaserCoords(x);
            p.y = ldMaths::unitedToLaserCoords(y);
        }
    }

   resetCache();
}


void ldOLPointObject::rotate(float angle, const ldVec2 &center)
{
//    qDebug() << __FUNCTION__ << angle << angle *180.f/M_PIf;

    auto rotate_point = [&](float cx, float cy,float angle, OLPoint p)
    {
        float s = sin(angle);
        float c = cos(angle);

        // translate point back to origin:
        p.x -= cx;
        p.y -= cy;

        // rotate point
        float xnew = p.x * c - p.y * s;
        float ynew = p.x * s + p.y * c;

        // translate point back:
        p.x = xnew + cx;
        p.y = ynew + cy;
        return p;
    };

    for(PointVector &pv : m_pointVectors) {
        for(OLPoint &p : pv) {
            p = rotate_point(center.x, center.y, angle*-1.f, p);
        }
    }

    resetCache();
}

void ldOLPointObject::scale(float scaleValue)
{

    for(PointVector &pv : m_pointVectors) {
        for(OLPoint &p : pv) {
            float x = ldMaths::laserToUnitedCoords(p.x);
            float y = ldMaths::laserToUnitedCoords(p.y);
            x = scaleValue*x;
            y = scaleValue*y;
            p.x = ldMaths::unitedToLaserCoords(x);
            p.y = ldMaths::unitedToLaserCoords(y);
        }
    }

    resetCache();

}

void ldOLPointObject::add(const PointVector &pv)
{
    m_pointVectors.push_back(pv);
    resetCache();
}

void ldOLPointObject::colorize(uint32_t color)
{
    for(PointVector &pv : m_pointVectors) {
        for(OLPoint &p : pv) {
            p.color = color;
        }
    }
}


void ldOLPointObject::moveTo(const ldVec2 &pos)
{
    ldVec2 moveDiff;
    moveDiff.x = pos.x - dim().left();
    moveDiff.y = pos.y - dim().bottom();

    translate(moveDiff);

    resetCache();
}

void ldOLPointObject::moveToCenter()
{
    const float centerCoord = 2.f;
    ldVec2 centerPos;
    centerPos.x = (centerCoord - dim().width())/2.f;
    centerPos.y = (centerCoord - dim().height())/2.f;

    translate(centerPos);

    resetCache();
}



void ldOLPointObject::resetCache()
{
    m_cachedDim = ldRect();
}


