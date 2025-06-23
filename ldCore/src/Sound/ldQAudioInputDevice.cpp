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
#include <QtMultimedia/QAudioInput>
#if QT_VERSION >= 0x060000
#include <QtMultimedia/QMediaDevices>
#endif

#include <ldCore/Sound/ldSoundData.h>

namespace {
    static const int bufferFrames = 4096;  // 4096 ~= 10hz ~= 3 frames@30fps
    static const int BufferSize = bufferFrames*4; // 4096 frames 16bit stereo
}


#if QT_VERSION >= 0x060000
QList<QAudioDevice> ldQAudioInputDevice::getDevices()
{
    QList<QAudioDevice> devices = QMediaDevices::audioInputs();
#else // QT_VERSION >= 0x060000
QList<QAudioDeviceInfo> ldQAudioInputDevice::getDevices()
{
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);

#ifdef Q_OS_WIN
    QStringList addedDevices;
    auto it = devices.begin();
    while (it != devices.end()) {
        if(addedDevices.contains(it->deviceName())) {
            qWarning() << __FUNCTION__ << QString("Ignoring duplicated device %1").arg(it->deviceName());
            printInfo(*it);
            for(QAudioDeviceInfo &eachInfo : devices) {
                if(addedDevices.contains(eachInfo.deviceName())) {
                    printInfo(eachInfo);
                }
            }
            it = devices.erase(it);
            continue;
        }

        addedDevices += (*it).deviceName();
        it++;
    }
#endif // Q_OS_WIN
#endif // QT_VERSION >= 0x060000

#ifdef Q_OS_ANDROID
    QStringList hiddenDevices{"camcorder", "voicerecognition"};
    // filter some internal devices on android
    auto it = devices.begin();
    while (it != devices.end()) {
#if QT_VERSION >= 0x060000
        if(hiddenDevices.contains(it->id()))
#else
        if(hiddenDevices.contains(it->deviceName()))
#endif
            it = devices.erase(it);
        else
            it++;
    }
#endif

    return devices;
}


#if QT_VERSION >= 0x060000
QAudioDevice ldQAudioInputDevice::getDefaultDevice()
{
    return QMediaDevices::defaultAudioInput();
}
#else
QAudioDeviceInfo ldQAudioInputDevice::getDefaultDevice()
{
    return QAudioDeviceInfo::defaultInputDevice();
}
#endif

#if QT_VERSION >= 0x060000
void ldQAudioInputDevice::printInfo(const QAudioDevice &inputAudioDevice)
{
    qDebug() << inputAudioDevice.id()
             << inputAudioDevice.description()
             << inputAudioDevice.isDefault()
             << inputAudioDevice.mode()
             << inputAudioDevice.preferredFormat()
             << inputAudioDevice.minimumSampleRate()
             << inputAudioDevice.maximumSampleRate()
             << inputAudioDevice.minimumChannelCount()
             << inputAudioDevice.maximumChannelCount()
             << inputAudioDevice.supportedSampleFormats()
             << inputAudioDevice.channelConfiguration()
        ;
}
#else
void ldQAudioInputDevice::printInfo(const QAudioDeviceInfo &inputAudioDevice)
{
    qDebug() << inputAudioDevice.deviceName()
             << inputAudioDevice.preferredFormat()
             << inputAudioDevice.supportedByteOrders()
             << inputAudioDevice.supportedChannelCounts()
             << inputAudioDevice.supportedCodecs()
             << inputAudioDevice.supportedSampleRates()
             << inputAudioDevice.supportedSampleSizes()
             << inputAudioDevice.supportedSampleTypes()
             << inputAudioDevice.realm()
        ;
}
#endif

ldQAudioInputDevice::ldQAudioInputDevice(QObject *parent)
    : ldSoundInterface(parent)
    , m_buffer(BufferSize, 0)

{

}

ldQAudioInputDevice::~ldQAudioInputDevice()
{

}

