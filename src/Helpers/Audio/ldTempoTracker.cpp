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

ldTempoTracker::ldTempoTracker(const QString &algorithm, bool _fastBeats, bool _allowPartialBeats, float _newBeatConfidenceCutoff)
    : m_algorithm(algorithm)
    , m_fastBeats(_fastBeats)
    , m_allowPartialBeats(_allowPartialBeats)
    , m_newBeatConfidenceCutoff(_newBeatConfidenceCutoff)
    , m_hop_size(m_fastBeats ? SAMPLE_SIZE/2 : SAMPLE_SIZE)
    , m_aubio(new_aubio_tempo(m_algorithm.toLatin1().constData(), SAMPLE_SIZE, m_hop_size, SAMPLE_RATE), del_aubio_tempo)
{
}

ldTempoTracker::~ldTempoTracker()
{
}


void ldTempoTracker::process(ldSoundData* pSoundData) {

    // update time
    float delta = AUDIO_UPDATE_DELTA_S;

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

    // create some vectors
    std::unique_ptr<fvec_t, void(*)(fvec_t*)> in(new_fvec (m_hop_size), del_fvec);
    std::unique_ptr<fvec_t, void(*)(fvec_t*)> out(new_fvec (2), del_fvec);
    std::unique_ptr<fvec_t, void(*)(fvec_t*)> out2(new_fvec (2), del_fvec);

    // put some fresh data in input vector
    for (uint_t i = 0; i < m_hop_size; i++) {
        float samp = pSoundData->GetWaveformL(i) + pSoundData->GetWaveformR(i);
        fvec_set_sample(in.get(), 8*samp, i);
    }
    // execute tempo
    aubio_tempo_do(m_aubio.get(), in.get(), out.get());

    if (m_fastBeats) {
        for (uint_t i = 0; i < m_hop_size; i++) {
            float samp = pSoundData->GetWaveformL(i+m_hop_size) + pSoundData->GetWaveformR(i+m_hop_size);
            fvec_set_sample(in.get(), 8*samp, i);
        }
        aubio_tempo_do(m_aubio.get(), in.get(), out2.get());
        out->data[0] += out2->data[0]; // combine
    }

    //qDebug() << aubio_tempo_get_confidence(o) << "c - b" << aubio_tempo_get_bpm(o);

    // do something with the beats
    if (cmpf(out->data[0], 0.f) && (m_allowPartialBeats || m_output <= 0)) {
        /* char s[1024];
            sprintf(s, "beat at %.3fms, %.3fs, frame %d, %.2fbpm with confidence %.2f\n",
                    aubio_tempo_get_last_ms(o), aubio_tempo_get_last_s(o),
                    aubio_tempo_get_last(o), aubio_tempo_get_bpm(o), aubio_tempo_get_confidence(o));
            qDebug() << s;*/

        // remember confid and bpm
        m_confidence = aubio_tempo_get_confidence(m_aubio.get());
        m_bpm = aubio_tempo_get_bpm(m_aubio.get());

        // set beat
        if (m_confidence >= m_newBeatConfidenceCutoff) {
            // activate output
            m_output = 1;
            // beat fade out in bets per second
            m_fade = m_bpm/60;
            if (m_fade < 0) m_fade = 0;
            //qDebug() << fade;
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
