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

#include "ldLaserController.h"

#include <QtCore/QtDebug>
#include <QtCore/QTimer>

#include "ldCore/ldCore.h"
#include <ldCore/Hardware/ldHardwareManager.h>
#include <ldCore/Data/ldDataDispatcher.h>

ldLaserController::ldLaserController(QObject *parent)
    : QObject(parent)
    , m_connectedDevices(0)
    , m_isActive(false)
{
    qDebug() << __FUNCTION__;

    connect(ldCore::instance()->hardwareManager(), &ldHardwareManager::deviceCountChanged, this, &ldLaserController::onHardwareDeviceCountChanged);
    connect(ldCore::instance()->dataDispatcher(), &ldDataDispatcher::activeChanged, this, &ldLaserController::refreshPlayState);

    refreshPlayState();
    refreshDeviceState();

#ifdef INCLUDE_SERGEY_AT_WORK
    if(ldCore::isDebugMode()) {
        QTimer::singleShot(0, this, &ldLaserController::togglePlay);
    }
#endif
}


void ldLaserController::togglePlay()
{
    bool isActive = !m_isActive;

    qDebug() << "Laser output " << isActive;

    ldCore::instance()->hardwareManager()->setConnectedDevicesActive(isActive);
    ldCore::instance()->dataDispatcher()->setActiveTransfer(isActive);
}

void ldLaserController::refreshDeviceState()
{
    update_connectedDevices(ldCore::instance()->hardwareManager()->getDeviceCount());
}

void ldLaserController::refreshPlayState()
{
    update_isActive(ldCore::instance()->dataDispatcher()->isActiveTransfer());
}

void ldLaserController::onHardwareDeviceCountChanged(uint /*count*/)
{
    refreshDeviceState();
}
