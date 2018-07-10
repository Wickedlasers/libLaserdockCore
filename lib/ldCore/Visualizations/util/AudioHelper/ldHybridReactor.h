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
//  ldHybridReactor.cpp
//
//  Created by feld on 3/31/16.
//  Copyright (c) 2016 Wicked Lasers. All rights reserved.
//
#ifndef _LDHYBRIDREACTOR_H
#define _LDHYBRIDREACTOR_H

#include "ldCore/Visualizations/MusicManager/ldMusicManager.h"
#include "ldCore/Visualizations/util/MathsHelper/ldSmoothUtil.h"


class LDCORESHARED_EXPORT HybridFlash { public:

    HybridFlash();
    void process(ldMusicManager* m);

    Selector selectorBeatAlgo[16];
    Selector selectorColorHue[16];
    int recent[16];
    bool ison[16];
    float powers[16];
    float hues[16];
    uint32_t colors[16];

    uint32_t getColor(int index);
    uint32_t getColor(int index, int max, int limit);
};



class LDCORESHARED_EXPORT HybridAnima { public:
    
    HybridAnima();
    void process(ldMusicManager* m);
    static const int nBaseFrames = 6;
    int outputBaseFrameIndex;
    float outputTrackPosition;
    
    bool toggle;
    int lockout;
    Selector selectorBeatAlgo;
    DurationalStatEstimator smooth1, smooth2;
    
};


class LDCORESHARED_EXPORT HybridAutoColor2 { public:
    
    HybridAutoColor2();
    void process(ldMusicManager* m);
    uint32_t outputColor1, outputColor2;
    
    Selector selectorColorHue1;
    Selector selectorColorHue2;
    Selector selectorColorSat2;
    
};

class LDCORESHARED_EXPORT HybridColorPalette { public:
    
    HybridColorPalette();
    void process (ldMusicManager* m);
    void colorize(float& rr, float& gg, float& bb);
    
    float p1[4][3][3];
    
    int base;
    float progress;
    float speed;
    
};

#endif /*_LDHYBRIDREACTOR_H*/
