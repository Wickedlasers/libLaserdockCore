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

#ifndef LDLOOPBACKAUDIODEVICE_H
#define LDLOOPBACKAUDIODEVICE_H

#include <jni.h>

#include <QtAndroidExtras/QAndroidJniObject>

#include <ldCore/Sound/ldSoundInterface.h>

class ldLoopbackAudioDevice : public ldSoundInterface
{
    Q_OBJECT

public:
    static QStringList getAvailableOutputDevices();

    static ldLoopbackAudioDevice *instance();

    virtual ~ldLoopbackAudioDevice();

    // ldSoundInterface
    virtual QAudioFormat getAudioFormat() const override;

    bool isValid() const;

public slots:
    bool setEnabled(bool enabled);
    void sendSound(const QByteArray &waveform);

signals:
    void error(QString error);

private:
    explicit ldLoopbackAudioDevice(QObject *parent = nullptr);

    QAndroidJniObject m_visualizer;
};

extern "C" {
JNIEXPORT void JNICALL Java_com_wickedlasers_laserOS_LdVisualizerController_onWaveFormDataCaptured(JNIEnv *env, jobject instance, jbyteArray jWaveform, jint jSamplingRate);
}


#endif // LDLOOPBACKAUDIODEVICE_H
