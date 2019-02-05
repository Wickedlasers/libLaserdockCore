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

#include "ldCore/Sound/ldAudioDecoder.h"

#include <QtCore/QtDebug>

#ifdef Q_OS_ANDROID
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#endif

#include <libaudiodecoder/include/audiodecoder.h>

#include <ldCore/Sound/ldSoundData.h>

namespace  {
#ifdef _WIN32
    const int IS_PRELOAD_FILE = true;
#else
    const int IS_PRELOAD_FILE = false;
#endif
    const int SAMPLE_SIZE_TO_SEND = SAMPLE_SIZE;  // stereo int
//    const int BUFFER_SIZE = 2000;
}

ldAudioDecoder::ldAudioDecoder(QObject *parent)
    : ldSoundInterface(parent)
    , m_isActive(false)
{
    m_timer.setInterval(1000 / STUBFPS); // 18 ms
    m_timer.setTimerType(Qt::PreciseTimer);
    connect(&m_timer, &QTimer::timeout, this, &ldAudioDecoder::timerSlot);
}

ldAudioDecoder::~ldAudioDecoder()
{
}

void ldAudioDecoder::start(const QString &filePath, qint64 elapsedTime)
{
    if(filePath.isEmpty()) {
        return;
    }


#ifdef AUIDIO_DECODER_SUPPORTED
    m_audioDecoder.reset(new AudioDecoder(filePath.toStdString()));
#ifdef Q_OS_ANDROID
    QAndroidJniEnvironment qjniEnv;
    m_audioDecoder->initAndroidEnv(&(*qjniEnv));
#endif
    int error = m_audioDecoder->open();
    if(error != AUDIODECODER_OK) {
        qDebug() << "Error open" << filePath;
        return;
    }

    m_elapsedTime = elapsedTime;
    m_duration = m_audioDecoder->duration() * 1000.f;

    m_elapsedTimer.start();
    m_timer.start();

    m_sampleData.clear();

    if(IS_PRELOAD_FILE) {
        m_sampleData.resize(m_audioDecoder->numSamples());
        m_audioDecoder->read(m_audioDecoder->numSamples(), &m_sampleData[0]);
    } else {
        m_sampleData.resize(SAMPLE_SIZE_TO_SEND);
    }
#else
    Q_UNUSED(elapsedTime)
#endif

    update_isActive(true);
}

void ldAudioDecoder::stop()
{
#ifdef AUIDIO_DECODER_SUPPORTED
    m_timer.stop();
    m_elapsedTimer.invalidate();
    m_duration = -1;

    m_audioDecoder.reset();
    update_isActive(false);
#endif
}

void ldAudioDecoder::setElapsedTime(qint64 time)
{
    m_elapsedTime = time;
    m_elapsedTimer.restart();
    update_isActive(true);
}

void ldAudioDecoder::timerSlot()
{
#ifdef AUIDIO_DECODER_SUPPORTED
    bool isAudioDecoder = m_audioDecoder.get();
    if(!isAudioDecoder
            || !m_elapsedTimer.isValid()
            || (m_duration == -1))
        return;

    qint64 elapsedTime = m_elapsedTime + m_elapsedTimer.elapsed();
    // get sample index to read
    double elapsedPercent = (double) elapsedTime / m_duration;

    if(elapsedPercent >= 1.0) {
        return;
    }

    //    qDebug() << elapsedTime << m_duration << elapsedPercent;

    int sampleIndex = elapsedPercent * m_audioDecoder->numSamples();
    if(sampleIndex + SAMPLE_SIZE_TO_SEND >= m_audioDecoder->numSamples()) {
        sampleIndex = m_audioDecoder->numSamples() - SAMPLE_SIZE_TO_SEND;
    }
    if(sampleIndex % 2 != 0)
        sampleIndex -= 1;

    // get sample to send
    float * sampleToSend = nullptr;

    int sampleSizeToSend = SAMPLE_SIZE_TO_SEND;
    if(IS_PRELOAD_FILE) {
        sampleToSend = &m_sampleData[sampleIndex];
    } else {
        m_audioDecoder->seek(sampleIndex);
        sampleSizeToSend = m_audioDecoder->read(SAMPLE_SIZE_TO_SEND, &m_sampleData[0]);
        sampleToSend = &m_sampleData[0];
    }

    processAudioBuffer(sampleToSend, sampleSizeToSend / 2);
#endif
}
