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

#include "ldCore/Helpers/ldLaserController.h"

#include <QtCore/QtDebug>
#include <QtGui/QGuiApplication>

#include "ldCore/ldCore.h"
#include <ldCore/Hardware/ldAbstractHardwareManager.h>
#include <ldCore/Hardware/ldHardwareManager.h>
#include <ldCore/Data/ldDataDispatcherManager.h>

ldLaserController::ldLaserController(QObject *parent)
    : QObject(parent)
    , m_connectedDevices(0)
    , m_isActive(false)
{
    qDebug() << __FUNCTION__;

    connect(ldCore::instance()->hardwareManager(), &ldHardwareManager::deviceCountChanged, this, &ldLaserController::onHardwareDeviceCountChanged);
    connect(ldCore::instance()->get_dataDispatcherManager(), &ldDataDispatcherManager::activeXferChanged, this, &ldLaserController::refreshPlayState);

    refreshPlayState();
    refreshDeviceState();
}


void ldLaserController::togglePlay()
{
    bool isActive = !m_isActive;

    qDebug() << "Laser output " << isActive;

    ldCore::instance()->hardwareManager()->setConnectedDevicesActive(isActive);
    ldCore::instance()->get_dataDispatcherManager()->setActiveTransfer(isActive);
}

void ldLaserController::debugAddDevice()
{
    ldAbstractHardwareManager *absHwMan = nullptr;
    for(ldAbstractHardwareManager *hwm : ldCore::instance()->hardwareManager()->hardwareManagers()) {
        absHwMan = hwm;
        break;
    }

    if(absHwMan)
        absHwMan->debugAddDevice();

    qDebug() << __FUNCTION__ << m_connectedDevices;
}

void ldLaserController::debugRemoveDevice()
{
    ldAbstractHardwareManager *absHwMan = nullptr;
    for(ldAbstractHardwareManager *hwm : ldCore::instance()->hardwareManager()->hardwareManagers()) {
        absHwMan = hwm;
        break;
    }

    if(absHwMan)
        absHwMan->debugRemoveDevice();

    qDebug() << __FUNCTION__ << m_connectedDevices;
}

void ldLaserController::refreshDeviceState()
{
    update_connectedDevices(ldCore::instance()->hardwareManager()->getDeviceCount());
}

void ldLaserController::refreshPlayState()
{
    update_isActive(ldCore::instance()->get_dataDispatcherManager()->isActiveTransfer());
}

void ldLaserController::onHardwareDeviceCountChanged(uint /*count*/)
{
    refreshDeviceState();
}
