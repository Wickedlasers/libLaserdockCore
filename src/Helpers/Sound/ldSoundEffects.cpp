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

#include "ldCore/Helpers/Sound/ldSoundEffects.h"

ldSoundEffects::ldSoundEffects() {
}

ldSoundEffects::~ldSoundEffects()
{
}

void ldSoundEffects::insert(int sfx, const QString &resourcePath)
{
    m_soundMap[sfx] = std::unique_ptr<ldQSound>(new ldQSound(resourcePath));
}

void ldSoundEffects::play(int sfx)
{
    if(!m_enabled) {
        return;
    }

    m_soundMap[sfx]->play();
}

void ldSoundEffects::stop(int sfx)
{
    m_soundMap[sfx]->stop();
}

void ldSoundEffects::setLoops(int sfx, int loops)
{
    m_soundMap[sfx]->setLoops(loops);
}

void ldSoundEffects::setSoundEnabled(bool enabled)
{
    m_enabled = enabled;
}

void ldSoundEffects::setSoundLevel(int soundLevel)
{
    for(auto &kv : m_soundMap) {
        kv.second->setVolumeLevel(soundLevel);
    }
}

ldQSound *ldSoundEffects::operator [](int i)
{
    return m_soundMap[i].get();
}
