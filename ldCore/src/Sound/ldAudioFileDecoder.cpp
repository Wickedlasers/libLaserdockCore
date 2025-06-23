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
#include <QtCore/QCoreApplication>

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

#include <ldCore/Sound/ldSoundData.h>

class ldAudioFileDecoderWorker : public QObject
{
    Q_OBJECT

public:
    ldAudioFileDecoderWorker(const QString &filePath) {
#ifdef AUIDIO_DECODER_SUPPORTED
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
        qDebug() << " open" << filePath;
        int error = m_audioDecoder->open();
        qDebug() << " open2" << filePath;
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

    // std::vector<float> m_data;

signals:
    void progress(float value);
    void resultReady(const std::vector<float> &data);

public slots:
    void doWork() {
#ifdef AUIDIO_DECODER_SUPPORTED
        bool isAudioDecoder = m_audioDecoder.get();
        if(!isAudioDecoder
            || (m_audioDecoder->duration() == -1)
            || m_audioDecoder->sampleRate() == 0)
            return;

        // prepare block for reading
        int fileSamplePos = 0;
        std::vector<float> blockData;
        int blockSize = m_audioDecoder->sampleRate() / 1000;
        blockData.resize(blockSize);

        std::vector<float> data;

        int numsamples = m_audioDecoder->numSamples();

        // read from file
        int progressIndex = 0;

        // how often we want to emit progress block. each 0.33% is enough here
        int numberOfReads = numsamples/blockSize;
        int progressEmitBlock = numberOfReads/300;
        if(progressEmitBlock < 1)
            progressEmitBlock = 1;

        while(fileSamplePos < numsamples ) {
            // qApp->processEvents();

            // do not emit signal too often
            // qDebug() << (float) fileSamplePos / numsamples;
            if(progressIndex % progressEmitBlock == 0)
                emit progress((float) fileSamplePos / numsamples);
            progressIndex++;

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

        if(!m_isStop) {
            // m_data = data;
            emit resultReady(data);
        }
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
{
}

ldAudioFileDecoder::~ldAudioFileDecoder()
{
    reset();
}

void ldAudioFileDecoder::start(const QString &filePath)
{
    qDebug() << "ldAudioFileDecoder" << __FUNCTION__ << filePath;

    if(filePath.isEmpty()) {
        return;
    }

    reset();

    emit progress(0);

    m_workerThread = new QThread();
    m_worker = new ldAudioFileDecoderWorker(filePath);
    m_worker->moveToThread(m_workerThread);

    connect(m_worker, &ldAudioFileDecoderWorker::progress, this, &ldAudioFileDecoder::progress);
    connect(m_worker, &ldAudioFileDecoderWorker::resultReady, this, &ldAudioFileDecoder::finished);

    connect(m_workerThread, &QThread::started, m_worker, &ldAudioFileDecoderWorker::doWork);
    connect(m_workerThread, &QThread::finished, m_worker, &ldAudioFileDecoderWorker::deleteLater);

    m_workerThread->start();
}

void ldAudioFileDecoder::reset()
{
    if(!m_workerThread)
        return;

    disconnect(m_worker, &ldAudioFileDecoderWorker::progress, this, &ldAudioFileDecoder::progress);
    disconnect(m_worker, &ldAudioFileDecoderWorker::resultReady, this, &ldAudioFileDecoder::finished);

    m_workerThread->quit();
    m_workerThread->wait();
    m_workerThread = nullptr;

    m_worker = nullptr;
}


#include "ldAudioFileDecoder.moc"
