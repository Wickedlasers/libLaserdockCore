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

//
//  ldLoopbackAudioDeviceWorker.h
//  ldCore
//
//  Created by Sergey Gavrushkin on 09/08/16.
//  Copyright (c) 2016 Wicked Lasers. All rights reserved.
//

#ifndef ldCore__ldLoopbackAudioDeviceWorker__
#define ldCore__ldLoopbackAudioDeviceWorker__

#include <QtCore/QMutex>
#include <QtCore/QThread>

#include "ldCore/Sound/ldSoundInterface.h"

class ldLoopbackAudioDeviceWorker : public QObject
{
    Q_OBJECT
public:
    explicit ldLoopbackAudioDeviceWorker(QString device, QObject *parent = 0);
    virtual ~ldLoopbackAudioDeviceWorker();

    static QStringList getAvailableOutputDevices();

    // ldSoundInterface
    QAudioFormat getAudioFormat() const {return m_format;}

    // worker
public slots:
    void process();
    void stop();

signals:
    void soundUpdated(const char * data, qint64 len);
    void finished();
    void error(QString error);

private:
    QRecursiveMutex m_mutex;
    bool m_stop = false;

    QAudioFormat m_format;
    QString m_device;
};

#endif

