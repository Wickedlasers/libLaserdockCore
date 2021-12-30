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

#ifndef LDAUDIOFILEDECODER_H
#define LDAUDIOFILEDECODER_H

#include <memory>

#include <QtCore/QElapsedTimer>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QTimer>

#include <ldCore/Sound/ldSoundInterface.h>

#include <QQmlHelpers>
#include <QQmlObjectListModel>


#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
#define Q_OS_LINUX_DESKTOP
#endif

#ifndef Q_OS_LINUX_DESKTOP
#define AUIDIO_DECODER_SUPPORTED
#endif

class QMediaPlayer;

class ldAudioFileDecoderThread;

class LDCORESHARED_EXPORT ldAudioFileDecoder : public QObject
{
    Q_OBJECT

    QML_READONLY_PROPERTY(bool, isActive)

public:
    explicit ldAudioFileDecoder(QObject *parent = nullptr);
    ~ldAudioFileDecoder();

    qint64 duration() const;

public slots:
    void start(const QString &filePath);
    void reset();

signals:
    void finished(const std::vector<float> &data);

private slots:
    void onThreadFinished();

private:
    qint64 m_duration = -1;
    QMutex m_mutex;

    ldAudioFileDecoderThread *m_currentThread = nullptr;
};

#endif //LDAUDIOFILEDECODER_H

