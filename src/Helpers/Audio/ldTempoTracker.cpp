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
//  ldTempoTracker.cpp
//  ldCore
//
//  Created by feldspar on 2/7/15.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//

/*!

  TempoTracker class.

  Provides a tempo tracking value based on aubio tempo beat tracker classes.
  When a beat is detected, the output value is set to 1, and then decays over
  time based on the bpm.  This produces a triangle-wave style signal.

  Call process() to feed in new data.

  Read member variable "output" to get result.


*/

#include <QtCore/QDebug>

#include "ldCore/Helpers/Audio/ldTempoTracker.h"

static const uint_t samplerate = 44100;
static const uint_t win_size = AUDIO_BLOCK_SIZE/2; // /2 is for convert to mono

ldTempoTracker::ldTempoTracker(char* _algorithm, bool _fastBeats, bool _allowPartialBeats, float _newBeatConfidenceCutoff)
    : algorithm(_algorithm),
      fastBeats(_fastBeats),
      allowPartialBeats(_allowPartialBeats),
      newBeatConfidenceCutoff(_newBeatConfidenceCutoff) {

    if (fastBeats)
        hop_size = win_size/2;
    else
        hop_size = win_size;

    // create tempo object
    m_aubioTempoDetector = new_aubio_tempo(algorithm, win_size, hop_size, samplerate);

    oldbeat = -1;
    fade = 0;

    overdriveSkip = -1;
}

ldTempoTracker::~ldTempoTracker()
{
    del_aubio_tempo(m_aubioTempoDetector);
}


float ldTempoTracker::process(ldSoundData* pSoundData) {

    // update time
    float delta = AUDIO_UPDATE_DELTA_S;

    //static int s = 99999; s++;
    //qDebug() << "called " << s << " with delta " << delta << "and hop size" << hop_size;

    // fade
    m_output -= fade*delta;
    if (m_output < 0) m_output = 0;

    // skip for overdrive factor, we are fixed 30fps
    overdriveSkip++;
    overdriveSkip %= AUDIO_OVERDRIVE_FACTOR;
    if (overdriveSkip) return output();

    // create some vectors
    fvec_t * in = new_fvec (hop_size); // input audio buffer
    fvec_t * out = new_fvec (2); // output position
    fvec_t * out2 = new_fvec (2); // output position

    // create tempo object
    aubio_tempo_t *o = m_aubioTempoDetector;


    // put some fresh data in input vector
    for (int i = 0; i < hop_size; i++) {
        float samp = pSoundData->GetWaveformL(i) + pSoundData->GetWaveformR(i);
        fvec_set_sample(in, 8*samp, i);
    }
    // execute tempo
    aubio_tempo_do(o,in,out);

    if (fastBeats) {
        for (int i = 0; i < hop_size; i++) {
            float samp = pSoundData->GetWaveformL(i+hop_size) + pSoundData->GetWaveformR(i+hop_size);
            fvec_set_sample(in, 8*samp, i);
        }
        aubio_tempo_do(o,in,out2);
        out->data[0] += out2->data[0]; // combine
    }

    //qDebug() << aubio_tempo_get_confidence(o) << "c - b" << aubio_tempo_get_bpm(o);

    // do something with the beats
    if (out->data[0] != 0 && (allowPartialBeats || m_output <= 0)) {
        /* char s[1024];
            sprintf(s, "beat at %.3fms, %.3fs, frame %d, %.2fbpm with confidence %.2f\n",
                    aubio_tempo_get_last_ms(o), aubio_tempo_get_last_s(o),
                    aubio_tempo_get_last(o), aubio_tempo_get_bpm(o), aubio_tempo_get_confidence(o));
            qDebug() << s;*/

        // remember confid and bpm
        m_confidence = aubio_tempo_get_confidence(o);
        m_bpm = aubio_tempo_get_bpm(o);

        // set beat
        if (m_confidence >= newBeatConfidenceCutoff) {
            // activate output
            m_output = 1;
            // beat fade out in bets per second
            fade = m_bpm/60;
            if (fade < 0) fade = 0;
            //qDebug() << fade;
        }

    }


    // clean up memory
    del_fvec(in);
    del_fvec(out);
    del_fvec(out2);
    
    return m_output;
}

float ldTempoTracker::output() const
{
    return m_output;
}

float ldTempoTracker::confidence() const
{
    return m_confidence;
}

float ldTempoTracker::bpm() const
{
    return m_bpm;
}
