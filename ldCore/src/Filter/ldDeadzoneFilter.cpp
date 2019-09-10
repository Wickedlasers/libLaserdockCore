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

#include "ldCore/Filter/ldDeadzoneFilter.h"
#include <QtCore/QDebug>
#include <math.h>

ldDeadzoneFilter::ldDeadzoneFilter()
{
    m_lastV.clear();
}

void ldDeadzoneFilter::process(Vertex &v) {
    if(!m_enabled)
        return;

    // see if we're on
    bool isOn = this->isOn(v.x(), v.y());
    // compare on-status with last frame
    if (m_lastOn && isOn) {
        // both points visible, do nothing
    } else if (!m_lastOn && !isOn) {
        // both points not visible, keep laser black
        attenuate(v);
    } else {

        if(m_lastV.isValid()) {
            // snap to border
            float bestx = m_lastV.x();
            float besty = m_lastV.y();
            //float bestx = v.x();
            //float besty = v.y();
            const int INTERVAL_COUNT = 32; // quality of interpolation, 8 should be fine?
            float deltaX = v.x() - m_lastV.x();
            float deltaY = v.y() - m_lastV.y();
            for (int i = 0; i < INTERVAL_COUNT; i++) {
                float f = (float) i / INTERVAL_COUNT;
                float tx = m_lastV.x() + deltaX * f;
                float ty = m_lastV.y() + deltaY * f;
                bool testOn = this->isOn(tx, ty);
                if (testOn == m_lastOn) {
                    bestx = tx;
                    besty = ty;
                    break;
                }
            }

            {
                // smooth the snap at lower attenuations
                Vertex vt = v;
                vt.color[0] = 1;
                attenuate(vt);
                float att = vt.color[0];
                float catt = 1-att;
                bestx = catt*bestx + att*v.x();
                besty = catt*besty + att*v.y();
            }

            v.x() = bestx;
            v.y() = besty;
        }

        if (!isOn)
            attenuate(v);
    }
    // memory for dead zones
    m_lastOn = isOn;
    m_lastV = v;
}

void ldDeadzoneFilter::resetFilter()
{
    m_lastOn = true;
    m_lastV.clear();
}

const QList<ldDeadzoneFilter::Deadzone> &ldDeadzoneFilter::deadzones() const
{
    return m_deadzones;
}

ldDeadzoneFilter::Deadzone *ldDeadzoneFilter::firstDeadzone()
{
    return !m_deadzones.isEmpty() ? &m_deadzones.first() : nullptr;
}

void ldDeadzoneFilter::resetToDefault()
{
    clear();
    add(ldDeadzoneFilter::Deadzone(QRectF(0, 0, 0.5, 0.5)));
}

void ldDeadzoneFilter::add(const ldDeadzoneFilter::Deadzone &deadzone)
{
    m_deadzones.push_back(deadzone);
}

void ldDeadzoneFilter::clear()
{
    m_deadzones.clear();
}

bool ldDeadzoneFilter::isOn(float x, float y) const
{
    bool isOut = isOutside(x, y);
    if(m_reverse) isOut = !isOut;
    return isOut;
}

bool ldDeadzoneFilter::isOutside(float x, float y) const {
    bool isOutside = true;

    for(const Deadzone &dz : m_deadzones) {
        if(dz.visRect().contains(x, y)) {
            isOutside = false;
            break;
        }
    }
    return isOutside;
}

ldDeadzoneFilter::Deadzone ldDeadzoneFilter::getDeadzone(float x, float y) const {
    ldDeadzoneFilter::Deadzone deadzone;

    float maxAttenuation = 0.f;
    for(const Deadzone &dz : m_deadzones) {
        if(!dz.visRect().contains(x, y))
            continue;

        if(dz.m_attenuation > maxAttenuation) {
            deadzone = dz;
            maxAttenuation = dz.m_attenuation;
        }
    }
    return deadzone;
}

void ldDeadzoneFilter::attenuate(Vertex& v) {
    const Deadzone &dz = getDeadzone(v.x(), v.y());
    v.color[0] *= (1-dz.m_attenuation);
    v.color[1] *= (1-dz.m_attenuation);
    v.color[2] *= (1-dz.m_attenuation);
}


ldDeadzoneFilter::Deadzone::Deadzone(QRectF rect, float attenuation)
    : m_rect(rect)
    , m_attenuation(attenuation)
{

}

QRectF ldDeadzoneFilter::Deadzone::visRect() const
{
    float dpx = m_rect.x();
    float dpy = m_rect.y();
    float dzx = m_rect.width();
    float dzy = m_rect.height();

    float x1 = dpx*(1-dzx)-dzx;
    float x2 = dpx*(1-dzx)+dzx;
    float y1 = dpy*(1-dzy)-dzy;
    float y2 = dpy*(1-dzy)+dzy;

    return QRectF(QPointF(x1, y1), QPointF(x2, y2));
}

