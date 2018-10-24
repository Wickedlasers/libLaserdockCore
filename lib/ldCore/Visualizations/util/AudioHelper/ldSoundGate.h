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

#ifndef LDSOUNDGATE_H
#define LDSOUNDGATE_H

#include <ldCore/Sound/ldSoundData.h>

class LDCORESHARED_EXPORT ldSoundGate
{
public:
    ldSoundGate();
    ~ldSoundGate();

    void process(ldSoundData* pSoundData);

    bool isSilent = true;
    float sliderValue,previouSliderValue;
    float computedCeilValue;
    float maxLevel;
    float minLevel;

private:
    static const int buffersize = 1*20*AUDIO_OVERDRIVE_FACTOR; // fps on 1s 1*30*AUDIO_OVERDRIVE_FACTOR
    static const int last_i = buffersize-1;

    float bufferData[buffersize];
    bool isStarted;
    int startCounter;

    void computeSliderValue();
};

#endif // LDSOUNDGATE_H
