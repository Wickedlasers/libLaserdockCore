//
//  ldLoopbackAudioDevice.cpp
//  ldCore
//
//  Created by Sergey Gavrushkin on 09/08/16.
//  Copyright (c) 2016 Wicked Lasers. All rights reserved.
//

#include "ldLoopbackAudioDevice.h"

#include <QtCore/QCoreApplication>

#include <QtConcurrent/QtConcurrent>
#include <QtMultimedia/QAudioDeviceInfo>

#include <ldCore/Sound/ldSoundData.h>

namespace {
QByteArray jbyteArray2QByteArray(JNIEnv *env, jbyteArray buf)
{
    int len = env->GetArrayLength(buf);
    QByteArray array;
    array.resize(len);
    env->GetByteArrayRegion (buf, 0, len, reinterpret_cast<jbyte*>(array.data()));
    return array;
}
}


/*!
  \class ldLoopbackAudioDevice
  \brief Actual class which represents the default output for the underlying platform.
  \inmodule audio

  This class is used to analyze a segment of audio data from output interface and extract relevant data from it.
  This data can then be used to generate live visualization.
*/

ldLoopbackAudioDevice *ldLoopbackAudioDevice::instance()
{
    static ldLoopbackAudioDevice instance;
    return &instance;
}

QStringList ldLoopbackAudioDevice::getAvailableOutputDevices()
{
    if(ldLoopbackAudioDevice::instance()->isValid())
        return QStringList {tr("Loopback")};
    else
        return QStringList();
}

ldLoopbackAudioDevice::ldLoopbackAudioDevice(QObject *parent)
    : ldSoundInterface(parent)
    , m_visualizer("com/wickedlasers/laserOS/LdVisualizerController")
{
    if(!m_visualizer.isValid()) {
        qWarning() << __FUNCTION__ << "ldLoopbackAudioDevice is not available";
        return;
    }
}


ldLoopbackAudioDevice::~ldLoopbackAudioDevice()
{
}

QAudioFormat ldLoopbackAudioDevice::getAudioFormat() const
{
    QAudioFormat format;
    format.setSampleRate(SAMPLE_RATE); // 44100
    format.setChannelCount(1);
    format.setSampleSize(8);
    format.setSampleType(QAudioFormat::UnSignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setCodec("audio/pcm");
    return format;
}

bool ldLoopbackAudioDevice::isValid() const
{
    return m_visualizer.isValid();
}

void ldLoopbackAudioDevice::sendSound(const QByteArray &waveform)
{
    emit soundUpdated(waveform.constData(), waveform.length());
}

bool ldLoopbackAudioDevice::setEnabled(bool enabled)
{
    if(!m_visualizer.isValid()) {
        qWarning() << __FUNCTION__ << "You need to setup Visualizer class in Java first";
        return false;
    }

    jint res = m_visualizer.callMethod<jint>("setEnabled",
                              "(Z)I",
                              enabled
                              );

    if(res != 0) {
        qWarning() << __FUNCTION__ << "Can't call setEnabled function" << res;
        return false;
    }
    return true;
}

// byte[] wavefor, int samplingRate
JNIEXPORT void JNICALL Java_com_wickedlasers_laserOS_LdVisualizerController_onWaveFormDataCaptured(JNIEnv *env, jobject instance, jbyteArray jWaveform, jint jSamplingRate)
{
    Q_UNUSED(env)
    Q_UNUSED(instance)
    Q_UNUSED(jSamplingRate)

    QByteArray waveform = jbyteArray2QByteArray(env, jWaveform);
    ldLoopbackAudioDevice::instance()->sendSound(waveform);
}
