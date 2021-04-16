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

#include <ldCore/Sound/ldSoundAnalyzer.h>

#include "Sound/ldQAudioInputDevice.h"
#include "Sound/ldSoundStubDevice.h"

// loopback
#ifdef LD_LOOPBACK_DEVICE_ENABLED
#ifdef Q_OS_WIN
#include "Sound/win/ldLoopbackAudioDevice.h"
#else
#include "Sound/android/ldLoopbackAudioDevice.h"
#endif
#endif

// midi
#ifdef LD_CORE_ENABLE_MIDI
#include "Sound/ldMidiDevice.h"
#include "ldCore/Sound/Midi/ldMidiInfo.h"
#endif


void ldSoundDeviceManager::registerMetaType()
{
#ifdef LD_CORE_ENABLE_MIDI
   ldMidiDevice::registerMetaType();
#endif
}

ldSoundDeviceManager::ldSoundDeviceManager(QObject *parent)
    : QObject(parent)
    , m_qaudioInputDevice(new ldQAudioInputDevice(this))
#ifdef LD_CORE_ENABLE_MIDI
    , m_midiDevice(new ldMidiDevice(this))
#endif
    , m_stubDevice(new ldSoundStubDevice(this))
    , m_analyzer(new ldSoundAnalyzer(this))
{
    qDebug() << __FUNCTION__;

    connect(m_qaudioInputDevice, &ldQAudioInputDevice::soundUpdated, m_analyzer, &ldSoundAnalyzer::handleSoundUpdated);
    connect(m_qaudioInputDevice, &ldQAudioInputDevice::error, this, &ldSoundDeviceManager::error);
#ifdef LD_CORE_ENABLE_MIDI
    connect(m_midiDevice, &ldMidiDevice::soundUpdated, m_analyzer, &ldSoundAnalyzer::handleSoundUpdated);
#endif
    connect(m_stubDevice, &ldSoundStubDevice::soundUpdated, m_analyzer, &ldSoundAnalyzer::handleSoundUpdated);


    //
    refreshAvailableDevices();

    QString debugStr = "Sound input devices: ";
    for(const ldSoundDeviceInfo &device : m_devices) {
        debugStr += QString::number(device.type()) + " " + device.name() + "; ";
    }
    qDebug().noquote() << debugStr;


    //QList<ldSoundDeviceInfo> availableDevices = getAvailableDevices(ldSoundDeviceInfo::Type::QAudioInput);
    //if(!availableDevices.isEmpty()) {
    //    setDeviceInfo(getDefaultDevice(ldSoundDeviceInfo::Type::QAudioInput));
    //    qDebug() << "Default audio input selected";
    //} else {
    //    qDebug() << "No audio input device found";
    //}
}

ldSoundDeviceManager::~ldSoundDeviceManager()
{
    deleteAudioInput();
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
    QList<ldMidiInfo> midiInfos = m_midiDevice->getDevices();
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

ldSoundAnalyzer* ldSoundDeviceManager::analyzer() const
{
    return m_analyzer;
}

void ldSoundDeviceManager::notified()
{
    qDebug() << "notified";
}


#ifdef LD_LOOPBACK_DEVICE_ENABLED
bool ldSoundDeviceManager::activateOutputDevice(ldSoundDeviceInfo info)
{
    qDebug() << "using sound output device: " << info.name();

    deleteAudioInput();

#ifdef Q_OS_WIN
    m_loopbackAudioDevice = new ldLoopbackAudioDevice(info.name(), this);
#else
    // android has only one loobpack device
    Q_UNUSED(info)
    m_loopbackAudioDevice = ldLoopbackAudioDevice::instance();
    if(!m_loopbackAudioDevice->isValid()) {
        m_loopbackAudioDevice = nullptr;
        return false;
    }
#endif

    if(!m_loopbackAudioDevice->setEnabled(true)) {
        m_loopbackAudioDevice = nullptr;
        return false;
    }


    connect(m_loopbackAudioDevice, &ldLoopbackAudioDevice::soundUpdated, m_analyzer, &ldSoundAnalyzer::handleSoundUpdated);
    connect(m_loopbackAudioDevice, &ldLoopbackAudioDevice::error, this, &ldSoundDeviceManager::error);

    return true;
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
        m_loopbackAudioDevice->setEnabled(false);
#ifdef Q_OS_WIN
        m_loopbackAudioDevice->deleteLater();
#else
        disconnect(m_loopbackAudioDevice, &ldLoopbackAudioDevice::soundUpdated, m_analyzer, &ldSoundAnalyzer::handleSoundUpdated);
        disconnect(m_loopbackAudioDevice, &ldLoopbackAudioDevice::error, this, &ldSoundDeviceManager::error);
#endif
        m_loopbackAudioDevice = nullptr;
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
            isSuccess = activateOutputDevice(info);
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

        // try to activate last device if exists
        if(m_info.isValid()) {
            // avoid infinitie loopback
            ldSoundDeviceInfo oldInfo = m_info;
            m_info = ldSoundDeviceInfo();
            initializeAudio(oldInfo);
        }
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
        if(!isOk) deleteAudioInput();
        return isOk;
    }

    return false;
}

#ifdef LD_CORE_ENABLE_MIDI
void ldSoundDeviceManager::activateMidiDevice(ldSoundDeviceInfo info) {

    // dummy options for pcm data
    m_midiDevice->start(info.id().value<ldMidiInfo>());

    qDebug() << "midi on";
}
#endif


void ldSoundDeviceManager::activateStubDevice(ldSoundDeviceInfo /*info*/)
{
    // dummy options for pcm data
    m_stubDevice->start();
}

