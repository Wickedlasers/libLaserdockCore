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

#ifndef LDUSBHARDWAREMANAGER_H
#define LDUSBHARDWAREMANAGER_H

#include <memory>

#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QTimer>

#include "ldAbstractHardwareManager.h"

class ldFilterManager;
class ldUSBHardware;
struct ldCompressedSample;

typedef bool (*ldAuthenticateCallbackFunc)(ldUSBHardware *device);

class LDCORESHARED_EXPORT ldUsbHardwareManager : public ldAbstractHardwareManager
{
    Q_OBJECT

public:
    explicit ldUsbHardwareManager(ldFilterManager *filterManager, QObject *parent = 0);
    ~ldUsbHardwareManager();

    int getBufferFullCount();

    bool hasActiveUsbDevices() const;

    bool isDeviceActive(int index) const;
    void setDeviceActive(int index, bool active);

    void sendData(uint startIndex, uint count);

    virtual void setConnectedDevicesActive(bool active) override;

    virtual uint deviceCount() const override;
    virtual std::vector<ldHardware*> devices() const override;

public slots:
    void setAuthenticateFunc(ldAuthenticateCallbackFunc authenticateFunc);

private slots:
    void usbDeviceCheck();

private:
    void updateHardwareFilters();
    void updateCheckTimerState();

    mutable QMutex m_mutex;

    QTimer m_checkTimer;

    ldAuthenticateCallbackFunc m_authenticateCb = nullptr;

    std::vector<std::unique_ptr<ldUSBHardware> > m_usbHardwares;
    ldFilterManager *m_filterManager;
};

#endif // LDUSBHARDWAREMANAGER_H
