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
#include "ldCore/Helpers/Maths/ldSmoothUtil.h"


class LDCORESHARED_EXPORT ldHybridFlash {
public:
    ldHybridFlash();

    void process(ldMusicManager* m);
    uint32_t getColor(int index);
    uint32_t getColor(int index, int max, int limit);

    int recent[16];
    float powers[16];
    float hues[16];

private:
    ldSelector selectorBeatAlgo[16];
    ldSelector selectorColorHue[16];

    bool ison[16];
    uint32_t colors[16];
};



class LDCORESHARED_EXPORT ldHybridAnima {
public:
    ldHybridAnima();
    void process(ldMusicManager* m);

    int outputBaseFrameIndex = 0;
    float outputTrackPosition = 0;

private:
    bool toggle = false;
    int lockout = 0;
    ldSelector selectorBeatAlgo;
    ldDurationalStatEstimator smooth1, smooth2;
};


class LDCORESHARED_EXPORT ldHybridAutoColor2 {
public:
    
    ldHybridAutoColor2();
    void process(ldMusicManager* m);
    
    ldSelector selectorColorHue1;
    ldSelector selectorColorHue2;

private:
    uint32_t outputColor1 = 0, outputColor2 = 0;
    ldSelector selectorColorSat2;
};

class LDCORESHARED_EXPORT ldHybridColorPalette {
public:
    ldHybridColorPalette();
    void process (ldMusicManager* m);
    void colorize(float& rr, float& gg, float& bb);

private:
    float p1[4][3][3];
    
    int base = 0;
    float progress = 0;
};

#endif /*_LDHYBRIDREACTOR_H*/
