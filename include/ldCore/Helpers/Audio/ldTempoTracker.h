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
//  ldTempoTracker.h
//  ldCore
//
//  Created by feldspar on 2/7/15.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//

#ifndef __ldCore__ldTempoTracker__
#define __ldCore__ldTempoTracker__

#include <memory>

#include <aubio/src/aubio.h>

#include <ldCore/Sound/ldSoundData.h>

class LDCORESHARED_EXPORT ldTempoTracker {

public:
    explicit ldTempoTracker(char* _algorithm = (char*)"default", bool _fastBeats = true, bool _allowPartialBeats = true, float _newBeatConfidenceCutoff = 0);
    virtual ~ldTempoTracker();

    float process(ldSoundData* pSoundData);

    float output() const;
    float confidence() const;
    float bpm() const;

private:
    // options
    // string for algorithm selection "default" for default
    char* algorithm;

    // notes on algorithms:
    //"specflux" is defailt, looks good and is stable
    //"rolloff" looks good and is more expressive but less consistent
    //"specdiff" and "phase" seem to work decently but no better than default

    // turn this on for faster beats
    bool fastBeats;
    // register a new beat even if the old beat is still fading out
    bool allowPartialBeats;
    // beats below this confidence level are ignored
    float newBeatConfidenceCutoff;

    // 0-1 value to prefer slow-fast tempos.  not yet implemented.
    //float speed;

    int hop_size;
    int overdriveSkip;
    float oldbeat;
    aubio_tempo_t* m_aubioTempoDetector;
    float fade;


    float m_output = 0;
    float m_confidence = 0;
    float m_bpm = 0;
};



#endif /* defined(__ldCore__ldTempoTracker__) */
