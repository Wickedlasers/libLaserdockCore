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

#include "ldQAudioDecoder.h"

#include <QtCore/QtDebug>

#include <QtMultimedia/QAudioDecoder>
#include <ldCore/Sound/ldSoundInterface.h>

#include <ldCore/Sound/ldSoundData.h>

namespace  {
    const int DATA_LENGTH_TO_SEND = 44100 / STUBFPS * 2 * 2;  // stereo int
//    const int BUFFER_SIZE = 2000;
}

ldQAudioDecoder::ldQAudioDecoder(QObject *parent) :
    QObject(parent)
{
    m_timer.setInterval(1000 / STUBFPS); // 18 ms
    m_timer.setTimerType(Qt::PreciseTimer);
    connect(&m_timer, &QTimer::timeout, this, &ldQAudioDecoder::timerSlot);
}

ldQAudioDecoder::~ldQAudioDecoder()
{
    stop();
}

void ldQAudioDecoder::start(const QString &filePath)
{
    if(filePath.isEmpty()) {
        return;
    }

    m_qaudioDecoder.reset(new QAudioDecoder());
    connect(m_qaudioDecoder.get(), &QAudioDecoder::durationChanged, this, [&](qint64 duration) {
       m_duration = duration;
    });
    connect(m_qaudioDecoder.get(), &QAudioDecoder::bufferReady, this, [&]() {
        QAudioBuffer buffer = m_qaudioDecoder->read();
        m_audioBuffers.push_back(buffer);


//        emit soundUpdated(
//        buffer.data()
//        m_sampleData->
//       m_duration = duration;
    });

    m_qaudioDecoder->setSourceFilename(filePath);
    m_qaudioDecoder->setAudioFormat(ldSoundInterface::getDefaultAudioFormat());
    if(m_qaudioDecoder->error() != QAudioDecoder::NoError) {
        qWarning() << "QAudioDecoder::error" << m_qaudioDecoder->error();
        stop();
        return;
    }
    m_qaudioDecoder->start();

    m_elapsedTimer.start();
    m_timer.start();

    m_soundData.resize(DATA_LENGTH_TO_SEND);
}

void ldQAudioDecoder::stop()
{
    m_timer.stop();
    m_elapsedTimer.invalidate();
    m_duration = -1;

    m_qaudioDecoder.reset();
}

void ldQAudioDecoder::timerSlot()
{
    bool isAudioDecoder = m_qaudioDecoder.get();
    if(!isAudioDecoder
            || !m_elapsedTimer.isValid()
            )
        return;

    qint64 elapsedTime = m_elapsedTimer.elapsed() * 1000;
    // get sample index to read
    uint filledData = 0;
    bool isNeedToFillBuffer = false;
    bool isFullBufferExpected = true;
    auto it = m_audioBuffers.begin();
    while(it != m_audioBuffers.end()) {
        int audioBufferIndex = it - m_audioBuffers.begin();
        const QAudioBuffer &buffer = *it;
//        qDebug() << buffer.startTime() << buffer.duration() << elapsedTime;
        bool isInTime = buffer.startTime() <= elapsedTime &&
                buffer.startTime() + buffer.duration() > elapsedTime;

        const char* data = buffer.data<const char>();
        int bufferSizeToCopy = 0;
        int startIndex = 0;
        if(isInTime) {
            int availableSize = m_soundData.size() - filledData;

            qint64 startBufferTime = elapsedTime - buffer.startTime();
            double startPercent = (double) startBufferTime / buffer.duration();
            startIndex = startPercent * buffer.sampleCount();

            int availableSizeToCopy = buffer.sampleCount() - startIndex;
            bufferSizeToCopy = availableSizeToCopy > availableSize ? availableSize : availableSizeToCopy;

//            int bufferEndIndex = startIndex + bufferSizeToCopy;

            if(audioBufferIndex == m_lastSentBufferIndex
                    && m_lastSentBufferEndIndex > startIndex) {
                bufferSizeToCopy = 0;
                startIndex = 0;
//                bufferSizeToCopy = bufferEndIndex - m_lastSentBufferEndIndex;
//                startIndex = m_lastSentBufferEndIndex;
//                isFullBufferExpected = false;
            }

            // got first buffer
//            buffer.
        }

//        bool isNeedToFillBuffer = filledData > 0 && filledData < m_soundData.size();
        if(isNeedToFillBuffer) {
            bufferSizeToCopy = m_soundData.size() - filledData;
            startIndex = 0;
            // one should be enough
            isNeedToFillBuffer = false;
        }

        if(bufferSizeToCopy > 0) {
            memcpy(&m_soundData[filledData], &data[startIndex], bufferSizeToCopy);
            filledData += bufferSizeToCopy;

            m_lastSentBufferIndex = audioBufferIndex;
            m_lastSentBufferEndIndex = startIndex + bufferSizeToCopy;

            qDebug() << buffer.startTime() << buffer.duration() << elapsedTime;
            qDebug() << startIndex << bufferSizeToCopy << filledData << startIndex + bufferSizeToCopy;
//            qDebug() << availableSize << availableSizeToCopy << bufferSizeToCopy << filledData;

            if(filledData >= m_soundData.size()) {
                break;
            } else {
                isNeedToFillBuffer = isFullBufferExpected;
                if(isNeedToFillBuffer) {
                    it = m_audioBuffers.erase(it);
                    continue;
                }
            }
        }

        it++;
    }


    if(filledData > 0) {
        emit soundUpdated(&m_soundData[0], filledData);
    }
}
