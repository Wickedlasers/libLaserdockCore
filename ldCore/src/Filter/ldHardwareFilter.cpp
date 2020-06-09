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

#include "ldCore/Filter/ldHardwareFilter.h"

#include <cmath>

#include <QtCore/QDebug>

#include "ldCore/ldCore.h"
#include "ldCore/Filter/ldDeadzoneFilter.h"
#include "ldCore/Filter/ldFilterManager.h"
#include "ldCore/Filter/ldProjectionUtil.h"
#include "ldCore/Data/ldFrameBuffer.h"

ldHardwareFilter::ldHardwareFilter(ldScaleFilter *globalScaleFilter, QObject *parent)
    : QObject(parent)
    , m_borderFilter(new ldDeadzoneFilter())
    , m_colorCurveFilter(new ldColorCurveFilter())
    , m_deadzoneFilter(new ldDeadzoneFilter())
    , m_flipFilter(new ldFlipFilter())
    , m_powerFilter(new ldPowerFilter)
    , m_projectionBasic(new ldProjectionBasic)
    , m_scaleFilter(new ldScaleFilter())
    , m_shiftFilter(new ldShiftFilter(std::vector<ldScaleFilter*>{m_scaleFilter.get(), globalScaleFilter }))
    , m_ttlFilter(new ldTtlFilter())
{
    m_colorCurveFilter->m_enabled = true;

    // set up deadzone with default zone
    m_deadzoneFilter->resetToDefault();
    // set up border
    ldDeadzoneFilter::Deadzone deadzone;
    deadzone.setAttenuation(1);
    deadzone.setWidth(1.01f);
    deadzone.setHeight(1.01f);
    deadzone.moveLeft(0);
    deadzone.moveTop(0);
    m_borderFilter->add(deadzone);
    m_borderFilter->setEnabled(true);
    m_borderFilter->setReverse(true);

    // data for offset correction
    // init old[] buffer with zeroes
    ldVertex t;
    t.x() = t.y() = 0;
    t.clearColor();
    for (int i = 0; i < OFFSET_MAX; i++) old[i] = t;


    m_lastFrame.resize(ldFrameBuffer::FRAMEBUFFER_CAPACITY);
}

ldHardwareFilter::~ldHardwareFilter()
{
}

ldColorCurveFilter *ldHardwareFilter::baseColorCurveFilter() const
{
    return m_colorCurveFilter.get();
}

void ldHardwareFilter::processFrame(const ldVertexFrame &frame)
{
    m_lastFrame = frame;

    for(ldVertex &v : m_lastFrame.frame()) {
        ldCore::instance()->filterManager()->dataFilter()->processFilter(v);
        processFrameV(v);
    }

    m_deadzoneFilter->processFrame(m_lastFrame.frame());

    // before putting it into buffer we have to make minor polish
    for(ldVertex &v : m_lastFrame.frame())
        processSafeLaserOutput(v);
}

void ldHardwareFilter::processVertex(ldVertex &v)
{
    m_deadzoneFilter->processFilter(v);
    processSafeLaserOutput(v);
}

void ldHardwareFilter::processFrameV(ldVertex &v)
{
    bool mode_disable_scale = ldCore::instance()->filterManager()->dataFilter()->frameModes & FRAME_MODE_SKIP_TRANSFORM;

    if (!mode_disable_scale) {
        m_flipFilter->processFilter(v);
        m_scaleFilter->processFilter(v);
        m_shiftFilter->processFilter(v);
    }

    bool mode_disable_colorcorrection = ldCore::instance()->filterManager()->dataFilter()->frameModes & FRAME_MODE_DISABLE_COLOR_CORRECTION;
    if (!mode_disable_colorcorrection) {
        // ttl filter should be before color adjustments
        m_ttlFilter->processFilter(v);
        m_colorCurveFilter->process(v);
    }

    // keystone
    bool mode_disable_projection = ldCore::instance()->filterManager()->dataFilter()->frameModes & FRAME_MODE_SKIP_TRANSFORM;
    if (!mode_disable_projection) {
        if(!m_projectionBasic->isNullTransform()) {
            // map
            float x = v.x();
            float y = v.y();
            // correct for size
            x *= 1.0f/m_projectionBasic->maxdim();
            y *= 1.0f/m_projectionBasic->maxdim();
            // transform
            m_projectionBasic->transform(x, y);

            v.x() = x;
            v.y() = y;
        }
    }

    if(!mode_disable_colorcorrection)
        m_powerFilter->processFilter(v);

    // borders and clamp
    m_borderFilter->processFilter(v);
    v.x() = std::min<float>(std::max<float>(v.x(), -1), 1);
    v.y() = std::min<float>(std::max<float>(v.y(), -1), 1);
}


void ldHardwareFilter::resetFilter()
{
    m_deadzoneFilter->resetFilter();
}

ldVertexFrame &ldHardwareFilter::lastFrame()
{
    return m_lastFrame;
}

