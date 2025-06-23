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

#ifndef LDWORKERTHREADTIMER_H
#define LDWORKERTHREADTIMER_H

#include <QtCore/QLoggingCategory>
#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtCore/QTimer>

#include "ldCore/ldCore_global.h"

class ldAbstractWorker;

class LDCORESHARED_EXPORT ldWorkerThreadTimer : public QObject
{
    Q_OBJECT
public:
    ldWorkerThreadTimer(QObject *parent = nullptr);
    ~ldWorkerThreadTimer();

    void init(ldAbstractWorker *worker);

    void start();
    void pause();
    void stop();

    bool isActive() const;

private:
    void startThread();
    void stopThread();

    ldAbstractWorker *m_worker = nullptr;
    std::unique_ptr<QThread> m_thread;
    std::unique_ptr<QTimer> m_timer;

    QThread *m_originalThread{nullptr};

    bool m_isAppAboutToQuit = false;
};

class LDCORESHARED_EXPORT ldAbstractWorker : public QObject
{
    Q_OBJECT
public:
    ldAbstractWorker() {}

    void startWork();
    void pauseWork();
    void stopWork();
    void doWork();

private:
    virtual void onStart() = 0;
    virtual void onPause() = 0;
    virtual void onStop() = 0;
    virtual void onRun() = 0;

};

#endif // LDWORKERTHREADTIMER_H
