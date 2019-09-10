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

ldHardwareManager::ldHardwareManager(QObject *parent)
    : QObject(parent)
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

void ldHardwareManager::setConnectedDevicesActive(bool active)
{
    for(ldAbstractHardwareManager *hardwareManager : m_hardwareManagers) {
        hardwareManager->setConnectedDevicesActive(active);
    }
}

void ldHardwareManager::addHardwareManager(ldAbstractHardwareManager *hardwareManager)
{
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
       deviceCount += hardwareManager->deviceCount();
    }
    setDeviceCount(deviceCount);
}
