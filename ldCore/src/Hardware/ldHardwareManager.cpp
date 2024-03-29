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

#include "ldCore/Hardware/ldHardwareManager.h"

#include <QtCore/QDebug>

#include "ldCore/Hardware/ldAbstractHardwareManager.h"

ldHardwareManager::ldHardwareManager(ldFilterManager *filterManager, QObject *parent)
    : QObject(parent)
    , m_filterManager(filterManager)
{
    qDebug() << __FUNCTION__;
}


ldHardwareManager::~ldHardwareManager()
{
}

int ldHardwareManager::getDeviceCount() const
{
    return m_deviceCount;
}

std::vector<ldHardware*> ldHardwareManager::devices() const
{
    std::vector<ldHardware*> devices;
    for(ldAbstractHardwareManager *hardwareManager : m_hardwareManagers) {
        std::vector<ldHardware*> hwDevices = hardwareManager->devices();
        devices.insert(devices.end(), hwDevices.begin(), hwDevices.end());
    }
    return devices;
}

std::vector<ldAbstractHardwareManager *> ldHardwareManager::hardwareManagers() const
{
    return m_hardwareManagers;
}

ldFilterManager *ldHardwareManager::filterManager() const
{
    return m_filterManager;
}

void ldHardwareManager::setConnectedDevicesActive(bool active)
{
    for(ldAbstractHardwareManager *hardwareManager : m_hardwareManagers) {
        hardwareManager->setConnectedDevicesActive(active);
    }
}

void ldHardwareManager::setExplicitActiveDevice(int index)
{
    for(ldAbstractHardwareManager *hardwareManager : m_hardwareManagers) {
        hardwareManager->setExplicitActiveDevice(index);
    }
}

void ldHardwareManager::addHardwareManager(ldAbstractHardwareManager *hardwareManager)
{
    connect(hardwareManager, &ldAbstractHardwareManager::isActiveChanged, this, &ldHardwareManager::updateDeviceCount);
    connect(hardwareManager, &ldAbstractHardwareManager::deviceCountChanged, this, &ldHardwareManager::updateDeviceCount);

    m_hardwareManagers.push_back(hardwareManager);
}


void ldHardwareManager::setDeviceCount(uint deviceCount)
{
    if(m_deviceCount == deviceCount) {
        return;
    }

    m_deviceCount = deviceCount;

    emit deviceCountChanged(m_deviceCount);
}


void ldHardwareManager::updateDeviceCount()
{
    uint deviceCount = 0;
    for(const ldAbstractHardwareManager *hardwareManager : m_hardwareManagers) {
        if(hardwareManager->get_isActive())
            deviceCount += hardwareManager->deviceCount();
    }
    setDeviceCount(deviceCount);
}
