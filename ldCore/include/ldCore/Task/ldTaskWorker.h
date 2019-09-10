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

#ifndef LDTASKWORKER_H
#define LDTASKWORKER_H

#include <QtCore/QObject>
#include <QtCore/QWaitCondition>
#include <QtCore/QMutex>
#include <QtCore/QElapsedTimer>

#include "ldCore/ldCore_global.h"

class ldAbstractTask;
class ldBufferManager;
class ldFrameBuffer;

class LDCORESHARED_EXPORT ldTaskWorker : public QObject
{
    Q_OBJECT

public:
    explicit ldTaskWorker(ldBufferManager *bufferManager, QObject *parent = 0);
    ~ldTaskWorker();

    bool isRunning() const;

public slots:
    void update(ldFrameBuffer * buffer);

    void loadTask(ldAbstractTask * task);
    void startTask();
    void stopTask();
    void resetTask();
    void unloadTask();

private:
    enum class Status {
        NoTask,
        Reset,
        Running,
        Stopped
    };

    void taskLoad();
    bool taskStart();
    bool taskStop();
    void taskReset(); // clears task, task must be stopped before reset
    void taskUnload();


    ldAbstractTask * m_taskToSet;
    ldAbstractTask * m_task;

    QElapsedTimer m_timer;
    quint64 m_last_update_time = 0;
    
    Status m_status = Status::NoTask;

};

#endif // LDTASKWORKER_H
