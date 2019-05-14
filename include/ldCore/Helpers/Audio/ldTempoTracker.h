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

#ifndef LDTEMPOTRACKER_H
#define LDTEMPOTRACKER_H

#include <memory>

#include <aubio/src/aubio.h>

#include <QtCore/QElapsedTimer>

#include <ldCore/Sound/ldSoundData.h>

class LDCORESHARED_EXPORT ldTempoTracker : public QObject
{
    Q_OBJECT
public:
    explicit ldTempoTracker(bool fastBeats = true, bool allowPartialBeats = true, QObject *parent = nullptr);
    virtual ~ldTempoTracker();

    void process(ldSoundData* pSoundData, float delta);

    float output() const;
    float confidence() const;
    float bpm() const;

signals:
    void beatDetected();

private:
    // notes on algorithms:
    //"specflux" is defailt, looks good and is stable
    //"rolloff" looks good and is more expressive but less consistent
    //"specdiff" and "phase" seem to work decently but no better than default

    // register a new beat even if the old beat is still fading out
    bool m_allowPartialBeats = true;

    // 0-1 value to prefer slow-fast tempos.  not yet implemented.
    //float speed;

    uint_t m_hop_size = SAMPLE_SIZE;
    int m_overdriveSkip = -1;
    std::unique_ptr<aubio_tempo_t, void(*)(aubio_tempo_t*)> m_aubio;
    float m_fade = 0.f;

    float m_output = 0;
    float m_confidence = 0;
    float m_bpm = 120;

    // create some vectors
    std::unique_ptr<fvec_t, void(*)(fvec_t*)> m_in;
    std::unique_ptr<fvec_t, void(*)(fvec_t*)> m_out;

    float m_lastBeatMs = 0;
};



#endif // LDTEMPOTRACKER_H
