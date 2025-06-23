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
#include <QtCore/QLoggingCategory>
#include <QtCore/QTimer>

#ifdef Q_OS_ANDROID
#if QT_VERSION >= 0x060000
#include <QtCore/QJniEnvironment>
#else
#include <QtAndroidExtras/QAndroidJniEnvironment>
#endif
#endif

#pragma warning(push, 0)
#include <audiodecoder.h>
#pragma warning(pop)

#include <ldCore/Sound/ldSoundAnalyzer.h>
#include <ldCore/Sound/ldSoundData.h>


namespace {
Q_LOGGING_CATEGORY(ldAd, "ld.ad")
}

// Sergey: see https://github.com/Wickedlasers/laserdock_apps_cmake/issues/2335 and other oscope bugs
// it can be tricky depending on the platform
// on Windows we need extra buffering
#ifdef _WIN32
const int ldAudioDecoder::LD_IS_BUFFERING_ENABLED = true;
#else
const int ldAudioDecoder::LD_IS_BUFFERING_ENABLED = false;
#endif

const int ldAudioDecoder::SAMPLE_SIZE_TO_SEND = SAMPLE_SIZE;  // stereo int
const int ldAudioDecoder::MAX_SAMPLE_SIZE_TO_SEND = SAMPLE_SIZE*3;  // stereo int
const int ldAudioDecoder::SAMPLE_BUFFER_SIZE = SAMPLE_SIZE_TO_SEND * 70*3;  // stereo int
const int ldAudioDecoder::DECODE_INTERVAL = 1000 / STUBFPS;

ldAudioDecoder::ldAudioDecoder(QObject *parent)
    : ldSoundInterface(parent)
    , m_isActive(false)
    , m_sampleDataBuffer(SAMPLE_BUFFER_SIZE)
    , m_analyzer(new ldSoundAnalyzer(this))
{
    m_timer.setInterval(DECODE_INTERVAL);
    connect(&m_timer, &QTimer::timeout, this, &ldAudioDecoder::timerSlot);
    connect(this, &ldAudioDecoder::bufferUpdated, m_analyzer, &ldSoundAnalyzer::processAudioBuffer);
}

ldAudioDecoder::~ldAudioDecoder()
{
}

ldSoundAnalyzer *ldAudioDecoder::analyzer() const
{
    return m_analyzer;
}

void ldAudioDecoder::start(const QString &filePath, qint64 elapsedTime)
{
    qCDebug(ldAd) << __FUNCTION__ << filePath << m_filePath;

    if(filePath.isEmpty()) {
        return;
    }

#ifdef AUIDIO_DECODER_SUPPORTED
    if(m_filePath != filePath) {
        reset();

        m_filePath = filePath;

        m_audioDecoder.reset(new AudioDecoder(filePath.toStdString()));
    #ifdef Q_OS_ANDROID
#if QT_VERSION >= 0x060000
        QJniEnvironment qjniEnv;
#else
        QAndroidJniEnvironment qjniEnv;
#endif
        m_audioDecoder->initAndroidEnv(&(*qjniEnv));
    #endif
        int error = m_audioDecoder->open();
        if(error != AUDIODECODER_OK) {
            qDebug() << "Error open" << filePath;
            return;
        }

        m_duration = m_audioDecoder->duration() * 1000.f;

        if(LD_IS_BUFFERING_ENABLED) {
            std::vector<float> sampleData(SAMPLE_BUFFER_SIZE);
            int readed = m_audioDecoder->read(sampleData.size(), &sampleData[0]);
            m_sampleDataBuffer.Push(&sampleData[0], readed);
            m_sampleData.resize(MAX_SAMPLE_SIZE_TO_SEND);
            m_fileSamplePos = readed;
        } else {
            m_sampleData.resize(MAX_SAMPLE_SIZE_TO_SEND);
        }
    }

    m_elapsedTime = elapsedTime;
    m_elapsedTimer.start();

#else
    Q_UNUSED(elapsedTime)
#endif

    if(!m_isActive)
        m_timer.start();

    update_isActive(true);
}

void ldAudioDecoder::pause()
{
    qCDebug(ldAd) << __FUNCTION__ << m_filePath;

    if(m_elapsedTimer.isValid())
        m_elapsedTime += m_elapsedTimer.elapsed();

    m_elapsedTimer.invalidate();
    m_timer.stop();

    update_isActive(false);
}

void ldAudioDecoder::stop()
{
    qCDebug(ldAd) << __FUNCTION__ << m_filePath;

    m_timer.stop();
#ifdef AUIDIO_DECODER_SUPPORTED
    reset();
    update_isActive(false);
#endif
}

void ldAudioDecoder::setElapsedTime(qint64 time)
{
    qCDebug(ldAd) << __FUNCTION__ << time;
    m_elapsedTime = time;

    if(m_elapsedTimer.isValid())
        m_elapsedTimer.restart();

    m_analyzer->reset();
}

