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

// ldAudioBasic.h
//  Created by Eric Brugère on 4/12/16.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.

#ifndef LDAUDIOBASIC_H
#define LDAUDIOBASIC_H

#include <ldCore/Sound/ldSoundData.h>

class LDCORESHARED_EXPORT ldAudioBasic
{
public:
    ldAudioBasic();
    ~ldAudioBasic();

    void process(ldSoundData* pSoundData);
    
    //
    float stereoLeft;
    float stereoRight;
    float mono;
    //
    
    //
    std::vector<float> freq16bands;
    std::vector<float> freq128bands;
    std::vector<float> frequencies;

    // values from volume correction system
    float powerRaw = 0.0f; // sound energy before volume correction
    float powerNorm = 0.0f; // sound energy after normalized by volume correction
    
protected:
private:
    
    // buffer size for mono, stereoLeft, stereoRight normalization
    static const int buffersize = 1*30*AUDIO_OVERDRIVE_FACTOR; // fps on 1s 1*30*AUDIO_OVERDRIVE_FACTOR
    
    std::vector<std::vector<float> > buffers;
    
};



#endif // LDAUDIOBASIC_H


