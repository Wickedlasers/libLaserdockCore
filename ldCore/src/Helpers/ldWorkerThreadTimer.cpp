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

#include <QtCore/QCoreApplication>

#include <ldCore/Helpers/ldWorkerThreadTimer.h>

namespace {
Q_LOGGING_CATEGORY(workerD, "ld.wtt")
}


void ldAbstractWorker::startWork() {
    qCDebug(workerD) << this << __FUNCTION__;
    onStart();
}

void ldAbstractWorker::pauseWork() {
    qCDebug(workerD) << this << __FUNCTION__;
    onPause();
}

void ldAbstractWorker::stopWork() {
    qCDebug(workerD) << this << __FUNCTION__;
    onStop();
}

void ldAbstractWorker::doWork() {
//    qCDebug(workerD) << this << __FUNCTION__;
    onRun();
}

ldWorkerThreadTimer::ldWorkerThreadTimer(QObject *parent)
    : QObject(parent)
{
    connect(qApp, &QCoreApplication::aboutToQuit, this, [&](){
        m_isAppAboutToQuit = true;
    });
}

ldWorkerThreadTimer::~ldWorkerThreadTimer()
{
    if(isActive())
        stop();
}

void ldWorkerThreadTimer::init(ldAbstractWorker *worker)
{
//    qCDebug(workerD) << this << m_worker << __FUNCTION__;

    m_worker = worker;
}

void ldWorkerThreadTimer::start()
{
    qCDebug(workerD) << this << m_worker << __FUNCTION__;

    if(!m_worker) {
        qWarning() << __FUNCTION__ << "is not inited" << this << parent();
        return;
    }

    if(!m_thread)
        startThread();
    else
        QTimer::singleShot(0, m_timer.get(), static_cast<void (QTimer::*)()>(&QTimer::start));


    m_worker->startWork();
}

void ldWorkerThreadTimer::pause()
{
    qCDebug(workerD) << this << m_worker << __FUNCTION__;

    QTimer::singleShot(0, m_timer.get(), &QTimer::stop);
    m_worker->pauseWork();
}

void ldWorkerThreadTimer::stop()
{
    if(!qApp->closingDown())
        qCDebug(workerD) << this << m_worker << __FUNCTION__;

    m_worker->stopWork();

    if(m_thread)
        stopThread();
}

bool ldWorkerThreadTimer::isActive() const
{
    return m_thread.get() != nullptr;
}

void ldWorkerThreadTimer::startThread()
{
    qCDebug(workerD) << this << m_worker << __FUNCTION__;

    m_thread.reset(new QThread());
    m_timer.reset(new QTimer());
#ifdef Q_OS_IOS
    // not sure if it really helps but iOS has some serious performance issues
    // SG i should check and debug it later
    m_timer->setInterval(33);
#else
    m_timer->setInterval(16); // mb 33 is enough and better for performance?
#endif

    connect(m_timer.get(), &QTimer::timeout, m_worker, &ldAbstractWorker::doWork);
    connect(m_thread.get(), &QThread::started, m_timer.get(), static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_thread.get(), &QThread::finished, m_timer.get(), &QTimer::stop);

    m_timer->moveToThread(m_thread.get());
    m_originalThread = m_worker->thread();
    connect(m_originalThread, &QThread::destroyed, this, [&]() {
        m_originalThread = nullptr;
    }, Qt::DirectConnection);
    m_worker->moveToThread(m_thread.get());

    m_thread->start();
}

void ldWorkerThreadTimer::stopThread()
{
    qCDebug(workerD) << this << m_worker << __FUNCTION__;

    m_thread->quit();
    // we can have deadlock issue here if we mess with mutexes somewhere in delegator
    if(!m_thread->wait(500)) {
        qWarning() << this << m_thread.get() << m_worker << __FUNCTION__ << "emergency thread termination";
        m_thread->terminate();
        m_thread->wait(500);
    }
    m_thread.reset();
    m_timer.reset();

    // move worker back to the main thread
    if(m_originalThread) {
        // Q_ASSERT(m_worker->thread() == nullptr);
        m_worker->moveToThread(m_originalThread); // crash in tl
    }
}
