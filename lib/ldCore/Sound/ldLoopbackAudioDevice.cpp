//
//  ldLoopbackAudioDevice.cpp
//  ldCore
//
//  Created by Sergey Gavrushkin on 09/08/16.
//  Copyright (c) 2016 Wicked Lasers. All rights reserved.
//

#include "ldLoopbackAudioDevice.h"

#include <QtCore/QCoreApplication>

#include <QtConcurrent/QtConcurrent>
#include <QtMultimedia/QAudioDeviceInfo>

#include "ldLoopbackAudioDeviceWorker.h"


/*!
  \class ldLoopbackAudioDevice
  \brief Actual class which represents the default output for the underlying platform.
  \inmodule audio

  This class is used to analyze a segment of audio data from output interface and extract relevant data from it.
  This data can then be used to generate live visualization.
*/

QStringList ldLoopbackAudioDevice::getAvailableOutputDevices()
{
    return ldLoopbackAudioDeviceWorker::getAvailableOutputDevices();
}

ldLoopbackAudioDevice::ldLoopbackAudioDevice(QString device, QObject *parent)
    : ldSoundInterface(parent)
    , m_device(device)
{
}

ldLoopbackAudioDevice::~ldLoopbackAudioDevice()
{
    stopCapture();
}

QAudioFormat ldLoopbackAudioDevice::getAudioFormat() const
{
    if(m_worker) {
        return m_worker->getAudioFormat();
    } else {
        return QAudioFormat();
    }
}


QString ldLoopbackAudioDevice::device() const
{
    return m_device;
}


void ldLoopbackAudioDevice::startCapture()
{
//    qDebug() << "ldLoopbackAudioDevice::startCapture";

    stopCapture();

    m_worker = new ldLoopbackAudioDeviceWorker(m_device);
    connect(m_worker, &ldLoopbackAudioDeviceWorker::soundUpdated, this, &ldLoopbackAudioDevice::soundUpdated);

    // setup thread
    m_worker_thread = new QThread();
    m_worker->moveToThread(m_worker_thread);
    connect(m_worker_thread, &QThread::started, m_worker, &ldLoopbackAudioDeviceWorker::process);
    connect(m_worker, &ldLoopbackAudioDeviceWorker::error, this, &ldLoopbackAudioDevice::error);
    connect(m_worker, &ldLoopbackAudioDeviceWorker::finished, m_worker_thread, &QThread::quit);
    connect(m_worker, &ldLoopbackAudioDeviceWorker::finished, m_worker, &ldLoopbackAudioDeviceWorker::deleteLater);
    connect(m_worker_thread, &QThread::finished, m_worker_thread, &QThread::deleteLater);

    // clean references in case of finish. immediately
    connect(m_worker, &ldLoopbackAudioDeviceWorker::finished, this, [&]() {
        m_worker = NULL;
        m_worker_thread = NULL;
    }, Qt::DirectConnection);

    // start
    m_worker_thread->start();
}

void ldLoopbackAudioDevice::stopCapture()
{
    if(m_worker_thread) {
//        qDebug() << "ldLoopbackAudioDevice::stopCapture";

        qApp->removePostedEvents(this);
        disconnect(m_worker, &ldLoopbackAudioDeviceWorker::soundUpdated, this, &ldLoopbackAudioDevice::soundUpdated);
        m_worker->stop();
        // we should process signals from worker before quit


        m_worker = NULL;
        m_worker_thread = NULL;
    }
}

