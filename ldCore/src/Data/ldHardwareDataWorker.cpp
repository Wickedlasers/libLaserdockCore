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

#include "ldCore/Data/ldHardwareDataWorker.h"

#include <QtCore/QtDebug>
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>

#include "ldCore/Hardware/ldHardwareManager.h"
#include "ldCore/Hardware/ldUSBHardwareManager.h"

#include "ldThreadedDataWorker.h"

ldHardwareDataWorker::ldHardwareDataWorker(ldBufferManager *bufferManager,
                                   ldHardwareManager *hardwareManager,
                                   std::vector<ldAbstractHardwareManager*> deviceHardwareManagers,
                                   ldSimulatorEngine *simulatorEngine,
                                   QObject *parent)
    : ldAbstractDataWorker(parent)
    , m_thread_worker(new ldThreadedDataWorker(bufferManager, simulatorEngine))
    , m_bufferManager(bufferManager)
    , m_hardwareManager(hardwareManager)
    , m_simulatorEngine(simulatorEngine)
    , m_deviceHardwareManagers(deviceHardwareManagers)
{    

   for (const auto &devman : qAsConst(m_deviceHardwareManagers)) {
        m_hardwareManager->addHardwareManager(devman);
    }

    m_thread_worker->setHardwareDeviceManagers(m_deviceHardwareManagers);

    m_thread_worker->moveToThread(&m_worker_thread);
    m_worker_thread.start();

    QObject::connect(m_thread_worker.data(), &ldThreadedDataWorker::activeChanged, this, &ldHardwareDataWorker::isActiveTransferChanged);

    for (const auto &devman : qAsConst(m_deviceHardwareManagers)) {
        connect(this,&ldHardwareDataWorker::isActiveTransferChanged,devman,&ldAbstractHardwareManager::setActiveTransfer);
    }

    connect(qApp, &QCoreApplication::aboutToQuit, [&]() {
        setActive(false);
        m_thread_worker.data()->stopProcess();
        m_worker_thread.quit();
        if(!m_worker_thread.wait(5000)) {
            qWarning() << "ldThreadedDataWorker worker_thread wasn't finished";
        }
    });
}

ldHardwareDataWorker::~ldHardwareDataWorker()
{
}

bool ldHardwareDataWorker::isActiveTransfer() const
{
    return m_thread_worker->isActiveTransfer();
}

bool ldHardwareDataWorker::hasActiveDevices() const
{
    for (const auto &devman : qAsConst(m_deviceHardwareManagers)) {
        if (devman->hasActiveDevices()) return true;
    }

    return false;
}

ldAbstractHardwareManager *ldHardwareDataWorker::deviceManager() const
{
    return m_deviceHardwareManagers.at(0);
}

void ldHardwareDataWorker::setActive(bool active)
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

    for (const auto &devman : qAsConst(m_deviceHardwareManagers)) {
        devman->set_isActive(active);
    }
}

void ldHardwareDataWorker::setActiveTransfer(bool active)
{
    m_thread_worker->setActiveTransfer(active);
}

void ldHardwareDataWorker::setSimulatorEnabled(bool enabled)
{
    m_thread_worker->setSimulatorEnabled(enabled);
}
