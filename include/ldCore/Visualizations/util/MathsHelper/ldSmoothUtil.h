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
//  ldSmoothUtil.h
//  ldCore
//
//  Created by feldspar on 8/3/15.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//

#ifndef __ldCore__ldSmoothUtil__
#define __ldCore__ldSmoothUtil__

#include "ldCore/ldCore_global.h"

inline float tweenf(float a, float b, float f) {return (1-f)*a+f*b;}

class LDCORESHARED_EXPORT ldDiscr {
public:
    ldDiscr();

    void update(float beatValue, float _bpf, float smoothCutoff, float instantCutoff, float toggleCutoff, bool targetValue);
    float smooth(float a, float b);

    bool outputInstant = false;
    float outputSmooth = 0;

private:
    float bpf = 120;
    float dir = -1;
    float lockout = 0;
};


class LDCORESHARED_EXPORT ldSelector {
public:
    ldSelector();

    void process(float targetValue, int indexCount, float beatValue, float smoothCutoff, float instantCutoff, float toggleCutoff, float _bpf);
    void process2D(float targetValue1, float targetValue2, int indexCount, float beatValue, float smoothCutoff, float instantCutoff, float toggleCutoff, float _bpf);

    int indexNew = 0;
    int indexOld = 0;
    float indexProgress = 0;
    float indexProgressCompliment = 0;
    float indexFloat = 0;

private:
    float bpf = 0;
    float lockout = 0;
    int transitionType = 0;
};

class LDCORESHARED_EXPORT ldTimeSlower {
public:
    ldTimeSlower(int _len = 2, float _speed = 0.25);
    float process(float f);
    void changeLen(int newlen);

    float output = 0;
    bool beatLong = 0;

private:
    bool beat = 0;
    int ipos = 0;
    float fpos = 0;
    int len = 0;
    float speed = 0;
};

class LDCORESHARED_EXPORT ldAdvancer {
public:
    ldAdvancer();
    float process(float walk, float dfreq, float beatness, float ismusic) ;

    float value = 0;

private:
    float lv = 0;
    float d1 = 0, d2 = 0;
};



#endif /* defined(__ldCore__ldSmoothUtil__) */
