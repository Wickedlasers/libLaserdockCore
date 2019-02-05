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

#include "ldCore/Sound/ldSoundDeviceManager.h"


#include <QtCore/QtDebug>
#include <QtCore/QCoreApplication>
#include <QtWidgets/QMessageBox>


#include "Sound/ldQAudioInputDevice.h"
// loopback
#ifdef LD_LOOPBACK_DEVICE_ENABLED
#include "Sound/ldLoopbackAudioDevice.h"
#include "Sound/ldLoopbackAudioDeviceWorker.h"
#include "ldCore/Visualizations/MusicManager/ldMusicManager.h"
#endif
// stub
#include "Sound/ldSoundStubDevice.h"
// midi
#ifdef LD_CORE_ENABLE_MIDI
#include "ldCore/Sound/Midi/ldMidiInfo.h"
#include "ldCore/Sound/Midi/ldMidiInput.h"
#endif

#include "Sound/ldMidiDevice.h"

void ldSoundDeviceManager::registerMetaType()
{
#ifdef LD_CORE_ENABLE_MIDI
   ldMidiDevice::registerMetaType();
#endif
}

ldSoundDeviceManager::ldSoundDeviceManager(QObject *parent)
    : ldSoundInterface(parent)
    , m_qaudioInputDevice(new ldQAudioInputDevice(this))
#ifdef LD_CORE_ENABLE_MIDI
    , m_midiDevice(new ldMidiDevice(this))
#endif
    , m_stubDevice(new ldSoundStubDevice(this))
{
    qDebug() << __FUNCTION__;

    connect(m_qaudioInputDevice, &ldQAudioInputDevice::soundUpdated, this, &ldSoundDeviceManager::soundUpdated);
    connect(m_qaudioInputDevice, &ldQAudioInputDevice::error, this, &ldSoundDeviceManager::error);
#ifdef LD_CORE_ENABLE_MIDI
    connect(m_midiDevice, &ldMidiDevice::soundUpdated, this, &ldSoundDeviceManager::soundUpdated);
#endif
    connect(m_stubDevice, &ldSoundStubDevice::soundUpdated, this, &ldSoundDeviceManager::soundUpdated);

    //
    refreshAvailableDevices();

//    QList<ldSoundDeviceInfo> availableDevices = getAvailableDevices(ldSoundDeviceInfo::Type::QAudioInput);
//    if(!availableDevices.isEmpty()) {
//        setDeviceInfo(getDefaultDevice(ldSoundDeviceInfo::Type::QAudioInput));
//    } else {
//        qDebug() << "No audio input device found";
//    }
}

ldSoundDeviceManager::~ldSoundDeviceManager()
{
    deleteAudioInput();
}

QAudioFormat ldSoundDeviceManager::getAudioFormat() const
{
    return m_format;
}

void ldSoundDeviceManager::refreshAvailableDevices()
{
    m_devices.clear();

    QList<QAudioDeviceInfo> inputAudioDevices = ldQAudioInputDevice::getDevices();
    for(const QAudioDeviceInfo &inputAudioDevice : inputAudioDevices) {
        m_devices.append(ldSoundDeviceInfo(ldSoundDeviceInfo::Type::QAudioInput, inputAudioDevice.deviceName()));
    }

#ifdef LD_LOOPBACK_DEVICE_ENABLED
    QStringList loopbackDevices = ldLoopbackAudioDevice::getAvailableOutputDevices();
    for(const QString &loopbackDevice : loopbackDevices) {
        m_devices.append(ldSoundDeviceInfo(ldSoundDeviceInfo::Type::Loopback,
                                           loopbackDevice));
    }
#endif

#ifdef LD_CORE_ENABLE_MIDI
    // midi devices
    QList<ldMidiInfo> midiInfos = m_midiDevice->input()->getDevices();
    for(const ldMidiInfo &midiInfo : midiInfos) {
        m_devices.append(midiInfo.toSoundDeviceInfo());
    }
#endif

    m_devices.append(ldSoundDeviceInfo(ldSoundDeviceInfo::Type::Stub,
                                       tr("Stub"))
                     );
}

QList<ldSoundDeviceInfo> ldSoundDeviceManager::getAvailableDevices(const ldSoundDeviceInfo::Type &type) const
{
    QList<ldSoundDeviceInfo> result;
    for(const ldSoundDeviceInfo &device : m_devices) {
        if(type & device.type() || type == ldSoundDeviceInfo::Type::None) {
            result << device;
        }
    }
    return result;
}

ldSoundDeviceInfo ldSoundDeviceManager::getDefaultDevice(const ldSoundDeviceInfo::Type &type) const
{
    ldSoundDeviceInfo result;

    if(type == ldSoundDeviceInfo::Type::QAudioInput) {
        result = ldSoundDeviceInfo(type, m_qaudioInputDevice->getDefaultDevice().deviceName());
    } else {
        QList<ldSoundDeviceInfo> devices = getAvailableDevices(type);
        if(!devices.empty())
            result = devices.first();
    }

    return result;
}

