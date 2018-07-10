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

//
//  ldAppakSpectrum.h
//
//  Created by Eric Brugère on 2/10/15.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//
#ifndef ldAppakSpectrum_H
#define ldAppakSpectrum_H

#include <ldCore/Sound/ldSoundData.h>

struct LDCORESHARED_EXPORT AppakSpectrumStruct {
    float binHzOne; // (x+1)*30 to have Hz
    float binHzTwo;
    float binHzThree;
};

//
class LDCORESHARED_EXPORT ldAppakSpectrum
{
public:
    ldAppakSpectrum();
    ~ldAppakSpectrum();

    static const int fps = 30*AUDIO_OVERDRIVE_FACTOR;
    static const int seconds = 2;
    static const int buffersize = seconds*fps;
    std::vector<float> frequencies;

    int frameDropper;

    void process(ldSoundData* pSoundData);
    void doSprectrumStats();
    int getMaxBinForHzInterval(float startHz, float endHz);
    AppakSpectrumStruct resSpectrum;

private:

};

#endif // ldAppakSpectrum_H
