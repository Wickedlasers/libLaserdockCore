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
//  ldOnsetDetector.h
//  ldCore
//
//  Created by feldspar on 3/22/15.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//

#ifndef __ldCore__ldOnsetDetector__
#define __ldCore__ldOnsetDetector__

#include <aubio/src/aubio.h>

#include <ldCore/Sound/ldSoundData.h>

class LDCORESHARED_EXPORT OnsetDetector {
public:    
    OnsetDetector(char* _algorithm = (char*)"specflux", float _gapTime = 0.35, float _decayTime = 0, float _threshold = 0.5);
    float process(ldSoundData* pSoundData);
    float output;

private:
    char* algorithm;
    float gapTime;
    float decayTime;
    float threshold;

    aubio_onset_t* o;
    int hop_size;
    int overdriveSkip;
};

#endif /* defined(__ldCore__ldOnsetDetector__) */