#if QT_VERSION >= 0x060000
bool ldQAudioInputDevice::activateInputDevice(const QAudioDevice &info)
{
//    m_info = ldSoundDeviceInfo(ldSoundDeviceInfo::Type::QAudioInput, info.id());

    qDebug() << "Using sound input device: " << info.description() << info.id()
             << "; preferred format" << info.preferredFormat();
#else
bool ldQAudioInputDevice::activateInputDevice(const QAudioDeviceInfo &info)
{
//    m_info = ldSoundDeviceInfo(ldSoundDeviceInfo::Type::QAudioInput, info.deviceName());

    qDebug() << "Using sound input device: " << info.deviceName()
             << "; preferred format" << info.preferredFormat();
#endif

    m_format = getDefaultAudioFormat();

    if (!info.isFormatSupported(m_format)) {
#if QT_VERSION >= 0x060000
        m_format = info.preferredFormat();
#else
        m_format = info.nearestFormat(m_format);
#endif
        qWarning() << "Default format not supported - trying to use nearest:" << m_format;
    }

    // qDebug() << "mic audio createAudioInput()";

#if QT_VERSION >= 0x060000
    m_audioInput.reset(new QAudioSource(info, m_format));
    connect(m_audioInput.get(), &QAudioSource::stateChanged, this, &ldQAudioInputDevice::onAudioInputStateChanged);
#else
    m_audioInput.reset(new QAudioInput(info, m_format));
    connect(m_audioInput.get(), &QAudioInput::stateChanged, this, &ldQAudioInputDevice::onAudioInputStateChanged);
#endif

    m_format = m_audioInput->format(); // FIXME: does it really needed? format should be the same as in QAudioInput constructor upper

#ifdef Q_OS_WIN
    // on Windows small buffer size can't be used because mic icon in tray starts flickering
    // 5880 is a compromise, the default buffer size is 35k on Windows
    m_audioInput->setBufferSize(AUDIO_SUB_BLOCK_SIZE * m_format.channelCount() * 4); //2940 * 4
#else
    // on mac & android small buffer works better
    int bufferSize = AUDIO_SUB_BLOCK_SIZE * m_format.channelCount();
//    if(m_format.sampleSize() > 8) {
//        double bufferModifier = m_format.sampleSize() / 8 / 2;
//        if(bufferModifier > 1)
//            bufferSize = static_cast<int>(bufferSize*bufferModifier);
//    }
//    qDebug() << bufferSize;
    m_audioInput->setBufferSize(bufferSize);
#endif


    //m_audioInput->setNotifyInterval(5);
    m_input.reset(m_audioInput->start());
    if(!m_audioInput) {
#if QT_VERSION >= 0x060000
        qWarning() << "Can't access audio input stream" << info.description() << info.id();
#else
        qWarning() << "Can't access audio input stream" << info.deviceName();
#endif
        emit error(tr("Failed to access audio input, please check your privacy settings"));
        return false;
    }
#if QT_VERSION >= 0x060000
    qDebug() << "Actually used format " << m_audioInput->format() << m_audioInput->format().sampleFormat() << m_audioInput->bufferSize();
#else
    qDebug() << "Actually used format " << m_audioInput->format() << m_audioInput->format().sampleSize() << m_audioInput->bufferSize();
#endif
    if(m_input) {
        connect(m_input.get(), &QIODevice::readyRead, this, &ldQAudioInputDevice::onInputReadyRead);
        return true;
    } else {
#if QT_VERSION >= 0x060000
        qWarning() << "Can't open audio input stream" << info.description() << info.id();
#else
        qWarning() << "Can't open audio input stream" << info.deviceName();
#endif
        emit error(tr("Failed to open audio device"));
        return false;
    }
}

bool ldQAudioInputDevice::isActive() const
{
    return m_audioInput.get();
}

QAudioFormat ldQAudioInputDevice::format() const
{
    return m_format;
}

void ldQAudioInputDevice::stop()
{
    if (m_audioInput) {
#if QT_VERSION >= 0x060000
        disconnect(m_audioInput.get(), &QAudioSource::stateChanged, this, &ldQAudioInputDevice::onAudioInputStateChanged);
#else
        disconnect(m_audioInput.get(), &QAudioInput::stateChanged, this, &ldQAudioInputDevice::onAudioInputStateChanged);
#endif
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
            m_input.release();
#endif
        }
    }
}

void ldQAudioInputDevice::onInputReadyRead()
{
    if (!m_input) return;

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

