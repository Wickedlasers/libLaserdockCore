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
    , m_3dRotateFilter(new ld3dRotateFilter())
    , m_movementFilter(new ldFilterMovement())
{
    qDebug() << __FUNCTION__;
}

ldFilterManager::~ldFilterManager()
{
}

// get existing filter by device id, or allocate a new one if a new device
// if no id is provided, return a default hardware filter
ldHardwareFilter *ldFilterManager::getFilterById(QString device_id)
{
    if (device_id=="") { // default id?
       qDebug() << "get hardware filter for unknown device";
       return nullptr;
    } else
    if (m_filtermap.contains(device_id)) { // filter already created for this device id?
        qDebug() << "get existing filter for device:" << device_id;
        return m_filtermap[device_id].get();
    } else { // allocate a filter for this device id
        qDebug() << "allocating hardware filter for device:" << device_id;
        QSharedPointer<ldHardwareFilter> hw(new ldHardwareFilter(m_dataFilter.scaleFilter()));
        //qDebug() << "hardware filter allocated";
        //m_hardwareFilters.push_back(std::move(hw));
       // m_filtermap[device_id] = hw.get();
        m_filtermap[device_id] = hw;
        return m_filtermap[device_id].get();
    }

}

// remove an existing filter by device id
void ldFilterManager::removeFilterById(QString /*device_id*/)
{
    // disabled removing of a filter so that it is still available if the unit is switched off then on.
    /*
    lock();
    if (m_filtermap.contains(device_id)) { // filter already created for this device id?
        qDebug() << "remove hardware filter for device:" << device_id;
        m_filtermap.remove(device_id);
    } else {
        qDebug() << "unable to remove hardware filter for unknown device";
    }
    unlock();
    */
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


void ldFilterManager::setFrameModes(int frameModes)
{
    m_dataFilter.frameModes = frameModes;
}

void ldFilterManager::processFrame(ldVertexFrame &frame)
{
    for(uint i = 0; i < frame.size(); i++) {
        if(m_preGlobalFilter)
            m_preGlobalFilter->processFilter(frame[i]);

        m_movementFilter->processFilter(frame[i]);

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

ldFilterMovement *ldFilterManager::movementFilter() const
{
    return m_movementFilter.get();
}

ld3dRotateFilter *ldFilterManager::rotate3dFilter() const
{
    return m_3dRotateFilter.get();
}

ldScaleFilter *ldFilterManager::globalScaleFilter() const
{
    return m_dataFilter.scaleFilter();
}

ldPowerFilter *ldFilterManager::globalPowerFilter() const
{
    return m_dataFilter.powerFilter();
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

void ldFilterManager::lock()
{
    m_mutex.lock();
}

void ldFilterManager::unlock()
{
    m_mutex.unlock();
}
