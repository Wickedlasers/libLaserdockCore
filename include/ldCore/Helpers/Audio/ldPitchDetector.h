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
//  ldPitchDetector.h
//  ldCore
//
//  Created by feldspar on 3/14/15.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//

#ifndef __ldCore__ldPitchDetector__
#define __ldCore__ldPitchDetector__

#include <ldCore/Sound/ldSoundData.h>

#include <aubio/src/aubio.h>

class LDCORESHARED_EXPORT PitchDetector {
public:
    PitchDetector(int type = 0);
    float process(ldSoundData* pSoundData);
    float output;
    float confidence;
    float note;
    float noteConfidence;
    float lastpitch;
    float lastnote;
    aubio_pitch_t * m_aubioPitch;
    float semitones[24];
};


#endif /* defined(__ldCore__ldPitchDetector__) */
