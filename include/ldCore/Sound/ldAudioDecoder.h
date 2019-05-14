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

#ifndef LDAUDIODECODER_H
#define LDAUDIODECODER_H

#include <memory>

#include <QtCore/QElapsedTimer>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QTimer>

#include <ldCore/Sound/ldSoundInterface.h>

#include <QQmlHelpers>

#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
#define Q_OS_LINUX_DESKTOP
#endif

#ifndef Q_OS_LINUX_DESKTOP
#define AUIDIO_DECODER_SUPPORTED
#endif

class QMediaPlayer;

class AudioDecoder;


class LDCORESHARED_EXPORT ldAudioDecoder : public ldSoundInterface
{
    Q_OBJECT

    QML_READONLY_PROPERTY(bool, isActive)

public:
    explicit ldAudioDecoder(QObject *parent);
    ~ldAudioDecoder();

public slots:
    void start(const QString &filePath, qint64 elapsedTime = 0);
    void stop();

    void setElapsedTime(qint64 time);

protected slots:
    void timerSlot();

private:
    const int STUBFPS = 30*2;

    QMutex mutex;

    QTimer m_timer;
    qint64 m_elapsedTime = 0;
    QElapsedTimer m_elapsedTimer;
    qint64 m_duration = -1;

#ifdef AUIDIO_DECODER_SUPPORTED
    std::unique_ptr<AudioDecoder> m_audioDecoder;
#endif

    std::vector<float> m_sampleData;
};

#endif //LDAUDIODECODER_H

