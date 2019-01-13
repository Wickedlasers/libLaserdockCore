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

#ifndef LDSILENTTHREE_H
#define LDSILENTTHREE_H

#include <ldCore/Sound/ldSoundData.h>
#include "ldCore/Helpers/Maths/ldMathStat.h"

class LDCORESHARED_EXPORT ldSilentThree
{
public:
    ldSilentThree();
    ~ldSilentThree();

    void process(ldSoundData* pSoundData);

    bool isSilent;

private:

    static const int buffersize = 1*20*AUDIO_OVERDRIVE_FACTOR; // fps on 1s 1*30*AUDIO_OVERDRIVE_FACTOR
    static const int last_i = buffersize-1;
    static const int waitingTimer= 3*10*AUDIO_OVERDRIVE_FACTOR;
    int waitingTimerCounter;

    float bufferData[buffersize];
    bool isStarted;
    int startCounter;

    bool previousIsSilent;
    float maxAverage;
    float minAverage;
    float maxAmpDiff;

    //
};

#endif // LDSILENTTHREE_H
