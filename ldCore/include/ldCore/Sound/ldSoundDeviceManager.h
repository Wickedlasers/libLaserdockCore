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

#ifndef LDSOUNDDEVICEMANAGER_H
#define LDSOUNDDEVICEMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QThread>

#include <QQmlHelpers>

#include <ldCore/Sound/ldSoundInterface.h>
#include <ldCore/Sound/ldSoundDeviceInfo.h>

#ifdef _WIN32
#define LD_LOOPBACK_DEVICE_ENABLED
#endif

class ldQAudioInputDevice;
#ifdef LD_CORE_ENABLE_MIDI
class ldMidiDevice;
#endif
class ldSoundStubDevice;

#ifdef LD_LOOPBACK_DEVICE_ENABLED
class ldLoopbackAudioDevice;
#endif

typedef void (*ldActivateCallbackFunc)(const ldSoundDeviceInfo &info);

class LDCORESHARED_EXPORT ldSoundDeviceManager: public ldSoundInterface
{
    Q_OBJECT
public:
    static void registerMetaType();

    explicit ldSoundDeviceManager(QObject *parent);
    ~ldSoundDeviceManager();
    
    // ldSoundInterface
    virtual QAudioFormat getAudioFormat() const override;

    QList<ldSoundDeviceInfo> getAvailableDevices(const ldSoundDeviceInfo::Type &type = ldSoundDeviceInfo::Type::None) const;

    ldSoundDeviceInfo getDefaultDevice(const ldSoundDeviceInfo::Type &type = ldSoundDeviceInfo::Type::None) const;

    ldSoundDeviceInfo getDeviceInfo() const;
    void setDeviceInfo(const ldSoundDeviceInfo &info);

    void setActivateCallbackFunc(ldActivateCallbackFunc func);

signals:
    void error(QString error);

private slots:
    void notified();

    bool initializeAudio(const ldSoundDeviceInfo &info);

private:
    bool activateQAudioInputDevice(const ldSoundDeviceInfo &info);

#ifdef LD_LOOPBACK_DEVICE_ENABLED
    void activateOutputDevice(ldSoundDeviceInfo name);
#endif

#ifdef LD_CORE_ENABLE_MIDI
    void activateMidiDevice(ldSoundDeviceInfo info);
#endif
    void activateStubDevice(ldSoundDeviceInfo info);

    void deleteAudioInput();

    void refreshAvailableDevices();

    bool m_isStopping = false;

    QAudioFormat m_format;

    QList<ldSoundDeviceInfo> m_devices;
    ldSoundDeviceInfo m_info;

    // different devices
    ldQAudioInputDevice *m_qaudioInputDevice;
#ifdef LD_LOOPBACK_DEVICE_ENABLED
    ldLoopbackAudioDevice* m_loopbackAudioDevice = NULL;
#endif
#ifdef LD_CORE_ENABLE_MIDI
    ldMidiDevice* m_midiDevice;
#endif
    ldSoundStubDevice* m_stubDevice;

    ldActivateCallbackFunc m_activateCallbackFunc = nullptr;
};

#endif //LDSOUNDDEVICEMANAGER_H

