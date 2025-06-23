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

#ifndef LDHARDWAREMANAGER_H
#define LDHARDWAREMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include <QQmlHelpers>

#include "ldCore/ldCore_global.h"

class ldAbstractHardwareManager;
class ldHardware;

class LDCORESHARED_EXPORT ldHardwareManager : public QObject
{
    Q_OBJECT

    QML_WRITABLE_PROPERTY(bool, isActive)

public:
    explicit ldHardwareManager(QObject *parent = nullptr);
    ~ldHardwareManager();

    int getDeviceCount() const;
    std::vector<ldHardware*> devices() const;

    std::vector<ldAbstractHardwareManager*> hardwareManagers() const;

    void setForcedDACRate(int rate);
    int getForcedDACRate() const;

    void checkDevices();
    void initCheckTimer();

    void removeDevice(ldHardware *hw);
public slots:
    void addHardwareManager(ldAbstractHardwareManager *hardwareManager);

    void setConnectedDevicesActive(bool active);

signals:
    void deviceCountChanged(uint count);
    void forcedDacRateChanged(int rate);

private:
    void setDeviceCount(uint deviceCount);
    void updateDeviceCount();
    void updateCheckTimerState();

    uint m_deviceCount{0};

    int m_forcedDACRate = -1;

    std::vector<ldAbstractHardwareManager*> m_hardwareManagers;

    QTimer *m_deviceCheckTimer{nullptr};
};

#endif // LDHARDWAREMANAGER_H
