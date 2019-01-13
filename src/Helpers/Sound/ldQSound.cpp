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
    setVolumeCoeff(0.5); // decrease game volume
}

ldQSound::~ldQSound()
{
    stopImpl();
}

void ldQSound::play()
{
    QMutexLocker lock(&m_mutex);

    QTimer::singleShot(0, [&]() { playImpl(); });
}

void ldQSound::stop()
{
    QMutexLocker lock(&m_mutex);
    QTimer::singleShot(0, [&]() { stopImpl(); });
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
}

void ldQSound::setVolumeCoeff(qreal volume)
{
    QMutexLocker lock(&m_mutex);
    m_volumeCoeff = volume;
    for(std::unique_ptr<QSoundEffect> &soundEffect : m_sounds) {
        soundEffect->setVolume(m_volumeCoeff * ((double) m_volumeLevel / 100.0));
    }
}

void ldQSound::setVolumeLevel(int level)
{
    QMutexLocker lock(&m_mutex);
    m_volumeLevel = level;

    for(std::unique_ptr<QSoundEffect> &soundEffect : m_sounds) {
        soundEffect->setVolume(m_volumeCoeff * ((double) m_volumeLevel / 100.0));
    }
}

void ldQSound::playImpl()
{
    QMutexLocker lock(&m_mutex);

    // clear finished sounds
    auto soundIt = m_sounds.begin();
    while (soundIt != m_sounds.end()) {
        if (!(*soundIt)->isPlaying())
            soundIt = m_sounds.erase(soundIt);
        else
            ++soundIt;
    }

    // allow only one sound
    if(!m_force && !m_sounds.empty()) {
        return;
    }

    // create new sound
    std::unique_ptr<QSoundEffect> soundEffect(new QSoundEffect());
    soundEffect->setSource(QUrl::fromLocalFile(m_filename));
    soundEffect->setLoopCount(m_loops);
    soundEffect->setVolume(m_volumeCoeff * ((double) m_volumeLevel / 100.0));
    // thread safety play
    soundEffect->play();
    //    QTimer::singleShot(0, soundEffect.get(), static_cast<void (QSound::*)()>(&QSound::play));
    // add to list
    m_sounds.push_back(std::move(soundEffect));
}

void ldQSound::stopImpl()
{
    QMutexLocker lock(&m_mutex);
    for(std::unique_ptr<QSoundEffect> &sound : m_sounds) {
        QCoreApplication::removePostedEvents(sound.get());
    }
    m_sounds.clear();
}
