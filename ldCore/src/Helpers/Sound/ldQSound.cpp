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

#include "ldCore/Helpers/Sound/ldQSound.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtCore/QtDebug>
#include <QtMultimedia/QSoundEffect>

ldQSound::ldQSound(const QString &filename)
    : QObject()
    , m_filename(filename)
{
#if !defined(Q_OS_IOS) && !defined(Q_OS_ANDROID)
    setVolumeCoeff(0.5); // decrease game volume
#endif

    // FIXME
    // uncomment these lines to debug missing sounds
    // QSoundEffect *soundEffect = new QSoundEffect(this);
    // soundEffect->setSource(QUrl::fromLocalFile(m_filename));

    // sound/wickedDungeon/win2.wav
    // sound/bubbleLase/gameOver.wav
    // sound/astbonusm_ship.wav
    // sound/robotron/enforcerDie.wav
    // sound/laserPeg/gameOver.wav
}

ldQSound::~ldQSound()
{
    stopImpl();
}

bool ldQSound::isPlaying() const
{
    for (QSoundEffect *sound : m_sounds) {
        if (sound->isPlaying())
            return true;
    }

    return false;
}

void ldQSound::play()
{
    // no need for mutex here
    // QMutexLocker lock(&m_mutex);
    QMetaObject::invokeMethod(this, "playImpl", Qt::QueuedConnection);
}

void ldQSound::stop()
{
    // no need for mutex here
    // QMutexLocker lock(&m_mutex);
    QMetaObject::invokeMethod(this, "stopImpl", Qt::QueuedConnection);
}

void ldQSound::setForce(bool force)
{
    QMutexLocker lock(&m_mutex);
    m_force = force;
}

void ldQSound::setLoops(int loops)
{
    QMutexLocker lock(&m_mutex);
    m_loops = loops;
    for (QSoundEffect *sound : m_sounds) {
        sound->setLoopCount(m_loops);
    }
}

void ldQSound::setVolumeCoeff(qreal volume)
{
    QMutexLocker lock(&m_mutex);
    m_volumeCoeff = volume;
    for(QSoundEffect *soundEffect : m_sounds) {
        soundEffect->setVolume(m_volumeCoeff * ((double) m_volumeLevel / 100.0));
    }
}

void ldQSound::setVolumeLevel(int level)
{
    QMutexLocker lock(&m_mutex);
    m_volumeLevel = level;

    for(QSoundEffect *soundEffect : m_sounds) {
        soundEffect->setVolume(m_volumeCoeff * ((double) m_volumeLevel / 100.0));
    }
}

void ldQSound::playImpl()
{
    QMutexLocker lock(&m_mutex);

    // create new sound cache
    if(m_sounds.empty()) {
        // Qt:
        // Since QSoundEffect requires slightly more resources to achieve lower latency playback,
        // the platform may limit the number of simultaneously playing sound effects.
        for(int i = 0; i < 5; i++) {
            QSoundEffect *soundEffect = new QSoundEffect(this);
            soundEffect->setSource(QUrl::fromLocalFile(m_filename));
            soundEffect->setLoopCount(m_loops);
            soundEffect->setVolume(m_volumeCoeff * ((double) m_volumeLevel / 100.0));
            m_sounds.push_back(soundEffect);
        }
    }

    // allow only one sound
    if(!m_force
        && isPlaying()
        ) {
        return;
    }

    // play
    for(QSoundEffect *soundEffect : m_sounds) {
        if(!soundEffect->isPlaying()) {
            soundEffect->play();
            break;
        }
    }
}

void ldQSound::stopImpl()
{
    QMutexLocker lock(&m_mutex);

    for(QSoundEffect *sound : m_sounds) {
        if(sound->isPlaying()) {
            sound->stop();
        }
    }
}
