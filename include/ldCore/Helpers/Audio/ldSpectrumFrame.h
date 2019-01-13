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

#ifndef LDSPECTRUMFRAME_H
#define LDSPECTRUMFRAME_H

#include <vector>

#include "ldCore/ldCore_global.h"

class ldSoundData;

// single frame of spectrogram. shortened log spectrum
class LDCORESHARED_EXPORT ldSpectrumFrame {
public:
    ldSpectrumFrame();
    void update(ldSoundData* psd);
    float getEnergy();
    float getEnergyScaled(float f) const;
    static float compare(const ldSpectrumFrame *p1, const ldSpectrumFrame *p2);

    static const int slsLen = 8;
    std::vector<float> sls;
};


#endif // LDSPECTRUMFRAME_H
