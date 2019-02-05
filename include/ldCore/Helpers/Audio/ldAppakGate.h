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

// ldAppakGate.h

#ifndef LDAPPAKGATE_H
#define LDAPPAKGATE_H

#include <ldCore/Sound/ldSoundData.h>

class LDCORESHARED_EXPORT ldAppakGate
{
public:
	ldAppakGate();
	~ldAppakGate();
	
    void basicMono(float mono);

    bool isSilent() const;
    float average() const;
    float maxAmpDiff() const;
    float debug() const;
    float maxAverage() const;
    float minAverage() const;

private:
    static const int m_buffersize = 1*20*AUDIO_OVERDRIVE_FACTOR; // fps on 1s 1*30*AUDIO_OVERDRIVE_FACTOR
    static const int m_last_i = m_buffersize-1;
    static const int m_waitingTimer= 3*10*AUDIO_OVERDRIVE_FACTOR;
    int m_waitingTimerCounter = 0;
    bool m_previousIsSilent = false;
    
    float m_bufferData[m_buffersize];
    bool m_isStarted = false;
    int m_startCounter = 0;

    bool m_isSilent = true;
    float m_average = 0.0f;
    float m_maxAmpDiff = 0.0f;
    float m_debug = 0.0f;
    float m_maxAverage = 0;
    float m_minAverage = 10000000000;

};

#endif // LDAPPAKGATE_H


