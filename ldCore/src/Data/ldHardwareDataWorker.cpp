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

#include <ldCore/Hardware/ldHardwareBatch.h>

#include "ldThreadedDataWorker.h"

ldHardwareDataWorker::ldHardwareDataWorker(ldFrameBuffer *frameBuffer,
                                   ldHardwareBatch *hardwareBatch,
                                   ldSimulatorEngine *simulatorEngine,
                                   QObject *parent)
    : ldAbstractDataWorker(parent)
    , m_thread_worker(new ldThreadedDataWorker(frameBuffer, simulatorEngine, hardwareBatch))
    , m_hwBatch(hardwareBatch)
    , m_simulatorEngine(simulatorEngine)
{
    m_thread_worker->moveToThread(&m_worker_thread);
    m_worker_thread.start();

    QObject::connect(m_thread_worker.data(), &ldThreadedDataWorker::activeChanged, this, &ldHardwareDataWorker::isActiveTransferChanged);
}

ldHardwareDataWorker::~ldHardwareDataWorker()
{
    stop();
}

bool ldHardwareDataWorker::isActive() const
{
    return m_isActive;
}

bool ldHardwareDataWorker::isActiveTransfer() const
{
    return m_thread_worker->isActiveTransfer();
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

    emit isActiveChanged(active);
}

void ldHardwareDataWorker::setActiveTransfer(bool active)
{
    m_thread_worker->setActiveTransfer(active);
}

void ldHardwareDataWorker::setSimulatorEnabled(bool enabled)
{
    m_thread_worker->setSimulatorEnabled(enabled);
}

void ldHardwareDataWorker::stop()
{
    qDebug() << "ldHardwareDataWorker" << __FUNCTION__;

    m_thread_worker->stopProcess();

    m_worker_thread.quit();
    if(!m_worker_thread.wait(5000)) {
        qWarning() << "ldThreadedDataWorker worker_thread wasn't finished";
        //     m_worker_thread.terminate();
        if(!m_worker_thread.wait(5000)) {
            qWarning() << "ldThreadedDataWorker worker_thread wasn't finished 2";
        }
    }
}
