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

#include <cmath>

#include <QtCore/QDebug>

ldDeadzoneFilter::ldDeadzoneFilter()
{
    m_lastV.clear();
}

void ldDeadzoneFilter::processFrame(std::vector<ldVertex> &frame) {
    if(!m_enabled || m_blocked)
        return;

    bool isLastOn = false;
    for(uint i = 0; i < frame.size(); i++) {
        // see if we're on
        bool isOn = this->isOn(frame[i].x(), frame[i].y());

        // compare on-status with last frame
        if (isLastOn && isOn) {
            // both points visible, do nothing

            // check if there is a deadzone between 2 points

            if(i > 0) {
                const int INTERVAL_COUNT = 3;
                float deltaX = frame[i].x() - frame[i-1].x();
                float deltaY = frame[i].y() - frame[i-1].y();
                ldVertex midV = frame[i-1];
                float f = 1.f / INTERVAL_COUNT;
                for(int j = 0; j < INTERVAL_COUNT; j++) {
                    midV.x() += deltaX * f;
                    midV.y() += deltaY * f;
                    bool isDeltaOn = this->isOn(midV.x(), midV.y());
                    if(!isDeltaOn) {
                        frame.insert(frame.begin() + i, midV);
                        i--;
                        isLastOn = false;
                        break;
                    }
                }
            }
        } else if (!isLastOn && !isOn) {
            // both points not visible, keep laser black
            attenuate(frame[i]);
        } else {
            if(i > 0) {
                // all points for on/off switch should be on border
                ldVec2 border = getBorderPoint(frame[i-1], frame[i], isLastOn);
                frame[i].x() = border.x;
                frame[i].y() = border.y;

                // insert points to frame
                auto insertBorderPoints = [&](int pointCnt) {
                    frame.insert(frame.begin() + i + 1, pointCnt, frame[i]);
                    i += pointCnt;
                };

//                auto getDistance = [&](const Vertex &v1, const Vertex &v2) {
//                    return sqrtf(powf((v1.x()-v2.x()), 2) + powf((v1.y()-v2.y()), 2));
//                };


                static const int POINTS_TO_START = 8;
//                static const int POINTS_TO_STOP = 4;

                if(!isOn) {
//                    const float MAX_DISTANCE = 0.01f;
//                    float distance = getDistance(frame[i-1], frame[i]);
//                    if(distance > MAX_DISTANCE) {
//                        const int POINTS_TO_INSERT = distance / MAX_DISTANCE;
//                        float distX = frame[i].x() - frame[i-1].x();
//                        float distY = frame[i].y() - frame[i-1].y();
//                        Vertex midV = frame[i-1];
//                        float percent = 1.f / POINTS_TO_INSERT;
//                        for(int j = 0; j < POINTS_TO_INSERT; j++) {
//                            midV.x() += distX * percent;
//                            midV.x() += distY * percent;
//                            frame.insert(frame.begin() + i, midV);
//                            i++;
//                        }
//                    }

                    // first border is colored, next one is blank
                    insertBorderPoints(POINTS_TO_START);
                    // frame[i] here because v is an old item now; TODO: ref to more beautiful one solution
                    attenuate(frame[i]);
//                    insertBorderPoints(POINTS_TO_STOP);

                } else {
                    // store original value
                    ldVertex original = frame[i];

                    // first border is blank, next one is colored
                    attenuate(frame[i]);
                    insertBorderPoints(POINTS_TO_START);
                    frame[i] = original;
//                    insertBorderPoints(POINTS_TO_STOP);
                }
            }
        }
        // memory for dead zones
        isLastOn = isOn;
    }
}

void ldDeadzoneFilter::process(ldVertex &v) {
    if(!m_enabled || m_blocked)
        return;

    // see if we're on
    bool isOn = this->isOn(v.x(), v.y());
    // compare on-status with last frame
    if (m_isLastOn && isOn) {
        // both points visible, do nothing
        pinToBorder(v);
    } else if (!m_isLastOn && !isOn) {
        // both points not visible, keep laser black
        attenuate(v);
        pinToBorder(v);
    } else {

//        Vertex oldV = v;
        if(m_lastV.isValid()) {
            // snap to border
            ldVec2 border = getBorderPoint(m_lastV, v, m_isLastOn);
            v.x() = border.x;
            v.y() = border.y;
            m_borderCount = 0;
            m_lastBorder = border;
        }

//        if (!isOn)
            attenuate(v);
//        qDebug() << (int) isOn << m_lastV << oldV << v;
    }
    // memory for dead zones
    m_isLastOn = isOn;
    m_lastV = v;
}

void ldDeadzoneFilter::resetFilter()
{
    m_isLastOn = true;
    m_lastV.clear();
}


void ldDeadzoneFilter::add(const ldDeadzoneFilter::Deadzone &deadzone)
{
    m_deadzones.push_back(deadzone);
}

