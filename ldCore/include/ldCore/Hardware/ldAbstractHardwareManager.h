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

#ifndef LDABSTRACTHARDWAREMANAGER_H
#define LDABSTRACTHARDWAREMANAGER_H

#include <QQmlHelpers>

#include <ldCore/ldCore_global.h>

class ldHardware;

class LDCORESHARED_EXPORT ldAbstractHardwareManager : public QObject
{
    Q_OBJECT

    QML_WRITABLE_PROPERTY(bool, isActive)

public:
    explicit ldAbstractHardwareManager(QObject *parent = nullptr);

    virtual uint deviceCount() const = 0;
    virtual std::vector<ldHardware*> devices() const = 0;

    virtual int getBufferFullCount() {return 0;}

    virtual int getSmallestBufferCount() {return 0;}
    virtual int getLargestBufferCount() {return 0;}

    virtual bool hasActiveDevices() const {return false;}

    virtual void sendData(uint startIndex, uint count) = 0;

    struct DeviceBufferConfig{
        uint samples_per_packet; // how many samples to send
        int remote_buffer_cutoff; // this sets the buffer point at which we stop sending to remote device
        int wait_connect_sleep_ms;
        int wait_buffer_sleep_ms;
        int sleep_after_packet_send_ms; // how long to sleep thread for after sending data to a device (in ms)
        uint max_samples_per_udp_xfer; // max number of samples to transmit over any single UDP transer (set to 0 for USB devices)
        uint max_udp_packets_per_xfer;
    } ;


    virtual DeviceBufferConfig getBufferConfig() {
        return m_default_device_config;
    } // get the buffer config for the hardware device(s)

    virtual void debugAddDevice() = 0;
    virtual void debugRemoveDevice() = 0;
    virtual QString managerName() const = 0;

protected:
    DeviceBufferConfig m_default_device_config{768,1024,12,6,0,0,0}; // original values for USB cube

public slots:
    virtual void setConnectedDevicesActive(bool active) = 0;
    void setExplicitActiveDevice(int index);
    virtual void setActiveTransfer(bool active) = 0;

signals:
    void deviceCountChanged(uint deviceCount);

protected:
    int m_explicitHardwareIndex = -1;

};


#endif // LDABSTRACTHARDWAREMANAGER_H
