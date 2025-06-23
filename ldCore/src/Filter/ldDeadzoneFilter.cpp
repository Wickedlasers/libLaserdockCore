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
    QMutexLocker lock(&m_mutex);

    if(!m_enabled || m_blocked)
        return;

    m_isLastOn = false;

    for(uint i = 0; i < frame.size(); i++) {
        // see if we're on
        bool isOn = this->isOn(frame[i].x(), frame[i].y());

        // compare on-status with last frame
        if (m_isLastOn && isOn) {
            // both points visible, do nothing

            // check if there is a deadzone between 2 points

                const int INTERVAL_COUNT = 3;
                float deltaX = frame[i].x() - m_lastV.x();
                float deltaY = frame[i].y() - m_lastV.y();
                ldVertex midV = m_lastV;
                float f = 1.f / INTERVAL_COUNT;
                for(int j = 0; j < INTERVAL_COUNT; j++) {
                    midV.x() += deltaX * f;
                    midV.y() += deltaY * f;
                    bool isDeltaOn = this->isOn(midV.x(), midV.y());
                    if(!isDeltaOn) {
                        frame.insert(frame.begin() + i, midV);
                        attenuate(frame[i]);
                        i--;
                        break;
                    }
//
            }
        } else if (!m_isLastOn && !isOn) {
            // both points not visible, keep laser black
            attenuate(frame[i]);
        } else {
//            if(i > 0) {
                // all points for on/off switch should be on border
                ldVec2 border = getBorderPoint(m_lastV, frame[i], m_isLastOn);
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
//            } else {
                //                qDebug() << isOn;
//                if(i == 0) {
//                    attenuate(frame[i]);
//                    isOn = false;
//                }
//            }
        }
        // memory for dead zones
        m_lastV = frame[i];
        m_isLastOn = isOn;
    }
}

void ldDeadzoneFilter::process(ldVertex &v) {
    QMutexLocker lock(&m_mutex);

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
//    m_isLastOn = false;
//    m_lastV.clear();
}


void ldDeadzoneFilter::add(const ldDeadzoneFilter::Deadzone &deadzone)
{
    QMutexLocker lock(&m_mutex);
    m_deadzones.push_back(deadzone);
}

void ldDeadzoneFilter::clear()
{
    QMutexLocker lock(&m_mutex);
    m_deadzones.clear();
}

void ldDeadzoneFilter::removeCurrent()
{
    QMutexLocker lock(&m_mutex);
    // do not allow to remove the last deadzone
    if(m_deadzones.size() == 1)
        return;

    m_deadzones.removeAt(m_selectedIndex);
    if(m_selectedIndex >= m_deadzones.size())
        m_selectedIndex = m_deadzones.size() - 1;
}

const QList<ldDeadzoneFilter::Deadzone> &ldDeadzoneFilter::deadzones() const
{
    // QMutexLocker lock(&m_mutex);
    return m_deadzones;
}

ldDeadzoneFilter::Deadzone *ldDeadzoneFilter::selectedDeadzone()
{
    QMutexLocker lock(&m_mutex);
    return !m_deadzones.isEmpty() && m_deadzones.size() > m_selectedIndex ? &m_deadzones[m_selectedIndex] : nullptr;
}

int ldDeadzoneFilter::selectedIndex() const
{
    // QMutexLocker lock(&m_mutex);
    return m_selectedIndex;
}

void ldDeadzoneFilter::setSelectedIndex(int index)
{
    QMutexLocker lock(&m_mutex);
    if(index < 0 || index >= m_deadzones.size()) {
        m_selectedIndex = 0;
        return;
    }

    m_selectedIndex = index;
}


void ldDeadzoneFilter::resetToDefault()
{
    QMutexLocker lock(&m_mutex);
    m_deadzones.clear();
    m_deadzones.push_back(ldDeadzoneFilter::Deadzone(QRectF(0, 0, 0.5, 0.5)));
    m_selectedIndex = 0;
}

void ldDeadzoneFilter::setReverse(bool reverse)
{
    QMutexLocker lock(&m_mutex);
    m_reverse = reverse;
}

void ldDeadzoneFilter::setEnabled(bool enabled)
{
    QMutexLocker lock(&m_mutex);
    m_enabled = enabled;
}

void ldDeadzoneFilter::setBlocked(bool blocked)
{
    QMutexLocker lock(&m_mutex);
    m_blocked = blocked;
}

void ldDeadzoneFilter::lockMutex()
{
    m_mutex.lock();
}

