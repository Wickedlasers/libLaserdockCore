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

#ifndef LDQAUDIODECODER_H
#define LDQAUDIODECODER_H

#include <memory>

#include <QtCore/QElapsedTimer>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QTimer>

#define STUBFPS (30*2)

//#define LD_QAUDIO_DECODER

class QMediaPlayer;

class QAudioBuffer;
class QAudioDecoder;

class ldQAudioDecoder : public QObject
{
    Q_OBJECT

public:
    explicit ldQAudioDecoder(QObject *parent);
    ~ldQAudioDecoder();
        
public slots:
    void start(const QString &filePath);
    void stop();
    
signals:
    void soundUpdated(const char * data, qint64 len);

protected slots:
    void timerSlot();

protected:
    QMutex mutex;

    QTimer m_timer;
    QElapsedTimer m_elapsedTimer;
    qint64 m_duration = -1;

    std::unique_ptr<QAudioDecoder> m_qaudioDecoder;
    std::vector<QAudioBuffer> m_audioBuffers;

    int m_lastSentBufferIndex = -1;
    int m_lastSentBufferEndIndex = -1;

    std::vector<char> m_soundData;
};

#endif //LDQAUDIODECODER_H

