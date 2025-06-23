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

#ifndef LDQAUDIOINPUTDEVICE_H
#define LDQAUDIOINPUTDEVICE_H

#include <memory>

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#if QT_VERSION >= 0x060000
#include <QtMultimedia/QAudioDevice>
#include <QtMultimedia/QAudioSource>
#else
#include <QtMultimedia/QAudioDeviceInfo>
#endif

#include <ldCore/Sound/ldSoundInterface.h>

class QAudioInput;
class QIODevice;

//
// ldQAudioInputDevice
// this device uses QAudioInputs as source
//
class ldQAudioInputDevice : public ldSoundInterface
{
    Q_OBJECT
public:
#if QT_VERSION >= 0x060000
    static QList<QAudioDevice> getDevices();
    static QAudioDevice getDefaultDevice();
    static void printInfo(const QAudioDevice &inputAudioDevice);
#else
    static QList<QAudioDeviceInfo> getDevices();
    static QAudioDeviceInfo getDefaultDevice();
    static void printInfo(const QAudioDeviceInfo &inputAudioDevice);
#endif

    explicit ldQAudioInputDevice(QObject *parent = nullptr);
    ~ldQAudioInputDevice();

    virtual QAudioFormat getAudioFormat() const override { return m_format; }

    bool isActive() const;

    QAudioFormat format() const;

public slots:
#if QT_VERSION >= 0x060000
    bool activateInputDevice(const QAudioDevice &info);
#else
    bool activateInputDevice(const QAudioDeviceInfo &info);
#endif
    void stop();

signals:
    void error(QString error);

private slots:
    void onInputReadyRead();
    void onAudioInputStateChanged(QAudio::State state);

private:
    QByteArray m_buffer;

    QAudioFormat m_format;

#if QT_VERSION >= 0x060000
    std::unique_ptr<QAudioSource> m_audioInput;
#else
    std::unique_ptr<QAudioInput> m_audioInput;
#endif
    std::unique_ptr<QIODevice> m_input;
};

#endif //LDQAUDIOINPUTDEVICE_H

