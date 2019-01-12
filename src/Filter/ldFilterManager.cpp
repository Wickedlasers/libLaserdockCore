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

#include "ldCore/Filter/ldFilterManager.h"

#include <math.h>

#include <QtCore/QDebug>

ldFilterManager::ldFilterManager(QObject *parent)
    : QObject(parent)
    , m_flipFilter(new ldFlipFilter())
    , m_rotateFilter(new ldRotateFilter())
{
    qDebug() << __FUNCTION__;
    m_dataFilter.addFilter(m_flipFilter.get());
    m_dataFilter.addFilter(m_rotateFilter.get());
}

void ldFilterManager::setGlobalFilter(ldFilter *globalFilter)
{
    m_globalFilter = globalFilter;
}

void ldFilterManager::setFrameModes(int frameModes)
{
    m_dataFilter.frameModes = frameModes;
}

void ldFilterManager::process(Vertex &tval, Vertex &simVal)
{
    m_basicGlobalFilter.process(tval);

    // apply global filter to simulator output
    if (m_globalFilter)
        m_globalFilter->process(tval);

    // store similator value
    simVal = tval;

    // apply data filter to data output
    // give filter proper settings
    m_dataFilter.process(tval);
}

ldColorCurveFilter *ldFilterManager::baseColorCurveFilter() const
{
    return m_dataFilter.colorCurveFilter();
}

ldColorCurveFilter *ldFilterManager::colorCurveFilter() const
{
    return m_basicGlobalFilter.colorCurveFilter();
}

ldDeadzoneFilter *ldFilterManager::deadzoneFilter() const
{
    return m_dataFilter.deadzone();
}

ldHueFilter *ldFilterManager::hueFilter() const
{
    return m_basicGlobalFilter.hueFilter();
}

ldHueShiftFilter *ldFilterManager::hueShiftFilter() const
{
    return m_basicGlobalFilter.hueShiftFilter();
}

ldFlipFilter *ldFilterManager::flipFilter() const
{
    return m_flipFilter.get();
}

ldScaleFilter *ldFilterManager::scaleFilter() const
{
    return m_dataFilter.scaleFilter();
}

ldShiftFilter *ldFilterManager::shiftFilter() const
{
    return m_dataFilter.shiftFilter();
}

ldSoundLevelFilter *ldFilterManager::soundLevelFilter() const
{
    return m_basicGlobalFilter.soundLevelFilter();
}

ldRotateFilter *ldFilterManager::rotateFilter() const
{
    return m_rotateFilter.get();
}

ldTracerFilter *ldFilterManager::tracerFilter() const
{
    return m_basicGlobalFilter.tracerFilter();
}

void ldFilterManager::setBrightness(float brightness)
{
    m_dataFilter.m_brightness = brightness;
}

void ldFilterManager::setKeystoneX(float keystoneX)
{
    m_dataFilter.setKeystoneX(keystoneX);
}

void ldFilterManager::setKeystoneY(float keystoneY)
{
    m_dataFilter.setKeystoneY(keystoneY);
}

void ldFilterManager::setOffset(int offset)
{
    m_dataFilter.m_offset = offset;
}

void ldFilterManager::setTtl(bool isTtl)
{
    m_basicGlobalFilter.ttl = isTtl;
}
