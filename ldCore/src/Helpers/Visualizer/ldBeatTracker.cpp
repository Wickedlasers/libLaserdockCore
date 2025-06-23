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

#include "ldCore/Helpers/Visualizer/ldVisualizerHelper.h"

#include <cmath>

#include <QtCore/QtDebug>

/*!
  \class BeatTracker
  \brief Calculates tempo/phase data with a beat detection algorithm.

  The algorithm consists of a 2d bank of comb filters.  The signal
  is applied and the filter that produces the highest energy is selected.

  Update:
  \l BeatTracker::add(float f)
  Call this once for every input frame.

  Outputs:
  bestphase: This is the most useful output, which contains the current
  phase of the beat.  A value of 0 is the start of a beat, 0.5 is halfway
  between beats. 1 means a beat is almost about to happen.
  bestbpm: The tempo of the music. The units are beats per frame (not
  actually bpm)

*/

#include "ldCore/Helpers/Visualizer/ldBeatTracker.h"

//#include <cmath>
//#include <cmath>

#include <ldCore/Sound/ldSoundData.h>

namespace {
    static constexpr uint HISTORY_COUNT = 120;

    static constexpr uint beat_overdrive = AUDIO_OVERDRIVE_FACTOR;
    static constexpr uint maxbeathistory = 4*HISTORY_COUNT*beat_overdrive;
    static constexpr uint maxbeatfilters = HISTORY_COUNT*beat_overdrive;

    static constexpr uint STRIDE_SKIP = overdrive;
    //#define STRIDE_SKIP 1
}

ldBeatTracker::ldBeatTracker() {
    m_history.resize(maxbeathistory);
    m_apriori.resize(maxbeatfilters);
}

void ldBeatTracker::add(float f) {

    uint historysize = std::min(std::max(4u, m_filterend*5), maxbeathistory);
    m_history.push_front(f);
    m_history.resize(historysize);

    m_filterstart = MAX(3, m_filterstart);

    float bestEnergy = 0;
    uint bestStride = 0;
    uint bestPhase = 0;

    for (uint stride = m_filterstart; stride < m_filterend; stride+=STRIDE_SKIP) {
        for (uint phase = 0; phase < stride; phase++) {
            float sum = 3*m_history[phase];
            sum += 2*m_history[phase+stride];
            sum += 1*m_history[phase+2*stride];

            float average = sum / 6.0f;
            float energy = average * m_apriori[stride];
            
            if (energy > bestEnergy) {
                bestEnergy = energy;
                bestStride = stride;
                bestPhase = phase;
            }
        }
    }

    m_bestbpm = (bestStride != 0) ? 1.0f/bestStride : 0;
    m_bestphase = bestPhase*m_bestbpm;
}

void ldBeatTracker::set(uint filterstart, uint filterend) {
    Q_ASSERT(filterstart <= filterend);

    m_filterstart = filterstart;
    m_filterend = filterend;
    if (m_filterstart > maxbeatfilters) m_filterstart = maxbeatfilters;
    if (m_filterend > maxbeatfilters) m_filterend = maxbeatfilters;
    for (uint i = 0; i < maxbeatfilters; i++) {
        float f = 0;
        uint nfilters = (m_filterend-m_filterstart);
        if (nfilters >= 1) f = ((i-m_filterstart)+0.5f)/nfilters;
        f = 1-(f-0.5f)*(f-0.5f)*4;
        f *= 2;
        if (f<0) f = 0;
        if (f>1) f = 1;
        m_apriori[i] = f;
    }
}

float ldBeatTracker::bestbpm() const
{
    return m_bestbpm;
}

float ldBeatTracker::bestphase() const
{
    return m_bestphase;
}

