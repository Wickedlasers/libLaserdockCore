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

#include <ldCore/Data/ldDataDispatcher.h>

#include <QtCore/QDebug>
#include <QtCore/QMetaObject>

#include "ldCore/ldCore.h"
#include "ldCore/Data/ldAbstractDataWorker.h"
#include "ldCore/Data/ldUsbDataWorker.h"
#include "ldCore/Hardware/ldHardware.h"
#include "ldCore/Hardware/ldHardwareManager.h"
#include "ldCore/Simulator/ldSimulatorEngine.h"

#include "ldCore/Data/ldBufferManager.h"

ldDataDispatcher::ldDataDispatcher(ldBufferManager *bufferManager, ldHardwareManager *hardwareManager, QObject *parent)
  : QObject(parent)
  , m_simulatorEngine(new ldSimulatorEngine())
  , m_usbDataWorker(new ldUsbDataWorker(bufferManager, hardwareManager, m_simulatorEngine.get()))
  , m_additionalDataWorker(nullptr)
{
    qDebug() << __FUNCTION__;

    connect(m_usbDataWorker.get(), &ldUsbDataWorker::isActiveTransferChanged, this, [&]() {
        emit activeChanged(
                    (m_additionalDataWorker && m_additionalDataWorker->isActiveTransfer())
                    || m_usbDataWorker->isActiveTransfer()
                    );
    });

    // auto stop when hardware connects
    connect(hardwareManager, &ldHardwareManager::deviceCountChanged, this, [&](uint count) {
        if(count == 0) {
            setActiveTransfer(false);
        }
    });

    m_usbDataWorker->setActive(true);
}

ldDataDispatcher::~ldDataDispatcher()
{
//    qDebug() << "~ldDataDispatcher";
}

bool ldDataDispatcher::isActiveTransfer() const
{
    return m_usbDataWorker->isActiveTransfer()
            || (m_additionalDataWorker && m_additionalDataWorker->isActiveTransfer())
            ;
}

void ldDataDispatcher::setAdditionalDataWorker(ldAbstractDataWorker *dataWorker)
{
    if(m_additionalDataWorker) {
        disconnect(m_additionalDataWorker, 0, this, 0);
    }

    m_additionalDataWorker = dataWorker;

    if(m_additionalDataWorker) {
        connect(m_additionalDataWorker, &ldAbstractDataWorker::isActiveTransferChanged, this, [&]() {
            emit activeChanged(m_additionalDataWorker->isActiveTransfer() || m_usbDataWorker->isActiveTransfer());
        });
    }
}

ldUsbDataWorker *ldDataDispatcher::usbDataWorker() const
{
    return m_usbDataWorker.get();
}

ldSimulatorEngine *ldDataDispatcher::simulatorEngine() const
{
    return m_simulatorEngine.get();
}

void ldDataDispatcher::setActiveTransfer(bool active)
{
    // manage worker state to have nice output
    if(m_additionalDataWorker) {
        if(active) {
            if(m_additionalDataWorker->hasActiveDevices()) {
                m_additionalDataWorker->setActive(true);
                // disable simulator output in USB worker because additional worker provide it already and it is preferrable option
                m_usbDataWorker->setSimulatorEnabled(false);
            }
        } else {
            // disable additional worker
            m_additionalDataWorker->setActive(false);
            // activate default usb stub
            m_usbDataWorker->setActive(true);
            m_usbDataWorker->setSimulatorEnabled(true);
        }

        m_additionalDataWorker->setActiveTransfer(active);
    }

    // ok, proceed transfer
    m_usbDataWorker->setActiveTransfer(active);
}

