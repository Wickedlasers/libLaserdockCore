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

class LDCORESHARED_EXPORT Discr {
public:
    bool outputInstant;
    float outputSmooth;
    float bpf;
    float dir;
    float lockout;
    Discr();
    void update(float beatValue, float _bpf, float smoothCutoff, float instantCutoff, float toggleCutoff, bool targetValue);
    float smooth(float a, float b);
};


class LDCORESHARED_EXPORT Selector {
public:
    int indexNew;
    int indexOld;
    float indexProgress;
    float indexProgressCompliment;
    float indexFloat;
    float bpf;
    float lockout;
    int transitionType;
    Selector();
    void process(float targetValue, int indexCount, float beatValue, float smoothCutoff, float instantCutoff, float toggleCutoff, float _bpf);
    void process2D(float targetValue1, float targetValue2, int indexCount, float beatValue, float smoothCutoff, float instantCutoff, float toggleCutoff, float _bpf);
};

class LDCORESHARED_EXPORT TimeSlower {
public:
    float output;
    bool beat;
    bool beatLong;
    int ipos;
    float fpos;
    int len;
    float speed;
    TimeSlower(int _len = 2, float _speed = 0.25);
    float process(float f);
    void changeLen(int newlen);
};

class LDCORESHARED_EXPORT Advancer {
public:
    float value;
    float lv;
    float d1, d2;
    Advancer();
    float process(float walk, float dfreq, float beatness, float ismusic) ;
};



#endif /* defined(__ldCore__ldSmoothUtil__) */
