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

#ifndef LDSOUNDINTERFACE_H
#define LDSOUNDINTERFACE_H
#include "ldCore/ldCore_global.h"

#include <memory>

#include <QtCore/QObject>
#include <QtMultimedia/qaudioinput.h>

#include <ldCore/Sound/ldSoundData.h>
#include <ldCore/Utilities/ldBasicDataStructures.h>
#include <ldCore/Utilities/ldThreadSafeQueue.h>


class LDCORESHARED_EXPORT ldSoundInterface : public QObject
{
    Q_OBJECT
public:
    static QAudioFormat getDefaultAudioFormat();

    explicit ldSoundInterface(QObject *parent = 0);
    ~ldSoundInterface();

    virtual QAudioFormat getAudioFormat() const;

    static int s_latencyms;
    static int s_enableLatency;

signals:
    void audioBlockUpdated(const AudioBlock &block);
    void soundUpdated(const char * data, qint64 len);

protected slots:
    void handleSoundUpdated(const char * data, qint64 len);
    void processAudioBuffer(float *convertedBuffer, int frames);

private:
    void convertRawToStereoFloatFrame(const char *data, qint64 frames, const QAudioFormat& m_format, float interleavedFrame[]);

    std::unique_ptr<ldThreadSafeQueue<float>> m_pAudioBuffer;
    std::unique_ptr<AudioBlock> m_pCurrentBlock;

private slots:
    void sendBlocks();

};

#endif // LDSOUNDINTERFACE_H
