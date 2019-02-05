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

// ldMathSpectrum.h
// Created by Eric Brugère on 9/24/16.
// Copyright (c) 2016 Wicked Lasers. All rights reserved.

#ifndef LDMATHSPECTRUM_H
#define LDMATHSPECTRUM_H

#include <ldCore/Sound/ldSoundData.h>

class LDCORESHARED_EXPORT ldMathSpectrum
{
public:
    // static float rndFloatBetween(float min, float max);
//    ldMathSpectrum();
//    ~ldMathSpectrum();

//    void process(ldSoundData* pSoundData);
    static std::vector<float> getAverageSpectrumValueFromInterval(ldSoundData* pSoundData, std::vector<int> interval, int nbBandSoundData);
    static std::vector<float> getNormalizedSpectrumValueFromInterval(ldSoundData* pSoundData, std::vector<int> interval, int nbBandSoundData);

};



#endif // LDMATHSPECTRUM_H