void ldDeadzoneFilter::clear()
{
    m_deadzones.clear();
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

void ldDeadzoneFilter::setReverse(bool reverse)
{
    m_reverse = reverse;
}

void ldDeadzoneFilter::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

void ldDeadzoneFilter::setBlocked(bool blocked)
{
    m_blocked = blocked;
}

void ldDeadzoneFilter::attenuate(ldVertex& v) const {
    const Deadzone &dz = getDeadzone(v.x(), v.y());
    v.r() *= (1-dz.attenuation());
    v.g() *= (1-dz.attenuation());
    v.b() *= (1-dz.attenuation());
}

bool ldDeadzoneFilter::isOn(float x, float y) const
{
    bool isOut = true;

    for(const Deadzone &dz : m_deadzones) {
        if(dz.visRect().contains(x, y)) {
            isOut = false;
            break;
        }
    }
    if(m_reverse) isOut = !isOut;
    return isOut;
}

ldVec2 ldDeadzoneFilter::getBorderPoint(const ldVertex &lastV, const ldVertex &v, bool isLastOn) const
{
    // get border point
    ldVec2 border(lastV.x(), lastV.y());

    const int INTERVAL_COUNT = 32; // quality of interpolation, 8 should be fine?
    float deltaX = v.x() - lastV.x();
    float deltaY = v.y() - lastV.y();
    for (int i = 0; i < INTERVAL_COUNT + 1; i++) {
        float f = static_cast<float>(i) / INTERVAL_COUNT;
        float tx = lastV.x() + deltaX * f;
        float ty = lastV.y() + deltaY * f;
        bool isDeltaOn = this->isOn(tx, ty);

//        if((m_isLastOn && isDeltaOn)
//            || (!m_isLastOn && isDeltaOn)) {
        if (isDeltaOn != isLastOn) {
//        if (isDeltaOn == m_isLastOn) {
            border.x = tx;
            border.y = ty;
            break;
        }
    }


    {
        // smooth the snap at lower attenuations
        const Deadzone &dz = getDeadzone(v.x(), v.y());
        float catt = dz.attenuation();
        float att = 1 - catt;

        border.x = catt*border.x + att*v.x();
        border.y = catt*border.y + att*v.y();
    }


    return border;
}

ldDeadzoneFilter::Deadzone ldDeadzoneFilter::getDeadzone(float x, float y) const {
    ldDeadzoneFilter::Deadzone deadzone;

    float maxAttenuation = 0.f;
    for(const Deadzone &dz : m_deadzones) {
        if(!dz.visRect().contains(x, y))
            continue;

        if(dz.attenuation() > maxAttenuation) {
            deadzone = dz;
            maxAttenuation = dz.attenuation();
        }
    }
    return deadzone;
}

void ldDeadzoneFilter::pinToBorder(ldVertex &v)
{
    if(!m_lastBorder.isNull()) {
        v.x() = m_lastBorder.x;
        v.y() = m_lastBorder.y;
        m_borderCount++;
        if(m_borderCount >= MAX_BORDER_COUNT)
            m_lastBorder = ldVec2();
    }
}



ldDeadzoneFilter::Deadzone::Deadzone(QRectF rect, float attenuation)
    : m_attenuation(attenuation)
    , m_rect(rect)
{
    updateVisRect();
}

QRectF ldDeadzoneFilter::Deadzone::rect() const
{
    return m_rect;
}

const QRectF &ldDeadzoneFilter::Deadzone::visRect() const
{
    return  m_visRect;
}

void ldDeadzoneFilter::Deadzone::moveLeft(float value)
{
    m_rect.moveLeft(value);
    updateVisRect();
}

void ldDeadzoneFilter::Deadzone::moveTop(float value)
{
    m_rect.moveTop(value);
    updateVisRect();
}

void ldDeadzoneFilter::Deadzone::setWidth(float value)
{
    m_rect.setWidth(value);
    updateVisRect();
}

void ldDeadzoneFilter::Deadzone::setHeight(float value)
{
    m_rect.setHeight(value);
    updateVisRect();
}

void ldDeadzoneFilter::Deadzone::setAttenuation(float attenuation)
{
    m_attenuation = attenuation;
}

float ldDeadzoneFilter::Deadzone::attenuation() const
{
    return m_attenuation;
}

void ldDeadzoneFilter::Deadzone::updateVisRect()
{
    float dpx = m_rect.x();
    float dpy = m_rect.y();
    float dzx = m_rect.width();
    float dzy = m_rect.height();

    float x1 = dpx*(1-dzx)-dzx;
    float x2 = dpx*(1-dzx)+dzx;
    float y1 = dpy*(1-dzy)-dzy;
    float y2 = dpy*(1-dzy)+dzy;

    m_visRect = QRectF(QPointF(x1, y1), QPointF(x2, y2));
}

