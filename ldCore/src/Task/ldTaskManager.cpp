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

#include "ldCore/Task/ldTaskManager.h"

#include <QtDebug>

#include "ldCore/Task/ldAbstractTask.h"
#include "ldCore/Task/ldTaskWorker.h"


/*!

  \class ldTaskManager
  \brief shell class that bootstraps \l ldTaskWorker, which does the real task execution.
  \inmodule rendering

*/

/*!

  \brief creates \l ldTaskWorker, and worker thread. Move the worker to thread and then calls start on the thread.

*/


ldTaskManager::ldTaskManager(ldBufferManager *bufferManager, QObject *parent) :
    QObject(parent),
    m_taskworker(new ldTaskWorker(bufferManager))
{
    qDebug() << __FUNCTION__;
    m_taskworker->moveToThread(&m_worker_thread);
    m_worker_thread.start();
    //m_worker_thread.setPriority(QThread::Priority::HighPriority);
}

ldTaskManager::~ldTaskManager()
{
    m_worker_thread.quit();
    if(!m_worker_thread.wait(500)) {
        qWarning() << "ldTaskManager m_worker_thread wasn't finished correctly";
    }

    m_taskworker.reset();
}

ldTaskWorker *ldTaskManager::taskWorker() const{
    return m_taskworker.data();
}
