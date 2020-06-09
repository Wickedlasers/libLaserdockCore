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
//  ldLoopbackAudioDevice.h
//  ldCore
//
//  Created by Sergey Gavrushkin on 09/08/16.
//  Copyright (c) 2016 Wicked Lasers. All rights reserved.
//

#ifndef ldCore__ldLoopbackAudioDevice__
#define ldCore__ldLoopbackAudioDevice__

#include <QtCore/QThread>

#include "ldCore/Sound/ldSoundInterface.h"

class ldLoopbackAudioDeviceWorker;

class ldLoopbackAudioDevice : public ldSoundInterface
{
    Q_OBJECT
public:
    static QStringList getAvailableOutputDevices();

    explicit ldLoopbackAudioDevice(QString device, QObject *parent = 0);
    virtual ~ldLoopbackAudioDevice();

    // ldSoundInterface
    virtual QAudioFormat getAudioFormat() const override;

    QString device() const;

public slots:
    bool setEnabled(bool enabled);

signals:
    void error(QString error);

private:
    void startCapture();
    void stopCapture();

    QThread *m_worker_thread = NULL;
    ldLoopbackAudioDeviceWorker *m_worker = NULL;

    QString m_device;
};

#endif
