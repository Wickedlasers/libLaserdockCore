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

#include "ldCore/Task/ldTaskWorker.h"

#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QTimer>

#include "ldCore/Data/ldBufferManager.h"
#include "ldCore/Task/ldAbstractTask.h"
#include "ldCore/Task/ldTaskManager.h"
#include "ldCore/ldCore.h"
#include "ldCore/Data/ldFrameBuffer.h"


/*!

  \class ldTaskWorker
  \brief Actual workhorse of the task execution.
  \inmodule rendering

  This classes lives in the task exection thread. It does the actual execution of the task loaded.
  The \l ldTaskWorker class is implements \l ldAppStateClientInterface and is a client of \l ldAppStateManager.

  This class listens for following commands from the \l {AppCommandAndStateManagement}{Application Command and State System}.

*/

ldTaskWorker::ldTaskWorker(ldBufferManager *bufferManager, QObject *parent) :
    QObject(parent)
    , m_task(NULL)
{
    connect(bufferManager, &ldBufferManager::bufferNeedsContent, this, &ldTaskWorker::update, Qt::ConnectionType::BlockingQueuedConnection);
}

ldTaskWorker::~ldTaskWorker()
{
    delete m_task;
}

bool ldTaskWorker::isRunning() const
{
    return m_status == Status::Running;
}


void ldTaskWorker::update(ldFrameBuffer * buffer){

   // buffer->mutex->lock();


    if(m_status == Status::Running) {
        if(!buffer->isFilled()) {
            quint64 elapsed = m_timer.elapsed();
            m_task->update(elapsed - m_last_update_time, buffer);
            m_last_update_time = elapsed;
            buffer->commit();
        }
    }


   // buffer->mutex->unlock();

}

void ldTaskWorker::loadTask(ldAbstractTask *task)
{
    m_taskToSet = task;
    QTimer::singleShot(0, this, &ldTaskWorker::taskLoad);
}

void ldTaskWorker::startTask()
{
    QTimer::singleShot(0, this, &ldTaskWorker::taskStart);
}

void ldTaskWorker::stopTask()
{
    QTimer::singleShot(0, this, &ldTaskWorker::taskStop);
}

void ldTaskWorker::resetTask()
{
    QTimer::singleShot(0, this, &ldTaskWorker::taskReset);
}

void ldTaskWorker::unloadTask()
{
    QTimer::singleShot(0, this, &ldTaskWorker::taskUnload);

}

bool ldTaskWorker::taskStart(){
    if(m_task == NULL) return false;

    if(m_status == Status::Reset || m_status == Status::Stopped ){
        m_task->start();
        m_status = Status::Running;
        m_timer.start();
        m_last_update_time = 0;

       //qDebug() << "Task Started!";
       return true;
    }

    return false;
}


bool ldTaskWorker::taskStop(){
    if(m_task == NULL) return false;

    if(m_status == Status::Running){
        m_task->stop();
        m_status = Status::Stopped;
        m_timer.invalidate();
       //qDebug() << "Task Stopped!";
       return true;
    }

    return false;
}


void ldTaskWorker::taskUnload(){
    if(m_task == NULL) return;

    if(m_status == Status::Reset || m_status == Status::Stopped ){
        delete m_task;
        m_status =  Status::NoTask;
        //qDebug() << "Task Unloaded!";
    }
}

void ldTaskWorker::taskReset(){
    if(m_task == NULL) return;

    if(m_status == Status::Stopped ){
        m_task->reset();
        m_status = Status::Reset;
        //qDebug() << "Task Reset!";
    }
}

void ldTaskWorker::taskLoad(){
    if(m_taskToSet == NULL) return;
    if(m_task) return;

    if(m_status == Status::NoTask) {
        m_task = m_taskToSet;
        m_taskToSet = nullptr;
        m_status = Status::Reset;
        m_task->initialize();
    }
}
