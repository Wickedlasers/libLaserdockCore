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

typedef QByteArray (*ldGenerateSecurityRequestCallbackFunc)();
typedef bool (*ldAuthenticateSecurityResponseCallbackFunc)(const QByteArray &resData);

class LDCORESHARED_EXPORT ldUsbHardwareManager : public ldAbstractHardwareManager
{
    Q_OBJECT

public:
    explicit ldUsbHardwareManager(ldFilterManager *filterManager, QObject *parent = 0);
    ~ldUsbHardwareManager();

    virtual QString hwType() const override;
    virtual QString managerName() const override { return "USB Hardware Manager"; }

    virtual void setConnectedDevicesActive(bool active) override;

    virtual uint deviceCount() const override;
    virtual std::vector<ldHardware*> devices() const override;

    virtual void debugAddDevice() override;
    virtual void debugRemoveDevice() override;

    virtual void deviceCheck() override;

    static void setGenerateSecurityRequestCb(ldGenerateSecurityRequestCallbackFunc authenticateFunc);
    static void setAuthenticateSecurityCb(ldAuthenticateSecurityResponseCallbackFunc checkFunc);

public slots:
    virtual void removeDevice(const QString &hwId) override;

private:
    mutable QRecursiveMutex m_mutex;

    static ldGenerateSecurityRequestCallbackFunc m_genSecReqCb;
    static ldAuthenticateSecurityResponseCallbackFunc m_authSecRespCb;

    std::vector<std::unique_ptr<ldUSBHardware> > m_usbHardwares;
    ldFilterManager *m_filterManager;

private:
    void removeDeviceImpl(uint index);

};

#endif // LDUSBHARDWAREMANAGER_H
