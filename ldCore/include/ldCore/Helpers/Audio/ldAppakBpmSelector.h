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

    float bestBpm() const;

private:
    static const int BUFFER_FPS = 30.f*AUDIO_OVERDRIVE_FACTOR;
    static const int BUFFER_SECONDS = 1;
    static const int BUFFER_SIZE = BUFFER_SECONDS * BUFFER_FPS;

    void doStats();

    float m_lastTrustableBPM = 0;
    float m_lastSomehowTrustableBPM = 0;
    float m_lastVeryTrustableBPM = 0;
//    float trustableTime = 0;

    float m_bpmAubio[BUFFER_SIZE];
    float m_bpmAppakBeat[BUFFER_SIZE];
    float m_bpmAppakPeak[BUFFER_SIZE];

    float m_bestBpm = 60.f;
};





#endif // ldAppakBpmSelector_H


