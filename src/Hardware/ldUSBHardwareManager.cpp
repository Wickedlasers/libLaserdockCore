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

#include <laserdocklib/LaserdockDevice.h>
#include <laserdocklib/LaserdockDeviceManager.h>
#include <laserdocklib/LaserdockSample.h>

#include "ldCore/Hardware/ldUSBHardware.h"

ldUsbHardwareManager::ldUsbHardwareManager(QObject *parent)
    : ldAbstractHardwareManager(parent)
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
            m_usbHardwares.erase(m_usbHardwares.begin());
            emit deviceCountChanged(m_usbHardwares.size());
        }
    }

    return fullCount;
}

void ldUsbHardwareManager::sendData(CompressedSample *samples, unsigned int count)
{
    QMutexLocker locker(&m_mutex);

    if (m_usbHardwares.empty()) {
        return;
    }

    auto usbHardwareIt = m_usbHardwares.begin();
    while(usbHardwareIt != m_usbHardwares.end()) {
        ldUSBHardware *usbHardware = (*usbHardwareIt).get();
        // we should explicitly activate devices before usage
        if(!usbHardware->isActive()) {
            usbHardwareIt++;
            continue;
        }

        bool b = usbHardware->send_samples((LaserdockSample * ) samples, count);
        if (!b) {
            // erase disconnected device
            usbHardwareIt = m_usbHardwares.erase(usbHardwareIt);
            emit deviceCountChanged(m_usbHardwares.size());
        } else {
            usbHardwareIt++;
        }

        // flip each next sample
        for(uint i = 0; i < count; i++){
            CompressedSample &sample = samples[i];
            sample.x = CompressedSample::flipCoord(sample.x);
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
            usbHardwareIt = m_usbHardwares.erase(usbHardwareIt);
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

        // check authentication if available
        if(m_authenticateCb
                && !m_authenticateCb(usbHardware.get())
                ) {
            qWarning() << "Can't authenticate LaserDock unit";
            continue;
        }

        // add to list
        m_usbHardwares.push_back(std::move(usbHardware));
    }

    if(oldDeviceCount != m_usbHardwares.size()) {
        emit deviceCountChanged(m_usbHardwares.size());
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
        QTimer::singleShot(0, this, &ldUsbHardwareManager::usbDeviceCheck);
    }
}

