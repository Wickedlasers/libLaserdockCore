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
//  ldPitchDetector.cpp
//  ldCore
//
//  Created by feldspar on 2/7/15.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//

/*!

  PitchDetector class.

  Provides a tempo tracking value based on aubio tempo beat tracker classes.
  When a beat is detected, the output value is set to 1, and then decays over
  time based on the bpm.  This produces a triangle-wave style signal.

  Call process() to feed in new data.

  Read member variable "output" to get result.


*/

#include <QtCore/QDebug>

#include "ldCore/Helpers/Audio/ldPitchDetector.h"

static const uint_t samplerate = 44100;
//static const uint_t win_size = AUDIO_BLOCK_SIZE/2; // mono
static const uint_t hop_size = AUDIO_SUB_BLOCK_SIZE/2; // mono

PitchDetector::PitchDetector(int type) {
    output = 0;
    confidence = 0;
    note = 0;
    noteConfidence = 0;
    lastpitch = 1;
    lastnote = 0;

    for (int i = 0; i < 24; i++) semitones[i] = 0;

    char t0[] = "default";
    char t1[] = "mcomb";
    char t2[] = "yinfft";
    char t3[] = "yin";
    char t4[] = "schmitt";
    char t5[] = "fcomb";
    char t6[] = "specacf";
    char* tt[7] = {t0,t1,t2,t3,t4,t5,t6};
    type = std::min(std::max(type, 0), 6);
    m_aubioPitch = new_aubio_pitch(tt[type], (type==3)?1024:4096, hop_size, samplerate);
    //m_aubioPitch = new_aubio_pitch("default", 4096, hop_size, samplerate);
}

float PitchDetector::process(ldSoundData* pSoundData) {

    // create some vectors
    fvec_t * in = new_fvec (hop_size); // input audio buffer
    fvec_t * out = new_fvec (1); // output pitch
    aubio_pitch_t * o = m_aubioPitch;

    // put some fresh data in input vector
    for (uint i = 0; i < hop_size; i++) {
        float samp = pSoundData->GetWaveformL(i) + pSoundData->GetWaveformR(i);
        fvec_set_sample(in, 8*samp, i);
    }

    // execute pitch
    aubio_pitch_do(o,in,out);
    confidence = aubio_pitch_get_confidence(o);
    output = out->data[0];

    // get note value
    note = log(output)/log(2);
    //note *= 2;
    note -= (int) note;

    int s = note*24;
    if (!(s >= 0 && s < 24)) s = 0;
    //semitones[s]++;

//    float delta = AUDIO_UPDATE_DELTA_S;
    int max = 0;
    float integral = 0;
    for (int i = 0; i < 24; i++) {
        //semitones[i] -= 0.25*delta;
        //semitones[i] *= powf(0.05, delta);
        semitones[i] *= 0.50;
    }
    semitones[(s+0+24)%24] += 1.0;
    semitones[(s+1+24)%24] += 0.5;
    semitones[(s-1+24)%24] += 0.5;
    for (int i = 0; i < 24; i++) {
        semitones[i] = std::min(std::max(0.f, semitones[i]), 2.f);
        if (semitones[i] > semitones[max]) max = i;
        integral += semitones[i];
    }
    //note = max/24.0f;
    if (integral > 0) noteConfidence = semitones[max]/integral;
    if (max!=s && max!=(s+1+24)%24 && max!=(s-1+24)%24) noteConfidence = 0;



    /*float f = note-lastnote;
    if (f < 0) f = -f;
    if (f > 0.5) f = 1-f;
    if (f>1.0/12.0) {
        lastpitch = output;
        lastnote = note;
    } else {
        output = lastpitch;
        note = lastnote;
    }*/


    // clean up memory
    del_fvec(in);
    del_fvec(out);

    return output;
}

