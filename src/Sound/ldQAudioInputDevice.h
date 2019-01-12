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

#include <QtCore/QObject>
#include <QtCore/QByteArray>

#include <ldCore/Sound/ldSoundInterface.h>

//
// ldQAudioInputDevice
// this device uses QAudioInputs as source
//
class ldQAudioInputDevice : public ldSoundInterface
{
    Q_OBJECT
public:
    static QList<QAudioDeviceInfo> getDevices();
    static QAudioDeviceInfo getDefaultDevice();

    explicit ldQAudioInputDevice(QObject *parent = nullptr);
    ~ldQAudioInputDevice();

    virtual QAudioFormat getAudioFormat() const override { return m_format; }

    bool isActive() const;

    QAudioFormat format() const;

public slots:
    bool activateInputDevice(const QAudioDeviceInfo &info);
    void stop();

signals:
    void error(QString error);

private slots:
    void onInputReadyRead();
    void onAudioInputStateChanged(QAudio::State state);

private:
    QByteArray m_buffer;

    QAudioFormat m_format;

    QScopedPointer<QAudioInput> m_audioInput;
    QScopedPointer<QIODevice> m_input;
};

#endif //LDQAUDIOINPUTDEVICE_H

