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

#ifndef LDSPECTROGRAM_H
#define LDSPECTROGRAM_H

#include <ldCore/Sound/ldSoundData.h>

#include "ldSpectrumFrame.h"

// Spectrogram class.  ring buffer of spectrum frames with fixed size
class LDCORESHARED_EXPORT ldSpectrogram
{
public:
    static const int histLenSeconds = 12;
    static const int histLen = histLenSeconds*30*AUDIO_OVERDRIVE_FACTOR; // 720

    ldSpectrogram();

    void addFrame(ldSpectrumFrame &f);
    ldSpectrumFrame getNoisedFrame(float histSec, float rootScale, float rootPower, bool alt1, bool alt2);

    void calculateS();
    float getS(int t) const;

    const ldSpectrumFrame &currentFrame() const;
    const ldSpectrumFrame &frame(int index) const;

private:
    int head = -1;
    ldSpectrumFrame hist[histLen];

    std::vector<float> m_cachedS;
};

#endif // LDSPECTROGRAM_H
