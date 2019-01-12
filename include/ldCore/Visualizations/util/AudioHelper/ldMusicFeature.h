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

#ifndef _LDMUSICFEATURE_H
#define _LDMUSICFEATURE_H

#include "ldDurationalStatEstimator.h"

class ldSoundData;
class ldSpectrumFrame;

class LDCORESHARED_EXPORT MusicFeature1 {
public:
    MusicFeature1();
    void update(const ldSpectrumFrame& s, ldSoundData* psd);

    float statGenreClassical;
    float statGenreWorld;
    float statGenreRock;
    float statGenrePop;

    float statMoodFunky;
    float statMoodMelodic;
    float statMoodFast;

    float statGenreClassical2;
    float statGenreWorld2;
    float statGenreRock2;
    float statGenrePop2;

    float statMoodFunky2;
    float statMoodMelodic2;

    // stat for each input value
    static const int nStat = 7;
    float stats[nStat];
    ldDurationalStatEstimator statEstimators[nStat];

    // features a subset of possible stats
    static const int nFeat = 8;
    float features[nFeat];

    // smoothers for output
    ldDurationalStatEstimator os[4];

};

#endif /*_LDMUSICFEATURE_H*/
