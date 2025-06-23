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


ldTaskManager::ldTaskManager(QObject *parent) :
    QObject(parent)
{
    qDebug() << __FUNCTION__;
}

ldTaskManager::~ldTaskManager()
{
    // create copy
    auto taskWorkers = m_taskWorkers;
    for(auto taskWorker : taskWorkers)
        deleteTaskWorker(taskWorker);
}

ldTaskWorker *ldTaskManager::createTaskWorker(ldFrameBuffer *frameBuffer)
{
    QThread *m_worker_thread2 = new QThread();

    ldTaskWorker *newTaskWorker = new ldTaskWorker(frameBuffer);
    newTaskWorker->moveToThread(m_worker_thread2);
    m_worker_thread2->start();

    m_taskWorkers.push_back(newTaskWorker);

    return newTaskWorker;
}

void ldTaskManager::deleteTaskWorker(ldTaskWorker *taskWorker)
{
    auto it = std::find(m_taskWorkers.begin(), m_taskWorkers.end(), taskWorker);
    if(it == m_taskWorkers.end()) {
        qWarning() << __FUNCTION__ << "Can't find" << taskWorker;
        return;
    }

    auto taskWorkerThread = taskWorker->thread();
    taskWorkerThread->quit();
    if(!taskWorkerThread->wait(500)) {
        qWarning() << "ldTaskManager m_worker_thread wasn't finished correctly" << taskWorker;
    }

    delete taskWorker;
    delete taskWorkerThread;

    m_taskWorkers.erase(it);
}
