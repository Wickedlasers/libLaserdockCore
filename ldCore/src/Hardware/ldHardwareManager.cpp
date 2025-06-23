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
#include <QtCore/QCoreApplication>

#include "ldCore/Hardware/ldAbstractHardwareManager.h"
#include "ldCore/Hardware/ldHardware.h"


ldHardwareManager::ldHardwareManager(QObject *parent)
    : QObject(parent)
    , m_isActive(false)
    , m_deviceCheckTimer(new QTimer(this))
{
    qDebug() << __FUNCTION__;

    connect(this, &ldHardwareManager::isActiveChanged, this, &ldHardwareManager::updateDeviceCount);

    connect(qApp, &QCoreApplication::aboutToQuit, [&]() {
        set_isActive(false);
    });

    m_deviceCheckTimer->setInterval(500);
    connect(m_deviceCheckTimer, &QTimer::timeout, this, &ldHardwareManager::checkDevices);

    QTimer::singleShot(0, this, &ldHardwareManager::initCheckTimer);
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

void ldHardwareManager::setForcedDACRate(int rate)
{
    if(m_forcedDACRate == rate)
        return;

    m_forcedDACRate = rate;

    emit forcedDacRateChanged(rate);
}

int ldHardwareManager::getForcedDACRate() const
{
    return m_forcedDACRate;
}

void ldHardwareManager::setConnectedDevicesActive(bool active)
{
    for(ldAbstractHardwareManager *hardwareManager : m_hardwareManagers) {
        hardwareManager->setConnectedDevicesActive(active);
    }

    QTimer::singleShot(0, this, &ldHardwareManager::updateCheckTimerState);
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
    // not sure why it was required but we shouldn't change device count because of active state
    // if(!get_isActive()) {
    //     setDeviceCount(0);
    //     return;
    // }

    uint deviceCount = 0;
    for(const ldAbstractHardwareManager *hardwareManager : m_hardwareManagers)
        deviceCount += hardwareManager->deviceCount();

    setDeviceCount(deviceCount);
}


void ldHardwareManager::initCheckTimer()
{
    updateCheckTimerState();
    connect(this, &ldHardwareManager::isActiveChanged, this, &ldHardwareManager::updateCheckTimerState, Qt::QueuedConnection);
    connect(this, &ldHardwareManager::deviceCountChanged, this, &ldHardwareManager::updateCheckTimerState, Qt::QueuedConnection);
}

void ldHardwareManager::removeDevice(ldHardware *hw)
{
    for(ldAbstractHardwareManager *hardwareManager : m_hardwareManagers) {
        if(hardwareManager->hwType() != hw->hwType())
            continue;

        QMetaObject::invokeMethod(hardwareManager, "removeDevice", Qt::BlockingQueuedConnection, Q_ARG(QString, hw->id()));
    }
}

void ldHardwareManager::checkDevices()
{
    // qDebug() << __FUNCTION__;

    for(ldAbstractHardwareManager *hardwareManager : m_hardwareManagers)
        QMetaObject::invokeMethod(hardwareManager, &ldAbstractHardwareManager::deviceCheck, Qt::QueuedConnection);
        // hardwareManager->deviceCheck();
}


void ldHardwareManager::updateCheckTimerState()
{
    // QMutexLocker locker(&m_mutex);

    bool hasActiveHws = false;

    if (get_isActive()) {
        for(ldHardware *hardware : devices()) {
            if(hardware->isActive()) {
                qDebug() << __FUNCTION__ << "active device found" << hardware->address();
                hasActiveHws = true;
                break;
            }
        }
    }

    qDebug() << __FUNCTION__ << hasActiveHws << m_deviceCheckTimer->isActive();

    if(hasActiveHws) {
        if(m_deviceCheckTimer->isActive()) {
            QTimer::singleShot(0, m_deviceCheckTimer, &QTimer::stop);
        }
    } else {
        if(!m_deviceCheckTimer->isActive()) {
            QTimer::singleShot(0, m_deviceCheckTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
            QTimer::singleShot(0, this, &ldHardwareManager::checkDevices);
        }
    }
}
