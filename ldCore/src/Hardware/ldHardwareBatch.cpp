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

#include "ldCore/Hardware/ldHardwareBatch.h"

#include <QtCore/QDebug>

#include "ldCore/Hardware/ldAbstractHardwareManager.h"
#include "ldCore/Hardware/ldHardware.h"
#include "ldCore/Hardware/ldHardwareManager.h"

ldHardwareBatch::ldHardwareBatch(ldHardwareManager *hwManager, QObject *parent)
    : QObject(parent)
    , m_hwManager(hwManager)
    , m_deviceBufferConfigDelegator(new ldDeviceBufferConfigDelegator(this))
{
    qDebug() << __FUNCTION__;

    connect(hwManager, &ldHardwareManager::isActiveChanged, this, &ldHardwareBatch::UpdateDACRates);
    for(auto hwDevManager : hwManager->hardwareManagers()) {
        connect(hwDevManager, &ldAbstractHardwareManager::deviceCountChanged, this, &ldHardwareBatch::UpdateDACRates);
        connect(hwDevManager, &ldAbstractHardwareManager::deviceEnabledStateChanged, this, &ldHardwareBatch::UpdateDACRates);
    }

    connect(m_hwManager, &ldHardwareManager::deviceCountChanged, this, &ldHardwareBatch::updateDeviceCount);
    connect(m_hwManager, &ldHardwareManager::forcedDacRateChanged, this, &ldHardwareBatch::UpdateDACRates);
}

ldHardwareBatch::~ldHardwareBatch()
{
    if(!m_isDefaultMode) {
        for(ldHardware *hwDevice : devices())
            hwDevice->setBatch(nullptr);
    }
}

uint ldHardwareBatch::getCommonDACRate() const
{
    return m_commonDACRate;
}

std::vector<ldHardware*> ldHardwareBatch::devices() const
{
    std::vector<ldHardware*> devices;
    for(ldAbstractHardwareManager *hardwareManager : m_hwManager->hardwareManagers()) {
        std::vector<ldHardware*> hwDevices = hardwareManager->devices();
        for(ldHardware *hwDevice : hwDevices) {
            // TODO handle batches and device count properly
            connect(hwDevice, &ldHardware::batchChanged, this, &ldHardwareBatch::updateDeviceCount, Qt::UniqueConnection);
            if(m_isDefaultMode) {
                if(hwDevice->batch() == nullptr)
                    devices.push_back(hwDevice);
            } else {
                if(hwDevice->batch() == this)
                    devices.push_back(hwDevice);
            }
        }
    }
    return devices;
}


void ldHardwareBatch::sendData(uint startIndex, uint count)
{
    auto availableDevices = devices();

    auto hardwareIt = availableDevices.begin();
    while(hardwareIt != availableDevices.end()) {
        ldHardware* hardware = (*hardwareIt);
        // we should explicitly activate devices before usage
        if(!hardware->isActive()) {
            hardwareIt++;
            continue;
        }

        bool b = hardware->send_samples(startIndex, count);

        if (!b){
            m_hwManager->removeDevice(hardware);
            break;
        } else {
            hardwareIt++;
        }
    }
}

bool ldHardwareBatch::hasActiveUSBDevices() const
{
    auto availableDevices = devices();
    for(ldHardware *hardware : availableDevices) {
        if(hardware->hwType() == "USB" && hardware->isActiveAndInitialized())
            return true;
    }

    return false;
}

ldDeviceBufferConfigDelegator::DeviceBufferConfig ldHardwareBatch::getBufferConfig() const
{
    return m_deviceBufferConfigDelegator->getBufferConfig();
}

// static int LIMIT = 1000;
// static int CURRENT = 0;

int ldHardwareBatch::getSmallestBufferCount() const
{
    auto availableDevices = devices();

    int fullCount = -1;
    int tmp = 0;

    auto networkHardwareIt = availableDevices.begin();
    while(networkHardwareIt != availableDevices.end()) {
        ldHardware *hardware = (*networkHardwareIt);
        // we should explicitly activate devices before usage
        if(!hardware->isActive()) {
            networkHardwareIt++;
            continue;
        }

        tmp = hardware->get_full_count();

        // CURRENT++;
        // if(CURRENT > LIMIT) {
        //     qDebug() << "BREAK";
        //     tmp = -1;
        //     CURRENT = 0;
        // }

        if (tmp==-1){
            m_hwManager->removeDevice(hardware);
            break;
        } else {
            networkHardwareIt++;
            if (fullCount==-1) fullCount = tmp;
            else if (tmp<fullCount) fullCount = tmp; // take the lowest buffer used from all connected cubes
        }
    } // end while

    return fullCount;
}


// update all devices to the lowest max dac rate available for all enabled devices
// If one device has a max dac rate of 35000, and another has a max dac of 30000, 30000 is used,
// and if the 30000 max dac rate device is disconnected or disabled, 35000 will then be used.
void ldHardwareBatch::UpdateDACRates()
{
    auto availableDevices = devices();
    if(availableDevices.size() == 0)
        return;

    uint rate = UINT_MAX;

    if(m_hwManager->getForcedDACRate() == -1) {
        for(ldHardware *hardware : availableDevices) {
            if (hardware->isEnabled()) {
                uint dr = hardware->getMaximumDacRate();
                if (dr!=0 && dr<rate) rate = dr;
            }
        }
    } else {
        rate = m_hwManager->getForcedDACRate();
    }

    if (rate!=UINT_MAX) {
        qDebug() << "setting all devices to the lowest Maximum DAC rate:" << rate << "pps";
        for(ldHardware *hardware : availableDevices) {
            hardware->setDacRate(rate);
        }

        if (rate != m_commonDACRate) {
            m_commonDACRate = rate;
            emit commonDACRateChanged(rate);
        }
    }

    m_deviceBufferConfigDelegator->updateBufferingStrategy(availableDevices);

    // now inform each device of the new buffer threshold
    int lvl = getBufferConfig().remote_buffer_cutoff;

    if (lvl > 0) {
        for(ldHardware *hardware : availableDevices) {
            hardware->setDacBufferTHold(lvl);
        }
    }
}

void ldHardwareBatch::setIds(const QStringList &ids)
{
    if(m_isDefaultMode) {
        if(!ids.isEmpty())
            qWarning() << "You are not supposed to call includeIds in default mode";
        return;
    }

    for(ldAbstractHardwareManager *hardwareManager : m_hwManager->hardwareManagers()) {
        std::vector<ldHardware*> hwDevices = hardwareManager->devices();
        for(ldHardware *hwDevice : hwDevices) {
            if(ids.contains(hwDevice->id())) {
                hwDevice->setBatch(this);
            } else if(m_ids.contains(hwDevice->id())) {
                hwDevice->setBatch(nullptr);
            }
        }
    }

    m_ids = ids;

    updateDeviceCount();
}

void ldHardwareBatch::updateIds()
{
    if(m_isDefaultMode)
        return;

    setIds(m_ids);
}

void ldHardwareBatch::setDefaultMode(bool isDefaultMode)
{
    m_isDefaultMode = isDefaultMode;

    updateDeviceCount();
}

void ldHardwareBatch::updateDeviceCount()
{
    auto availableDevices = devices();

    if(m_deviceCount != availableDevices.size()) {
        m_deviceCount = availableDevices.size();
        emit deviceCountChanged(m_deviceCount);
    }
}