void ldAudioDecoder::timerSlot()
{
    if(!m_isActive)
        return;

    qCDebug(ldAd) << __FUNCTION__ << m_elapsedTime << m_elapsedTimer.elapsed() << m_duration << m_audioDecoder->numSamples();

#ifdef AUIDIO_DECODER_SUPPORTED
    bool isAudioDecoder = m_audioDecoder.get();
    if(!isAudioDecoder
            || !m_elapsedTimer.isValid()
            || (m_duration == -1))
        return;

    qint64 elapsedTime = m_elapsedTime;
    qint64 deltaTime = m_elapsedTimer.restart();
    if(deltaTime == 0)
        return;

    m_elapsedTime+=deltaTime;

    // get sample index to read
    double elapsedPercent = (double) elapsedTime / m_duration;
    double deltaPercent = (double) deltaTime / m_duration;

    if(elapsedPercent + deltaPercent >= 1.0) {
        qCWarning(ldAd) <<  __FUNCTION__ << "elapsedPercent >= 1.0";
        return;
    }

    // qCDebug(ldAd) << elapsedTime << elapsedPercent;

    // calculate current sample index
    int sampleIndex = elapsedPercent * m_audioDecoder->numSamples();
    int sampleSizeToSend = deltaPercent * m_audioDecoder->numSamples();
    if(sampleSizeToSend > MAX_SAMPLE_SIZE_TO_SEND)
        sampleSizeToSend = MAX_SAMPLE_SIZE_TO_SEND;

    // check for last block
    if(sampleIndex + sampleSizeToSend >= m_audioDecoder->numSamples()) {
        sampleIndex = m_audioDecoder->numSamples() - sampleSizeToSend;
    }

    // it should be even (stereo)
    if(sampleIndex % 2 != 0)
        sampleIndex -= 1;
    if(sampleSizeToSend % 2 != 0)
        sampleSizeToSend -= 1;

    if(LD_IS_BUFFERING_ENABLED) {
        qCDebug(ldAd) << sampleIndex << m_fileSamplePos - sampleSizeToSend << m_fileSamplePos - SAMPLE_BUFFER_SIZE << m_sampleDataBuffer.GetLevel();
        if(sampleIndex > m_fileSamplePos - sampleSizeToSend
            || sampleIndex < m_fileSamplePos - SAMPLE_BUFFER_SIZE) {
            // refill buffer
            sampleIndex = m_audioDecoder->seek(sampleIndex);
            std::vector<float> sampleData(SAMPLE_BUFFER_SIZE);
            int readed = m_audioDecoder->read(sampleData.size(), &sampleData[0]);
            qCDebug(ldAd) << readed;
            m_sampleDataBuffer.Reset();
            m_sampleDataBuffer.Push(&sampleData[0], readed);
            m_fileSamplePos = sampleIndex + readed;
        }

        qCDebug(ldAd) << sampleIndex << m_fileSamplePos - SAMPLE_BUFFER_SIZE << m_sampleDataBuffer.GetLevel();
        if(sampleIndex > m_fileSamplePos - SAMPLE_BUFFER_SIZE) {
            std::vector<float> sampleData(sampleIndex - (m_fileSamplePos - SAMPLE_BUFFER_SIZE));
            if(m_fileSamplePos < m_audioDecoder->numSamples()) {
                int readed = m_audioDecoder->read(sampleData.size(), &sampleData[0]);
                qCDebug(ldAd) << readed;
                if(readed < sampleData.size())
                    std::fill(sampleData.begin() + readed, sampleData.end(), 0);
            } else {
                std::fill(sampleData.begin(), sampleData.end(), 0);
            }
            m_sampleDataBuffer.Push(&sampleData[0], sampleData.size());
            m_fileSamplePos += sampleData.size();
        }

        qCDebug(ldAd) << sampleIndex << m_fileSamplePos - SAMPLE_BUFFER_SIZE << m_sampleDataBuffer.GetLevel();

        // fill buffer
        m_sampleDataBuffer.GetFirst(&m_sampleData[0], sampleSizeToSend);
    } else {
        sampleIndex = m_audioDecoder->seek(sampleIndex);
        std::fill(m_sampleData.begin(), m_sampleData.end(), 0);
        sampleSizeToSend = m_audioDecoder->read(sampleSizeToSend, &m_sampleData[0]);
    }


    // qCDebug(ldAd) << sampleIndex << sampleSizeToSend;
    emit bufferUpdated(&m_sampleData[0], sampleSizeToSend / 2, getDefaultAudioFormat().sampleRate());
#endif
}

void ldAudioDecoder::reset()
{
    qCDebug(ldAd) << __FUNCTION__ << m_filePath;

    m_filePath.clear();

    m_elapsedTime = 0;
    m_elapsedTimer.invalidate();
    m_duration = -1;

#ifdef AUIDIO_DECODER_SUPPORTED
    m_audioDecoder.reset();
#endif

    m_sampleDataBuffer.Reset();
    m_sampleData.clear();

    m_fileSamplePos = 0;
    m_currentBlockPos = 0;

    m_analyzer->reset();
}

