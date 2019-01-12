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

#ifndef LDTASKMANAGER_H
#define LDTASKMANAGER_H
#include "ldCore/ldCore_global.h"

#include <QtCore/QObject>
#include <QtCore/QThread>


class ldAbstractTask;
class ldBufferManager;
class ldTaskWorker;

class LDCORESHARED_EXPORT ldTaskManager : public QObject
{
    Q_OBJECT

public:
    explicit ldTaskManager(ldBufferManager* bufferManager, QObject *parent = 0);
    virtual ~ldTaskManager();

    ldTaskWorker *taskWorker() const;

private:
    QThread m_worker_thread;
    QScopedPointer<ldTaskWorker> m_taskworker;
};

#endif // LDTASKMANAGER_H
