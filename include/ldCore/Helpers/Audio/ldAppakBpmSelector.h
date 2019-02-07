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

// ldAppakBpmSelector.h

#ifndef ldAppakBpmSelector_H
#define ldAppakBpmSelector_H

// include
#include <ldCore/Sound/ldSoundData.h>

class LDCORESHARED_EXPORT ldAppakBpmSelector
{
public:
    ldAppakBpmSelector();
    ~ldAppakBpmSelector();
	
    void process(float aubioFastBpm, float appakaBeatBpm, float appakaPeakBpm);
    float bestBpm = 120;

private:
    void doStats();

    float lastTrustableBPM = 0;
    float lastSomehowTrustableBPM = 0;
    float lastVeryTrustableBPM = 0;
//    float trustableTime = 0;

    static const int fps = 30*AUDIO_OVERDRIVE_FACTOR;
    //static const int seconds = 1;
    static const int buffersize = fps;//seconds*fps;
    float bpmAubio[buffersize];
    float bpmAppakBeat[buffersize];
    float bpmAppakPeak[buffersize];


};





#endif // ldAppakBpmSelector_H


