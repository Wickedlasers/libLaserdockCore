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

#include "ldCore/Data/ldUsbDataWorker.h"

#include <QtCore/QtDebug>
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>

#include "ldCore/Hardware/ldHardwareManager.h"
#include "ldCore/Hardware/ldUSBHardwareManager.h"

#include "ldThreadedDataWorker.h"

ldUsbDataWorker::ldUsbDataWorker(ldBufferManager *bufferManager,
                                   ldHardwareManager *hardwareManager,
                                   ldSimulatorEngine *simulatorEngine,
                                   QObject *parent)
    : ldAbstractDataWorker(parent)
    , m_thread_worker(new ldThreadedDataWorker(bufferManager, simulatorEngine))
    , m_bufferManager(bufferManager)
    , m_hardwareManager(hardwareManager)
    , m_simulatorEngine(simulatorEngine)
    , m_usbHardwareManager(new ldUsbHardwareManager(this))
{
    QObject::connect(m_thread_worker.data(), &ldThreadedDataWorker::activeChanged, this, &ldUsbDataWorker::isActiveTransferChanged);

    m_thread_worker->setUsbDeviceManager(m_usbHardwareManager);
    m_hardwareManager->addHardwareManager(m_usbHardwareManager);
    m_thread_worker->moveToThread(&m_worker_thread);
    m_worker_thread.start();

    connect(qApp, &QCoreApplication::aboutToQuit, [&]() {
        setActive(false);

        m_worker_thread.quit();
        if(!m_worker_thread.wait(5000)) {
            qWarning() << "ldUsbDataWorker2 worker_thread wasn't finished";
        }
    });
}

ldUsbDataWorker::~ldUsbDataWorker()
{
}

bool ldUsbDataWorker::isActiveTransfer() const
{
    return m_thread_worker->isActiveTransfer();
}

bool ldUsbDataWorker::hasActiveDevices() const
{
    return m_usbHardwareManager->hasActiveUsbDevices();
}

ldUsbHardwareManager *ldUsbDataWorker::deviceManager() const
{
    return m_usbHardwareManager;
}

void ldUsbDataWorker::setActive(bool active)
{
    if(m_isActive == active) {
        return;
    }

    m_isActive = active;

    if(active) {
        QTimer::singleShot(0, m_thread_worker.data(), &ldThreadedDataWorker::startProcess);
    } else {
        m_thread_worker->stopProcess();
    }
}

void ldUsbDataWorker::setActiveTransfer(bool active)
{
    m_thread_worker->setActiveTransfer(active);
}

void ldUsbDataWorker::setSimulatorEnabled(bool enabled)
{
    m_thread_worker->setSimulatorEnabled(enabled);
}
