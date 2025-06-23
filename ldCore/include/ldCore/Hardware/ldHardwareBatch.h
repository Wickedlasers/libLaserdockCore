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

#ifndef LDHARDWAREBATCH_H
#define LDHARDWAREBATCH_H

#include <QtCore/QObject>

#include <ldCore/ldCore_global.h>
#include <ldCore/Hardware/ldAbstractHardwareManager.h>
#include "ldCore/Hardware/ldDeviceBufferConfigDelegator.h"

class ldHardwareManager;


class LDCORESHARED_EXPORT ldHardwareBatch : public QObject
{
    Q_OBJECT

public:
    explicit ldHardwareBatch(ldHardwareManager *hwManager, QObject *parent = nullptr);
    ~ldHardwareBatch();

    // get the DAC rate that all devices are currently operating at
    uint getCommonDACRate() const;

    std::vector<ldHardware *> devices() const;

    void sendData(uint startIndex, uint count);

    bool hasActiveUSBDevices() const;

    ldDeviceBufferConfigDelegator::DeviceBufferConfig getBufferConfig() const;

    int getSmallestBufferCount() const;

    void UpdateDACRates();

    void setIds(const QStringList &ids);
    void updateIds();

    void setDefaultMode(bool isDefaultMode = true);

signals:
    void deviceCountChanged(uint count);
    void commonDACRateChanged(uint rate);

private:
    void updateDeviceCount();

    std::atomic<uint>m_commonDACRate{30000};

    ldHardwareManager *m_hwManager;

    bool m_isDefaultMode{false};
    QStringList m_ids;
    ldDeviceBufferConfigDelegator * m_deviceBufferConfigDelegator;

    size_t m_deviceCount = 0;
};

#endif // LDHARDWAREBATCH_H