ldDeadzoneFilter *ldHardwareFilter::deadzone() const
{
    return m_deadzoneFilter.get();
}

ldFlipFilter *ldHardwareFilter::flipFilter()
{
    return m_flipFilter.get();
}

ldPowerFilter *ldHardwareFilter::powerFilter() const
{
    return m_powerFilter.get();
}


ldScaleFilter *ldHardwareFilter::scaleFilter() const
{
    return m_scaleFilter.get();
}

ldShiftFilter *ldHardwareFilter::shiftFilter() const
{
    return m_shiftFilter.get();
}

ldTtlFilter *ldHardwareFilter::ttlFilter() const
{
    return m_ttlFilter.get();
}

void ldHardwareFilter::setKeystoneX(float keystoneX)
{
    m_projectionBasic->setPitch(keystoneX);
}

void ldHardwareFilter::setKeystoneY(float keystoneY)
{
    m_projectionBasic->setYaw(keystoneY);
}

void ldHardwareFilter::setOffset(int offset)
{
    m_offset = offset;
    // perform offset
    if (m_offset < -OFFSET_MAX + 1) m_offset = -OFFSET_MAX + 1;
    if (m_offset >  OFFSET_MAX - 1) m_offset = OFFSET_MAX - 1;
}

void ldHardwareFilter::processSafeLaserOutput(ldVertex &v)
{
    bool mode_disable_overscan = ldCore::instance()->filterManager()->dataFilter()->frameModes & FRAME_MODE_UNSAFE_OVERSCAN;
    bool mode_disable_underscan = ldCore::instance()->filterManager()->dataFilter()->frameModes & FRAME_MODE_UNSAFE_UNDERSCAN;

    if (!galvo_libre) {

        // scan protection
        // overscan
        // quiet algorithm
        if (!mode_disable_overscan) {
            float maxDistance = overscan_speed; // (per point)
            float tx = v.x();
            float ty = v.y();
            float dx = tx - m_lastX1;
            float dy = ty - m_lastY1;
            float dist = sqrtf(dx*dx+dy*dy);
            if (!alternate_maxspeed) {
                // algorithm with a max 2d distance.  has fewer visual artifacts but is more limiting
                float mult = 1.0;
                if (dist > maxDistance) mult = maxDistance / dist;
                tx = m_lastX1 + dx*mult;
                ty = m_lastY1 + dy*mult;
            } else {
                // alternate algorithm treats x/y galvos independantly
                if (dx > maxDistance) dx = maxDistance;
                if (dx < -maxDistance) dx = -maxDistance;
                if (dy > maxDistance) dy = maxDistance;
                if (dy < -maxDistance) dy = -maxDistance;
                tx = m_lastX1 + dx;
                ty = m_lastY1 + dy;
            }
            v.x() = m_lastX1 = tx;
            v.y() = m_lastY1 = ty;

        }

        // underscan
        // restless algorithm
        // if laser is too slow it will draw a circle
        bool underscan = true;
        // skip if it's a black point
        if (v.isBlank()) underscan = false;
        if (underscan && !mode_disable_underscan)
        {
            float dx = v.x() - m_lastX2;
            float dy = v.y() - m_lastY2;
            float d = sqrtf(dx*dx+dy*dy);
            if (d > underscan_speed) {
                d = 1;
                m_lastX2 = v.x();
                m_lastY2 = v.y();
            } else {
                d = 0;
            }
            //if (v.b() == 0 && v.g() == 0 && v.b() == 0) d = 1;
            // with these constants, algo takes about 2ms of constant laser to activate
            m_averaged = 0.99*m_averaged + 0.01*d;
            if (d == 1) m_averaged = 0.98*m_averaged + 0.02*d;
            float r = 0;
            float mad = 0.4f; // at least 40% of points must be under
            if (d == 0 && m_averaged < mad) r = 0.125 * (mad-m_averaged)/mad;


            if (r > 0) {
                // move laser in a circle and fade out
                m_turns += 1.0/64.0;
                m_turns -= (int) m_turns;
                // set color
                float ff = fminf(fmaxf(1.0,((mad-m_averaged)/mad)*2-1), 1);
                v.r() *= 1-ff;
                v.g() *= 1-ff;
                v.b() *= 1-ff;
            }
        }
    }


    //
    // color/pos alignment offset
    // save in old buffer in praparation for performing offset
    for (int i = OFFSET_MAX - 1; i >= 1; i--)
        old[i] = old[i - 1];
    old[0] = v;

    // convert signed offset value to array indexes coloroffset and posoffset
    int co = 0;
    int po = 0;
    if (m_offset > 0) co = m_offset;
    if (m_offset < 0) po = -m_offset;
    // take values from old buffer depending on offsets
    for (int i = 0; i < ldVertex::COLOR_COUNT; i++) v.color[i] = old[co].color[i];
    for (int i = 0; i < ldVertex::POS_COUNT; i++) v.position[i] = old[po].position[i];
}
