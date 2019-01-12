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

#ifndef LDSOUNDEFFECTS_H
#define LDSOUNDEFFECTS_H

#include <unordered_map>

#include "ldCore/ldCore_global.h"

#include "ldQSound.h"

class LDCORESHARED_EXPORT ldSoundEffects
{
public:
    explicit ldSoundEffects();
    virtual ~ldSoundEffects();

    ldSoundEffects(const ldSoundEffects&) = delete;
    ldSoundEffects& operator=(const ldSoundEffects&) = delete;

    void insert(int sfx, const QString &resourcePath);

    void play(int sfx);
    void stop(int sfx);
    void setLoops(int sfx, int loops);

    void setSoundEnabled(bool enabled);
    void setSoundLevel(int soundLevel);

    ldQSound* operator [](int i);

private:
    std::unordered_map<int, std::unique_ptr<ldQSound>> m_soundMap;

    bool m_enabled = true;
};

#endif // LDSOUNDEFFECTS_H
