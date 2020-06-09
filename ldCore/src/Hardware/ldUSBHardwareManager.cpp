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

#include "ldCore/Hardware/ldUSBHardwareManager.h"

#include <QtCore/QtDebug>

#ifdef LD_CORE_ENABLE_LASERDOCKLIB
#include <laserdocklib/LaserdockDevice.h>
#include <laserdocklib/LaserdockDeviceManager.h>
#include <laserdocklib/LaserdockSample.h>
#endif

#include <ldCore/Filter/ldFilterManager.h>
#include <ldCore/Hardware/ldUSBHardware.h>

ldUsbHardwareManager::ldUsbHardwareManager(ldFilterManager *filterManager, QObject *parent)
    : ldAbstractHardwareManager(parent)
    , m_filterManager(filterManager)
{
    connect(this, &ldUsbHardwareManager::deviceCountChanged, this, &ldUsbHardwareManager::updateCheckTimerState);
    connect(&m_checkTimer, &QTimer::timeout, this, &ldUsbHardwareManager::usbDeviceCheck);
    m_checkTimer.setInterval(1000);

    updateCheckTimerState();
}

ldUsbHardwareManager::~ldUsbHardwareManager()
{
}

int ldUsbHardwareManager::getBufferFullCount()
{
    QMutexLocker locker(&m_mutex);

    int fullCount = -1;

    if(!m_usbHardwares.empty()) {
        fullCount = m_usbHardwares[0]->get_full_count(); // TODO Probably we should manager buffer size for each device separately
        if(fullCount == -1) {
            qDebug() << "Remove disconnected device on get_full_count" << QString::fromStdString(m_usbHardwares[0]->params().serial_number);
            m_usbHardwares.erase(m_usbHardwares.begin());
            updateHardwareFilters();
            emit deviceCountChanged(m_usbHardwares.size());
        }
    }

    return fullCount;
}

void ldUsbHardwareManager::sendData(uint startIndex, uint count)
{
    QMutexLocker locker(&m_mutex);

    if (m_usbHardwares.empty()) {
        return;
    }

    if(m_explicitHardwareIndex >= 0) {
        uint hwIndex =  static_cast<uint>(m_explicitHardwareIndex);
        if(hwIndex < m_usbHardwares.size()) {
            ldUSBHardware *hardware = m_usbHardwares[hwIndex].get();
            bool b = hardware->send_samples(startIndex, count);
            if (!b) {
                // erase disconnected device
                qDebug() << "Remove disconnected device on send_samples" << QString::fromStdString(hardware->params().serial_number);
                m_usbHardwares.erase(m_usbHardwares.begin() + hwIndex);
                m_explicitHardwareIndex = -1;
                updateHardwareFilters();
                emit deviceCountChanged(m_usbHardwares.size());
            }
            return;
        }
    }

    auto usbHardwareIt = m_usbHardwares.begin();
    while(usbHardwareIt != m_usbHardwares.end()) {
        ldUSBHardware *usbHardware = (*usbHardwareIt).get();
        // we should explicitly activate devices before usage
        if(!usbHardware->isActive()) {
            usbHardwareIt++;
            continue;
        }

        bool b = usbHardware->send_samples(startIndex, count);
        if (!b) {
            // erase disconnected device
            qDebug() << "Remove disconnected device on send_samples" << QString::fromStdString(usbHardware->params().serial_number);
            usbHardwareIt = m_usbHardwares.erase(usbHardwareIt);
            updateHardwareFilters();
            emit deviceCountChanged(m_usbHardwares.size());
        } else {
            usbHardwareIt++;
        }
    }

}

void ldUsbHardwareManager::setConnectedDevicesActive(bool active)
{
    for(std::unique_ptr<ldUSBHardware> &usbHardware : m_usbHardwares) {
        usbHardware->setActive(active);
    }

    updateCheckTimerState();
}

uint ldUsbHardwareManager::deviceCount() const
{
    return m_usbHardwares.size();
}

std::vector<ldHardware*> ldUsbHardwareManager::devices() const
{
    std::vector<ldHardware*> devices;
    for(const std::unique_ptr<ldUSBHardware> &usbHardware : m_usbHardwares)
        devices.push_back(usbHardware.get());
    return devices;
}

bool ldUsbHardwareManager::isDeviceActive(int index) const
{
    if(index < 0 || index >= (int) m_usbHardwares.size()) {
        return false;
    }

    const std::unique_ptr<ldUSBHardware> &usbHardware = m_usbHardwares[index];
    return usbHardware->isActive();
}

