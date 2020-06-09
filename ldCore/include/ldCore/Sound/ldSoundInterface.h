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

#ifndef LDSOUNDINTERFACE_H
#define LDSOUNDINTERFACE_H

#include <QtCore/QObject>
#include <QtMultimedia/QAudioFormat>

#include <ldCore/ldCore_global.h>

class LDCORESHARED_EXPORT ldSoundInterface : public QObject
{
    Q_OBJECT
public:
    static QAudioFormat getDefaultAudioFormat();

    explicit ldSoundInterface(QObject *parent = nullptr);
    ~ldSoundInterface();

    virtual QAudioFormat getAudioFormat() const;

signals:
    // 2 options to process buffer further. Device should send only one of them
    void bufferUpdated(float *convertedBuffer, int framesm, int sampleRate);
    void soundUpdated(const char * data, qint64 len);
};

#endif // LDSOUNDINTERFACE_H
