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
#include <QtCore/QTimer>
#include <QtCore/QMutex>
#include <QtCore/QObject>

#include <ldCore/Sound/ldSoundInterface.h>
#include <ldCore/Utilities/ldCircularBuffer.h>

#include <QQmlHelpers>


#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
#define Q_OS_LINUX_DESKTOP
#endif

#ifndef Q_OS_LINUX_DESKTOP
#define AUIDIO_DECODER_SUPPORTED
#endif

class QMediaPlayer;

class AudioDecoder;

class ldSoundAnalyzer;

class LDCORESHARED_EXPORT ldAudioDecoder : public ldSoundInterface
{
    Q_OBJECT

    QML_READONLY_PROPERTY(bool, isActive)

public:
    explicit ldAudioDecoder(QObject *parent = nullptr);
    ~ldAudioDecoder();

    ldSoundAnalyzer* analyzer() const;

public slots:
    void start(const QString &filePath, qint64 elapsedTime = 0);
    void pause();
    void stop();

    void setElapsedTime(qint64 time);

signals:
    void bufferUpdated(float *convertedBuffer, int framesm, int sampleRate);

protected slots:
    void timerSlot();

private:
    void reset();

    static const int LD_IS_BUFFERING_ENABLED;

    static const int SAMPLE_SIZE_TO_SEND;
    static const int MAX_SAMPLE_SIZE_TO_SEND;
    static const int SAMPLE_BUFFER_SIZE;
    static const int STUBFPS = 30*2;
    static const int DECODE_INTERVAL;

    QMutex mutex;

    QString m_filePath;

    qint64 m_elapsedTime = 0;
    QElapsedTimer m_elapsedTimer;
    QTimer m_timer;
    qint64 m_duration = -1;

#ifdef AUIDIO_DECODER_SUPPORTED
    std::unique_ptr<AudioDecoder> m_audioDecoder;
#endif

    ldCircularBuffer<float> m_sampleDataBuffer;
    std::vector<float> m_sampleData;

    int m_fileSamplePos = 0;
    uint m_currentBlockPos = 0;

    ldSoundAnalyzer *m_analyzer;
};

#endif //LDAUDIODECODER_H

