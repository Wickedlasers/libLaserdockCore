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

#include <cmath>

#include <QtCore/QDebug>

#include <ldCore/Filter/ldHardwareFilter.h>

ldFilterManager::ldFilterManager(QObject *parent)
    : QObject(parent)
    , m_hardwareFilter(new ldHardwareFilter(m_dataFilter.scaleFilter()))
    , m_hardwareFilter2(new ldHardwareFilter(m_dataFilter.scaleFilter()))
    , m_3dRotateFilter(new ld3dRotateFilter())
{
    qDebug() << __FUNCTION__;
}

ldFilterManager::~ldFilterManager()
{
}

ldFilter *ldFilterManager::preGlobalFilter() const
{
    return m_preGlobalFilter;
}

void ldFilterManager::setPreGlobalFilter(ldFilter *globalFilter)
{
    m_preGlobalFilter = globalFilter;
}

ldFilter *ldFilterManager::globalFilter() const
{
    return m_globalFilter;
}

void ldFilterManager::setGlobalFilter(ldFilter *globalFilter)
{
    m_globalFilter = globalFilter;
}

ldFilterBasicData *ldFilterManager::dataFilter()
{
    return &m_dataFilter;
}

ldHardwareFilter *ldFilterManager::hardwareFilter()
{
    return m_hardwareFilter.get();
}

ldHardwareFilter *ldFilterManager::hardwareFilter2()
{
    return m_hardwareFilter2.get();
}

void ldFilterManager::setFrameModes(int frameModes)
{
    m_dataFilter.frameModes = frameModes;
}

void ldFilterManager::processFrame(ldVertexFrame &frame)
{
    for(uint i = 0; i < frame.size(); i++) {
        if(m_preGlobalFilter)
            m_preGlobalFilter->processFilter(frame[i]);

        if (m_globalFilter)
            m_globalFilter->processFilter(frame[i]);

        m_basicGlobalFilter.processFilter(frame[i]);
    }
}

void ldFilterManager::resetFilters()
{
    m_basicGlobalFilter.resetFilter();
    if (m_globalFilter)
        m_globalFilter->resetFilter();
    m_dataFilter.resetFilter();
}

ldColorCurveFilter *ldFilterManager::colorCurveFilter() const
{
    return m_basicGlobalFilter.colorCurveFilter();
}

ldColorFaderFilter *ldFilterManager::colorFaderFilter() const
{
    return m_basicGlobalFilter.colorFaderFilter();
}

ldStrobeFilter *ldFilterManager::strobeFilter() const
{
    return m_basicGlobalFilter.strobeFilter();
}

ldHueFilter *ldFilterManager::hueFilter() const
{
    return m_basicGlobalFilter.hueFilter();
}

ldHueShiftFilter *ldFilterManager::hueShiftFilter() const
{
    return m_basicGlobalFilter.hueShiftFilter();
}

ld3dRotateFilter *ldFilterManager::rotate3dFilter() const
{
    return m_3dRotateFilter.get();
}

ldScaleFilter *ldFilterManager::globalScaleFilter() const
{
    return m_dataFilter.scaleFilter();
}

ldSoundLevelFilter *ldFilterManager::soundLevelFilter() const
{
    return m_basicGlobalFilter.soundLevelFilter();
}

ldRotateFilter *ldFilterManager::rotateFilter() const
{
    return m_dataFilter.rotateFilter();
}

ldTracerFilter *ldFilterManager::tracerFilter() const
{
    return m_basicGlobalFilter.tracerFilter();
}

void ldFilterManager::setHueFiltersActive(bool active)
{
    m_basicGlobalFilter.setHueFiltersActive(active);
}
