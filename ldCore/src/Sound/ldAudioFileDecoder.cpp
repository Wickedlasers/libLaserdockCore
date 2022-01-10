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

#include "ldCore/Sound/ldAudioFileDecoder.h"

#include <QtCore/QtDebug>
#include <QtCore/QThread>

#ifdef Q_OS_ANDROID
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#endif

#pragma warning(push, 0)
#include <audiodecoder.h>
#pragma warning(pop)

#include <ldCore/Sound/ldSoundData.h>

class ldAudioFileDecoderThread : public QThread
{
    Q_OBJECT

public:
    ldAudioFileDecoderThread(const QString &filePath) {
#ifdef AUIDIO_DECODER_SUPPORTED
        m_filePath = filePath;

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
#endif
    }

    qint64 duration() const
    {
#ifdef AUIDIO_DECODER_SUPPORTED
        return m_audioDecoder->duration();
#else
        return 0;
#endif
    }


    void stop() {
        m_isStop = true;
    }

    std::vector<float> m_data;

protected:
    virtual void run() override {
#ifdef AUIDIO_DECODER_SUPPORTED
        bool isAudioDecoder = m_audioDecoder.get();
        if(!isAudioDecoder
            || (m_audioDecoder->duration() == -1))
            return;

        // prepare block for reading
        int fileSamplePos = 0;
        std::vector<float> blockData;
        int blockSize = m_audioDecoder->sampleRate() / 30;
        blockData.resize(blockSize);

        std::vector<float> data;

        int numsamples = m_audioDecoder->numSamples();

        // read from file
        while(fileSamplePos < numsamples ) {
            if(m_isStop)
                break;
            int readed = m_audioDecoder->read(blockSize, blockData.data());
            if(!readed)
                break;

            fileSamplePos += readed;

            float tMaxL = 0.0f, tMaxR = 0.0f;
            for(int j = 0; j <  (readed/2); j++) {
                float tLeft = blockData[j*2+0];
                float tRight = blockData[j*2+1];
                float tAbsL = fabs(tLeft);
                if(tAbsL > tMaxL)
                    tMaxL = tAbsL;

                float tAbsR = fabs(tRight);
                if(tAbsR > tMaxR)
                    tMaxR = tAbsR;
            }

            data.push_back((tMaxL + tMaxR)/2);
        }

        if(!m_isStop)
            m_data = data;
#endif
    }
private:
    QString m_filePath;

    std::atomic<bool> m_isStop = false;
#ifdef AUIDIO_DECODER_SUPPORTED
    std::unique_ptr<AudioDecoder> m_audioDecoder;
#endif

};

ldAudioFileDecoder::ldAudioFileDecoder(QObject *parent)
    : QObject(parent)
    , m_isActive(false)
{
}

ldAudioFileDecoder::~ldAudioFileDecoder()
{
    if(m_currentThread) {
        m_currentThread->stop();
        if(m_currentThread->wait(500))
            delete m_currentThread;
    }
}

qint64 ldAudioFileDecoder::duration() const
{
    return m_duration;
}

void ldAudioFileDecoder::start(const QString &filePath)
{
    QMutexLocker lock(&m_mutex);

    qDebug() << "ldAudioFileDecoder" << __FUNCTION__ << filePath;

    if(filePath.isEmpty()) {
        return;
    }

    reset();

    m_currentThread = new ldAudioFileDecoderThread(filePath);
    m_duration = m_currentThread->duration();
    connect(m_currentThread, &ldAudioFileDecoderThread::finished, this, &ldAudioFileDecoder::onThreadFinished, Qt::QueuedConnection);
    m_currentThread->start();
    update_isActive(true);
}

void ldAudioFileDecoder::reset()
{
    m_duration = -1;
    if(m_currentThread) {
        m_currentThread->stop();
//        m_thread->quit();
        if(m_currentThread->wait(500))
            m_currentThread->deleteLater();
        m_currentThread = nullptr;
    }
    update_isActive(false);

    emit finished(std::vector<float>{});
}


void ldAudioFileDecoder::onThreadFinished()
{
    QMutexLocker lock(&m_mutex);

    ldAudioFileDecoderThread *thread = qobject_cast<ldAudioFileDecoderThread*>(sender());
    if(m_currentThread != thread) {
        thread->deleteLater();
        return;
    }

    qDebug() << "ldAudioFileDecoder" << __FUNCTION__;

    update_isActive(false);

    // TODO: std::move here for optimisation
    emit finished(thread->m_data);
    thread->deleteLater();
    m_currentThread = 0;
}

#include "ldAudioFileDecoder.moc"