ldSoundDeviceInfo ldSoundDeviceManager::getDeviceInfo() const
{
    return m_info;
}

void ldSoundDeviceManager::setDeviceInfo(const ldSoundDeviceInfo &info)
{
    if(m_info == info) {
        return;
    }

    if(initializeAudio(info)) {
        m_info = info;
    }
}

void ldSoundDeviceManager::setActivateCallbackFunc(ldActivateCallbackFunc func)
{
    m_activateCallbackFunc = func;
}

void ldSoundDeviceManager::notified()
{
    qDebug() << "notified";
}


#ifdef LD_LOOPBACK_DEVICE_ENABLED
void ldSoundDeviceManager::activateOutputDevice(ldSoundDeviceInfo info)
{
//    m_info = info;// info.deviceName();

    qDebug() << "using sound output device: " << info.name();

    // TODO: fill with correct loopback options
    m_format = getDefaultAudioFormat();

    deleteAudioInput();

    m_loopbackAudioDevice = new ldLoopbackAudioDevice(info.name(), this);
    connect(m_loopbackAudioDevice, SIGNAL(soundUpdated(const char*,qint64)), this, SIGNAL(soundUpdated(const char*,qint64)));
    connect(m_loopbackAudioDevice, &ldLoopbackAudioDevice::error, this, &ldSoundDeviceManager::error);
    m_loopbackAudioDevice->startCapture();
}
#endif

void ldSoundDeviceManager::deleteAudioInput()
{
    if(m_isStopping) return;

    m_isStopping = true;

    if(m_qaudioInputDevice->isActive()) {
        m_qaudioInputDevice->stop();
    }

#ifdef LD_LOOPBACK_DEVICE_ENABLED
    if(m_loopbackAudioDevice) {
        disconnect(m_loopbackAudioDevice, SIGNAL(soundUpdated(const char*,qint64)), this, SIGNAL(soundUpdated(const char*,qint64)));
        m_loopbackAudioDevice->stopCapture();
        m_loopbackAudioDevice->deleteLater();
        m_loopbackAudioDevice = NULL;
    }

#endif

#ifdef LD_CORE_ENABLE_MIDI
    m_midiDevice->stop();
#endif

    m_stubDevice->stop();

    m_isStopping = false;
}

bool ldSoundDeviceManager::initializeAudio(const ldSoundDeviceInfo &info)
{
	deleteAudioInput();

	bool isSuccess = false;

	// find device
    switch (info.type()) {
    case ldSoundDeviceInfo::Type::QAudioInput:
    {
        isSuccess = activateQAudioInputDevice(info);
        break;
    }
    case ldSoundDeviceInfo::Type::Loopback:
    {
#ifdef LD_LOOPBACK_DEVICE_ENABLED
        // try look for loopback device
        if(ldLoopbackAudioDevice::getAvailableOutputDevices().contains(info.name())) {
            activateOutputDevice(info);
            isSuccess = true;
        }
#endif
        break;
    }
#ifdef LD_CORE_ENABLE_MIDI
    case ldSoundDeviceInfo::Type::Midi:
    {
        activateMidiDevice(info);
        isSuccess = true;
        break;
    }
#endif
    case ldSoundDeviceInfo::Type::Stub:
    {
        activateStubDevice(info);
        isSuccess = true;
        break;
    }
    case ldSoundDeviceInfo::Type::None:
    {
        isSuccess = true;
        break;
    }
    }

	if (!isSuccess) {
		// show error - nothing found
        qWarning() << "show error - nothing found";
        emit error(tr("Error, can't open: %1").arg(info.name()));
    } else {
        qDebug() << __FUNCTION__ << info.name() << info.type();
    }

    if(isSuccess && m_activateCallbackFunc) {
        m_activateCallbackFunc(info);
    }

    return isSuccess;
}

bool ldSoundDeviceManager::activateQAudioInputDevice(const ldSoundDeviceInfo &info)
{
    QList<QAudioDeviceInfo> inputDevices = ldQAudioInputDevice::getDevices();

    auto it = std::find_if(inputDevices.begin(), inputDevices.end(), [&](const QAudioDeviceInfo &inputDevice) {
        return inputDevice.deviceName() == info.name();
    });

    if (it != inputDevices.end()) {
        bool isOk = m_qaudioInputDevice->activateInputDevice(*it);
        if(isOk) {
            m_format = m_qaudioInputDevice->format();
        } else {
            deleteAudioInput();
        }
        return isOk;
    }

    return false;
}

#ifdef LD_CORE_ENABLE_MIDI
void ldSoundDeviceManager::activateMidiDevice(ldSoundDeviceInfo info) {

    // dummy options for pcm data
    m_format = getDefaultAudioFormat();
    m_midiDevice->start(info.id().value<ldMidiInfo>());

    qDebug() << "midi on";
}
#endif


void ldSoundDeviceManager::activateStubDevice(ldSoundDeviceInfo /*info*/)
{
    // dummy options for pcm data
    m_format = getDefaultAudioFormat();
    m_stubDevice->start();
}

