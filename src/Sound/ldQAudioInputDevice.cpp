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

#include "ldQAudioInputDevice.h"

#include <QtCore/QtDebug>

namespace {
    static const int bufferFrames = 4096;  // 4096 ~= 10hz ~= 3 frames@30fps
    static const int BufferSize = bufferFrames*4; // 4096 frames 16bit stereo
}

QList<QAudioDeviceInfo> ldQAudioInputDevice::getDevices()
{
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);

#ifdef Q_OS_ANDROID
    auto it = devices.begin();
    while (it != devices.end()) {
        if(it->deviceName() == "camcorder"
                || it->deviceName() == "voicerecognition")
            it = devices.erase(it);
        else
            it++;
    }
#endif

    return devices;
}

QAudioDeviceInfo ldQAudioInputDevice::getDefaultDevice()
{
    return QAudioDeviceInfo::defaultInputDevice();
}

ldQAudioInputDevice::ldQAudioInputDevice(QObject *parent)
    : ldSoundInterface(parent)
    , m_buffer(BufferSize, 0)

{

}

ldQAudioInputDevice::~ldQAudioInputDevice()
{

}

bool ldQAudioInputDevice::activateInputDevice(const QAudioDeviceInfo &info)
{
//    m_info = ldSoundDeviceInfo(ldSoundDeviceInfo::Type::QAudioInput, info.deviceName());

    qDebug() << "Using sound input device: " << info.deviceName()
             << "; preferred format" << info.preferredFormat();

    m_format = getDefaultAudioFormat();

    if (!info.isFormatSupported(m_format)) {
        m_format = info.nearestFormat(m_format);
        qWarning() << "Default format not supported - trying to use nearest:" << m_format;
    }

    // qDebug() << "mic audio createAudioInput()";

    m_audioInput.reset(new QAudioInput(info, m_format, this));
    connect(m_audioInput.data(), &QAudioInput::stateChanged, this, &ldQAudioInputDevice::onAudioInputStateChanged);

    m_format = m_audioInput->format(); // FIXME: does it really needed? format should be the same as in QAudioInput constructor upper


    m_audioInput->setBufferSize(AUDIO_SUB_BLOCK_SIZE * m_format.channelCount());
  //m_audioInput->setBufferSize(AUDIO_SUB_BLOCK_SIZE * m_format.channelCount() * (m_format.sampleSize()/8));

    //m_audioInput->setNotifyInterval(5);
    m_input.reset(m_audioInput->start());
    qDebug() << "Actually used format " << m_audioInput->format() << m_audioInput->format().sampleSize() << m_audioInput->bufferSize();
    if(m_input) {
        connect(m_input.data(), &QIODevice::readyRead, this, &ldQAudioInputDevice::onInputReadyRead);
        return true;
    } else {
        emit error(tr("Failed to open audio device"));
        return false;
    }
}

bool ldQAudioInputDevice::isActive() const
{
    return m_audioInput;
}

QAudioFormat ldQAudioInputDevice::format() const
{
    return m_format;
}

void ldQAudioInputDevice::stop()
{
    if (m_audioInput) {
        m_audioInput->stop();
        m_audioInput.reset();
        // Note: The pointer will become invalid after the stream is stopped or if you start another stream.
        if(m_input) {
#ifdef QAUDIO_INPUT_CRASH_TEST
            // It looks like correct way for memory leaks for valgrind for some audio devices
            // BUT! it causes crash for some devices since input stream is deleted in m_audioInput->stop();
            // looks like there is no way to determine when
            m_input->close();
            m_input->reset();
#else
            m_input.take();
#endif
        }
    }
}

void ldQAudioInputDevice::onInputReadyRead()
{
    if (!m_audioInput) return;

    //qDebug() << "audio input bytes " << m_audioInput->bytesReady() << "/ " << BufferSize;

//    qint64 len = m_audioInput->bytesReady();
//    if (len > BufferSize) len = BufferSize;

    qint64 l = m_input->read(m_buffer.data(), BufferSize);

    if (l > 0) {
        emit soundUpdated(m_buffer.constData(), l);
    } else {
        emit soundUpdated(NULL, 0);
    }
}

void ldQAudioInputDevice::onAudioInputStateChanged(QAudio::State state)
{
    if (!m_audioInput) return;

//    qDebug() << "Handle audio input" << m_info.name() << "state changed: " << state;

    if(state == QAudio::StoppedState) {
        stop();
    }
}

