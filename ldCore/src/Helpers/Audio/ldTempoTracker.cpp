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
#include "ldCore/Helpers/Maths/ldMaths.h"

ldTempoTracker::ldTempoTracker(bool fastBeats, bool allowPartialBeats, QObject *parent)
    : QObject(parent)
    , m_allowPartialBeats(allowPartialBeats)
    , m_hop_size(fastBeats ? SAMPLE_SIZE / 3 : SAMPLE_SIZE)
    , m_aubio(new_aubio_tempo("default", SAMPLE_SIZE, m_hop_size, SAMPLE_RATE), del_aubio_tempo)
    , m_in(new_fvec (m_hop_size), del_fvec)
    , m_out(new_fvec (2), del_fvec)
{
}

ldTempoTracker::~ldTempoTracker()
{
}


void ldTempoTracker::process(ldSoundData* pSoundData, float delta)
{
    //static int s = 99999; s++;
    //qDebug() << "called " << s << " with delta " << delta << "and hop size" << hop_size;

    // fade
    m_output -= m_fade*delta;
    if (m_output < 0) m_output = 0;

    // skip for overdrive factor, we are fixed 30fps
    m_overdriveSkip++;
    m_overdriveSkip %= AUDIO_OVERDRIVE_FACTOR;
    if (m_overdriveSkip)
        return;

    // put some fresh data in input vector
    smpl_t isBeat = 0.f;
    int soundDataIndex = 0;
    while(soundDataIndex < SAMPLE_SIZE) {
        for (uint_t i = 0; i < m_hop_size; i++) {
            float samp = (pSoundData->GetWaveformBufferL()[soundDataIndex] + pSoundData->GetWaveformBufferR()[soundDataIndex]) / 2.f;
            fvec_set_sample(m_in.get(), samp, i);
            soundDataIndex++;
        }
        // execute tempo
        aubio_tempo_do(m_aubio.get(), m_in.get(), m_out.get());

        isBeat += m_out->data[0];
    }


    float confidence = aubio_tempo_get_confidence(m_aubio.get());
    float bpm = aubio_tempo_get_bpm(m_aubio.get());
    if(!cmpf(bpm, 0)
            && !cmpf(confidence, 0.f)
        && !std::isnan(confidence)) {
        m_bpm = bpm;
        m_confidence = confidence;
//        if(m_hop_size < SAMPLE_SIZE) qDebug() << m_bpm << m_confidence;
    }


    float beatMs = aubio_tempo_get_last_ms(m_aubio.get());
    if(!cmpf(beatMs, m_lastBeatMs)) {
        emit beatDetected();
        m_lastBeatMs = beatMs;
    }

    // do something with the beats
    if (isBeat && (m_allowPartialBeats || m_output <= 0)) {
        /* char s[1024];
            sprintf(s, "beat at %.3fms, %.3fs, frame %d, %.2fbpm with confidence %.2f\n",
                    aubio_tempo_get_last_ms(o), aubio_tempo_get_last_s(o),
                    aubio_tempo_get_last(o), aubio_tempo_get_bpm(o), aubio_tempo_get_confidence(o));
            qDebug() << s;*/

        // remember confid and bpm
        // set beat
        if (m_confidence >= 0.f) {
            // activate output
            m_output = 1;
            // beat fade out in bets per second
            m_fade = m_bpm/60;
            if (m_fade < 0) m_fade = 0;
        }

    }
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