void ldUsbHardwareManager::setDeviceActive(int index, bool active)
{
    if(index < 0 || index >= (int) m_usbHardwares.size()) {
        return;
    }

    std::unique_ptr<ldUSBHardware> &usbHardware = m_usbHardwares[index];
    usbHardware->setActive(active);

    updateCheckTimerState();
}

bool ldUsbHardwareManager::hasActiveUsbDevices() const
{
    auto it = std::find_if(m_usbHardwares.begin(), m_usbHardwares.end(),
                           [&](const std::unique_ptr<ldUSBHardware> &networkHardware) {
            return (networkHardware->isActive() && networkHardware->status() == ldHardware::Status::INITIALIZED);
    });

    return it != m_usbHardwares.end();
}

void ldUsbHardwareManager::setAuthenticateFunc(ldAuthenticateCallbackFunc authenticateFunc)
{
    m_authenticateCb = authenticateFunc;
}


void ldUsbHardwareManager::usbDeviceCheck()
{
    QMutexLocker locker(&m_mutex);

//    qDebug() << __FUNCTION__;

#ifdef LD_CORE_ENABLE_LASERDOCKLIB
    uint oldDeviceCount = m_usbHardwares.size();
    // check for disconnected devices
    auto usbHardwareIt = m_usbHardwares.begin();
    while(usbHardwareIt != m_usbHardwares.end()) {
        std::unique_ptr<ldUSBHardware> &usbHardware = (*usbHardwareIt);

        // do not check active devices
        if(usbHardware->isActive()) {
            usbHardwareIt++;
            continue;
        }

        // ping each device
        bool pinged = false;
        if (usbHardware->status() == ldHardware::Status::INITIALIZED
                && usbHardware->params().device->status() == LaserdockDevice::Status::INITIALIZED) {
            usbHardware->params().device->get_output(&pinged);
        }
        // device was disconnected
        if (!pinged) {
            qDebug() << "Remove disconnected device on get_output" << QString::fromStdString(usbHardware->params().serial_number);
            usbHardwareIt = m_usbHardwares.erase(usbHardwareIt);
            updateHardwareFilters();
        } else {
            usbHardwareIt++;
        }
    }

    std::vector<LaserdockDevice*> existingDevices;
    for(const std::unique_ptr<ldUSBHardware> &hardware : m_usbHardwares) {
        existingDevices.push_back(hardware->params().device);
    }

    // check for new devices
    std::vector<std::unique_ptr<LaserdockDevice> > newDevices = LaserdockDeviceManager::getInstance().get_laserdock_devices(existingDevices);
    for(std::unique_ptr<LaserdockDevice> &newDevice : newDevices) {
        // initialize device
        std::unique_ptr<ldUSBHardware> usbHardware(new ldUSBHardware(newDevice.release()));

        if(usbHardware->status() != ldHardware::Status::INITIALIZED) {
            qWarning() << "Can't initialize LaserDock unit";
            continue;
        }

        // check authentication if available
        if(m_authenticateCb
                && !m_authenticateCb(usbHardware.get())
                ) {
            qWarning() << "Can't authenticate LaserDock unit";
            continue;
        }

        // add to list
        m_usbHardwares.push_back(std::move(usbHardware));
        updateHardwareFilters();
    }

    if(oldDeviceCount != m_usbHardwares.size()) {
        emit deviceCountChanged(m_usbHardwares.size());
    }
#endif
}

void ldUsbHardwareManager::updateHardwareFilters()
{
    for(uint i = 0; i < m_usbHardwares.size(); i++) {
        auto &usbHardware = m_usbHardwares[i];
        if(i == 0)
            usbHardware->setFilter(m_filterManager->hardwareFilter());
        else
            usbHardware->setFilter(m_filterManager->hardwareFilter2());
    }
}

// check for new devices always when we are not active
// and do not check if active because we don't want to have flickering
void ldUsbHardwareManager::updateCheckTimerState()
{
    bool isActive = false;
    for(std::unique_ptr<ldUSBHardware> &usbHardware : m_usbHardwares) {
        if(usbHardware->isActive()) {
            isActive = true;
            break;
        }
    }
    if(isActive) {
        QTimer::singleShot(0, &m_checkTimer, &QTimer::stop);
    } else {
        QTimer::singleShot(0, &m_checkTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
        QTimer::singleShot(100, this, &ldUsbHardwareManager::usbDeviceCheck);
    }
}

