//
//  ldLoopbackAudioDeviceWorker.h
//  ldCore
//
//  Created by Sergey Gavrushkin on 09/08/16.
//  Copyright (c) 2016 Wicked Lasers. All rights reserved.
//

#ifndef __ldCore__ldLoopbackAudioDeviceWorker__
#define __ldCore__ldLoopbackAudioDeviceWorker__

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
    QMutex m_mutex;
    bool m_stop = false;

    QAudioFormat m_format;
    QString m_device;
};

#endif