void ldDeadzoneFilter::unlockMutex()
{
    m_mutex.unlock();
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
        if (dz.shapeIndex() == 0) {
            // could call dz.contains but surely a bit quicker like this
            if(dz.visRect().contains(x, y)) {
                isOut = false;
                break;
            }
        } else {
            if(dz.contains(x, y)) {
                isOut = false;
                break;
            }
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

ldDeadzoneFilter::Deadzone::Deadzone(QRectF rect, float attenuation, int shapeIndex)
    : m_attenuation(attenuation)
    , m_shapeIndex(shapeIndex)
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

const QList<QPointF>  &ldDeadzoneFilter::Deadzone::shape() const
{
    return  m_shape;
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

void ldDeadzoneFilter::Deadzone::setShapeIndex(int shapeIndex)
{
    m_shapeIndex = shapeIndex;
    updateVisRect();
}


int ldDeadzoneFilter::Deadzone::shapeIndex() const
{
    return m_shapeIndex;
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

    //
    switch (DeadZoneShapes(m_shapeIndex)) {
    case MShapeCircle:
        updateShapeCircle();
        break;
    case MShapeTriangle:
        updateShapeTriangle();
        break;
    case MShapeCornerTopLeft:
        updateShapeCorner(0);
        break;
    case MShapeCornerTopRight:
        updateShapeCorner(1);
        break;
    case MShapeCornerBotRight:
        updateShapeCorner(2);
        break;
    case MShapeCornerBotLeft:
        updateShapeCorner(3);
        break;
    case MShapeDiamond:
        updateShapeDiamond();
        break;
    case MShapeRect:
    default:
        updateShapeRectangle();
        break;
    }
}

//
void ldDeadzoneFilter::Deadzone::updateShapeRectangle()
{
    m_shape.clear();
    //
    m_shape.push_back(m_visRect.topLeft());
    m_shape.push_back(m_visRect.topRight());
    m_shape.push_back(m_visRect.bottomRight());
    m_shape.push_back(m_visRect.bottomLeft());
    m_shape.push_back(m_visRect.topLeft());
}

//
void ldDeadzoneFilter::Deadzone::updateShapeCircle()
{
    m_shape.clear();
    // maybe set 64 .. but can be rapidly heavy as we compute on each points in contains(x,y)..
    int maxPoints = 32;
    for (int i=0; i<maxPoints; i++) {
        float slope = (1.0f*i)/(maxPoints-1.0f);
        float x = 0.5f*m_visRect.width()*cosf(slope*M_2PIf) + m_visRect.center().x();
        float y = 0.5f*m_visRect.height()*sinf(slope*M_2PIf) +  m_visRect.center().y();
        m_shape.push_back(QPointF(x,y));
    }
}

//
void ldDeadzoneFilter::Deadzone::updateShapeTriangle()
{
    m_shape.clear();
    // y is inversed
    const QPointF& top_center = (m_visRect.bottomLeft() + m_visRect.bottomRight())/2;
    //
    m_shape.push_back(top_center);
    m_shape.push_back(m_visRect.topRight());
    m_shape.push_back(m_visRect.topLeft());
    m_shape.push_back(top_center);
}

//
void ldDeadzoneFilter::Deadzone::updateShapeCorner(int p_cornerCase)
{
    m_shape.clear();

    // case 0 default
    // y is inversed
    QPointF ptA = m_visRect.bottomLeft();
    QPointF ptB = m_visRect.bottomRight();
    QPointF ptC = m_visRect.topLeft();

    switch (p_cornerCase) {
    case 1: // TR
    {
        ptA = m_visRect.bottomRight();
        ptB = m_visRect.topRight();
        ptC = m_visRect.bottomLeft();
    }
        break;
    case 2: // BR
    {
        ptA = m_visRect.topRight();
        ptB = m_visRect.topLeft();
        ptC = m_visRect.bottomRight();
    }
    break;
    case 3: // BL
    {
        ptA = m_visRect.topLeft();
        ptB = m_visRect.bottomLeft();
        ptC = m_visRect.topRight();
    }
    break;
    default:
        break;
    }
    //
    m_shape.push_back(ptA);
    m_shape.push_back(ptB);
    m_shape.push_back(ptC);
    m_shape.push_back(ptA);
}

//
void ldDeadzoneFilter::Deadzone::updateShapeDiamond()
{
    m_shape.clear();
    const QPointF& top_center = (m_visRect.topLeft() + m_visRect.topRight())/2;
    const QPointF& right_center = (m_visRect.topRight() + m_visRect.bottomRight())/2;
    const QPointF& bot_center = (m_visRect.bottomLeft() + m_visRect.bottomRight())/2;
    const QPointF& left_center = (m_visRect.topLeft() + m_visRect.bottomLeft())/2;
    //
    m_shape.push_back(top_center);
    m_shape.push_back(right_center);
    m_shape.push_back(bot_center);
    m_shape.push_back(left_center);
    m_shape.push_back(top_center);
}

//
bool ldDeadzoneFilter::Deadzone::contains(float p_x, float p_y) const
{
    //
    if (m_shape.size() < 1) return false;
    int crossings = 0;
    size_t numPoints = m_shape.size();
    //
    for (size_t i = 0; i < numPoints; ++i) {
        size_t next = (i + 1) % numPoints;
        //
        if ((m_shape[i].y() <= p_y && p_y < m_shape[next].y()) ||
            (m_shape[next].y() <= p_y && p_y < m_shape[i].y())) {
            if (m_shape[i].y() != m_shape[next].y()) {
                    if (p_x < m_shape[i].x() + (m_shape[next].x() - m_shape[i].x()) * (p_y - m_shape[i].y()) / (m_shape[next].y() - m_shape[i].y())) {
                        crossings++;
                    }
            } else if (p_y == m_shape[i].y()
                       && p_x <= std::max(m_shape[i].x(), m_shape[next].x())
                       && p_x >= std::min(m_shape[i].x(), m_shape[next].x())) {
                    // point lies on the horizontal line segment
                    return true;
            }
        }
    }
    // if crossings is odd, the point is inside the polygon
    return (crossings % 2 == 1);
}
