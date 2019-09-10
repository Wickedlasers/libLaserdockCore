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

// ldAudioBasic.cpp
//  Created by Eric Brugère on 4/12/16.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.

#include "ldCore/Helpers/Audio/ldAudioBasic.h"
#include <QtCore/QDebug>

#include "ldCore/Helpers/Maths/ldMaths.h"

// indexes for buffer
#define AUDIO_DATA_STEREO_LEFT 0
#define AUDIO_DATA_STEREO_RIGHT 1

// ldAudioBasic
ldAudioBasic::ldAudioBasic() {
    mono = 0;
    stereoLeft = 0;
    stereoRight = 0;
    //
    buffers.clear();
//    buffers.resize(buffersize);
    for (int ad=0; ad<2; ad++) {
        buffers.push_back(std::vector<float>());
        for (int i=0; i<buffersize; i++) {
            buffers[ad].push_back(0.0);
        }
    }
    //
//    freq16bands.resize(16);
//    freq128bands.resize(128);
    for (int i=0; i<16; i++) {
        freq16bands.push_back(0.0);
    }
    for (int i=0; i<128; i++) {
        freq128bands.push_back(0.0);
    }
    for (int i=0; i<SAMPLE_SIZE/2; i++) {
        frequencies.push_back(0.0);
    }


}

// ~ldAudioBasic
ldAudioBasic::~ldAudioBasic() {
    freq16bands.clear();
    freq128bands.clear();
    // clear buffers
    for (auto it : buffers) it.clear();
    buffers.clear();
}

// process
void ldAudioBasic::process(ldSoundData* pSoundData)
{
    if (!pSoundData) return;
    
    
    // history
    for (uint c=0; c < buffers.size(); c++) {
        for (uint i=1; i<buffers[c].size(); i++) {
            buffers[c][i-1] = buffers[c][i];
        }
    }
    
    // feed
    for (uint c=0; c < buffers.size(); c++) {
        size_t s = (int) buffers[c].size()-1;
        switch (c) {
            case AUDIO_DATA_STEREO_LEFT:
                buffers[c][s] = pSoundData->GetWaveformL();
                break;
            case AUDIO_DATA_STEREO_RIGHT:
                buffers[c][s] = pSoundData->GetWaveformR();
                break;
            default: break;
        }
    }
    
    // compile error?
    //float maxLeft = *max_element(begin(buffers[AUDIO_DATA_STEREO_LEFT]), end(buffers[AUDIO_DATA_STEREO_LEFT]));
    //float maxRight = *max_element(begin(buffers[AUDIO_DATA_STEREO_RIGHT]), end(buffers[AUDIO_DATA_STEREO_RIGHT]));

    float maxLeft  = *max_element((buffers[AUDIO_DATA_STEREO_LEFT] ).begin(),
                                  (buffers[AUDIO_DATA_STEREO_LEFT] ).end());
    float maxRight = *max_element((buffers[AUDIO_DATA_STEREO_RIGHT]).begin(),
                                  (buffers[AUDIO_DATA_STEREO_RIGHT]).end());

    
    if (maxLeft < 0.1) maxLeft = 0.1f;
    if (maxRight < 0.1) maxRight = 0.1f;
    
    // stereo
    stereoLeft = pSoundData->GetWaveformL()/maxLeft;
    stereoRight = pSoundData->GetWaveformR()/maxRight;
    
    // mono
    mono = 0.5*(stereoLeft+stereoRight);
    
    //qDebug() << "maxLeft " << maxLeft << "     mono " << mono;
    
    // frequencies 16 bands
    for (int i=0; i<16; i++) {
        freq16bands[i] = pSoundData->powerAt(i,16);
    }
    
    // frequencies 128 bands
    for (int i=0; i<128; i++) {
        freq128bands[i] = pSoundData->powerAt(i,128);
    }

    // all bins frequencies
    pSoundData->GetFullFFT(frequencies, true, false); // bool normalize, bool weight
    


    powerRaw = pSoundData->volumePowerPre;
    powerNorm = pSoundData->volumePowerPost;
    
    //
}



