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

#include "ldCore/Sound/ldSoundInterface.h"

/*!
  \class ldSoundInterface
  \brief Interface for an audio source
  \inmodule audio
*/

#include <QtCore/QDebug>
#include <QtCore/QTimer>

#include <ldCore/Sound/ldSoundData.h>

QAudioFormat ldSoundInterface::getDefaultAudioFormat()
{
    QAudioFormat format;
    format.setSampleRate(SAMPLE_RATE); // 44100
    format.setChannelCount(2); // 2
    format.setSampleSize(16);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setCodec("audio/pcm");
    return format;
}

ldSoundInterface::ldSoundInterface(QObject *parent) :
    QObject(parent)
{
}

ldSoundInterface::~ldSoundInterface()
{
}

QAudioFormat ldSoundInterface::getAudioFormat() const
{
    return getDefaultAudioFormat();
}


