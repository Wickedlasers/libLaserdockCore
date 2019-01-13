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
//  ldAppakSpectrum.h
//
//  Created by Eric Brugère on 2/10/15.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//
#ifndef ldAppakSpectrum_H
#define ldAppakSpectrum_H

#include <ldCore/Sound/ldSoundData.h>
//
class LDCORESHARED_EXPORT ldAppakSpectrum
{
public:
    struct LDCORESHARED_EXPORT Spectrum {
        float binHzOne = 0.f; // (x+1)*30 to have Hz
        float binHzTwo = 0.f;
        float binHzThree = 0.f;
    };

    ldAppakSpectrum();
    ~ldAppakSpectrum();

    void process(ldSoundData* pSoundData);
    int getMaxBinForHzInterval(float startHz, float endHz) const;

    std::vector<float> frequencies;
    Spectrum resSpectrum;

private:
    void doSprectrumStats();

    std::vector<float> m_binCounter;
    int m_frameDropper = 0;

};

#endif // ldAppakSpectrum_H
