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

#include <thread>

#include <QtCore/QtDebug>

#ifdef LASERDOCKLIB_USB_SUPPORT
#include <laserdocklib/LaserdockDevice.h>
#include <laserdocklib/LaserdockDeviceManager.h>
#include <laserdocklib/LaserdockSample.h>
#endif

#include <ldCore/Filter/ldFilterManager.h>
#include <ldCore/Hardware/ldUSBHardware.h>

ldGenerateSecurityRequestCallbackFunc ldUsbHardwareManager::m_genSecReqCb = nullptr;
ldAuthenticateSecurityResponseCallbackFunc ldUsbHardwareManager::m_authSecRespCb = nullptr;

ldUsbHardwareManager::ldUsbHardwareManager(ldFilterManager *filterManager, QObject *parent)
    : ldAbstractHardwareManager(parent)
    , m_filterManager(filterManager)
{
}

ldUsbHardwareManager::~ldUsbHardwareManager()
{
}

QString ldUsbHardwareManager::hwType() const
{
    return "USB";
}

void ldUsbHardwareManager::setConnectedDevicesActive(bool active)
{
    QMutexLocker locker(&m_mutex);
    for(std::unique_ptr<ldUSBHardware> &usbHardware : m_usbHardwares) {
        usbHardware->setActive(active);
    }

}

uint ldUsbHardwareManager::deviceCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_usbHardwares.size();
}

std::vector<ldHardware*> ldUsbHardwareManager::devices() const
{
    QMutexLocker locker(&m_mutex);
    std::vector<ldHardware*> devices;
    for(const std::unique_ptr<ldUSBHardware> &usbHardware : m_usbHardwares)
        devices.push_back(usbHardware.get());
    return devices;
}

void ldUsbHardwareManager::debugAddDevice()
{
    qWarning() << "not implemented yet";
    return;

     // FIXME: known crash
//    std::unique_ptr<LaserdockDevice> newDevice(new LaserdockDevice(nullptr));
//    std::unique_ptr<ldUSBHardware> usbHardware(new ldUSBHardware(newDevice.release()));
//    m_usbHardwares.push_back(std::move(usbHardware)); // move device from init list to the main list
//    emit deviceCountChanged(static_cast<uint>(m_usbHardwares.size()));
}

void ldUsbHardwareManager::debugRemoveDevice()
{
    QMutexLocker locker(&m_mutex);
    removeDeviceImpl(m_usbHardwares.size() - 1);
}

void ldUsbHardwareManager::setGenerateSecurityRequestCb(ldGenerateSecurityRequestCallbackFunc authenticateFunc)
{
    m_genSecReqCb = authenticateFunc;
}

void ldUsbHardwareManager::setAuthenticateSecurityCb(ldAuthenticateSecurityResponseCallbackFunc checkFunc)
{
    m_authSecRespCb = checkFunc;
}

void ldUsbHardwareManager::deviceCheck()
{
    QMutexLocker locker(&m_mutex);

    //qDebug() << "Hello";
    // qDebug() << __FUNCTION__;

#ifdef LASERDOCKLIB_USB_SUPPORT
    uint oldDeviceCount = m_usbHardwares.size();

    //qDebug() << "devices:" << oldDeviceCount;
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

        QByteArray reqByteArray;
        if(m_genSecReqCb) {
            reqByteArray = m_genSecReqCb();
        } else {
            uint8_t security_req_pkt[]={
                0x01,
                0xe0, 0x2e, 0x00, 0x00, 0x40, 0x9c, 0x00,
                0x00, 0x23, 0x27, 0x08, 0x00, 0x00, 0x00, 0xa1,
                0x21, 0x00, 0x00, 0xea, 0x35, 0x00, 0x00, 0x75,
                0x4f, 0x00, 0x00, 0x90, 0x1f, 0x00, 0x00, 0x40,
                0x39, 0x00, 0x00, 0x9c, 0x6d, 0x00, 0x00, 0xf2,
                0x2d, 0x00, 0x00, 0xa2, 0x6f, 0x00, 0x00, 0x73,
                0xc4
            };

            reqByteArray = QByteArray(reinterpret_cast<const char*>(security_req_pkt),sizeof(security_req_pkt));
        }

        bool ok = usbHardware->send_security_requst(reqByteArray);
        if(!ok) {
            qWarning() << "Can't send security request";
            continue;
        }


        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // fixed period to get security response

        QByteArray responseByteArray;
        usbHardware->get_security_response(responseByteArray);
        if(!ok) {
            qWarning() << "Can't get security response";
            continue;
        }

        if(m_authSecRespCb && !m_authSecRespCb(responseByteArray)) {
            qWarning() << "Can't authenticate LaserDock unit";
            continue;
        } else {
            qDebug() << "Device Authenication Successful.";
        }


        usbHardware->setFilter(m_filterManager->getFilterById(usbHardware->id()));

        connect(usbHardware.get(),&ldHardware::enabledChanged,this,&ldAbstractHardwareManager::deviceEnabledStateChanged);
        // add to list
        m_usbHardwares.push_back(std::move(usbHardware));
    }

    if(oldDeviceCount != m_usbHardwares.size()) {
        emit deviceCountChanged(m_usbHardwares.size());
    }
#endif
}

void ldUsbHardwareManager::removeDevice(const QString &hwId)
{
    QMutexLocker locker(&m_mutex);

    auto usbHardwareIt = m_usbHardwares.begin();
    while(usbHardwareIt != m_usbHardwares.end()) {
        std::unique_ptr<ldUSBHardware> &usbHardware = (*usbHardwareIt);
        if(usbHardware->id() == hwId) {
            m_usbHardwares.erase(usbHardwareIt);
            emit deviceCountChanged(m_usbHardwares.size());
            return;
        }

        usbHardwareIt++;
    }
}

void ldUsbHardwareManager::removeDeviceImpl(uint index)
{
    m_usbHardwares.erase(m_usbHardwares.begin() + index);
    emit deviceCountChanged(m_usbHardwares.size());
}

