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
//  ldSmoothUtil.cpp
//  ldCore
//
//  Created by feldspar on 8/3/15.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//

#include "ldCore/Helpers/Maths/ldSmoothUtil.h"

#include <QtCore/QDebug>

#include <ldCore/Sound/ldSoundInterface.h>
#include "ldCore/Helpers/Maths/ldGeometryUtil.h"

ldSelector::ldSelector() {
}

void ldSelector::process(float targetValue, int indexCount, float beatValue, float smoothCutoff, float instantCutoff, float toggleCutoff, float _bpf) {
    clampfp(targetValue, 0, 1);
    // advance progress counter
    clampfp(bpf, 0.005f, 0.5f);
    indexProgress += bpf;
    clampfp(indexProgress, 0, 1);
    // check if finished transition
    if (indexProgress >= 1) {
        indexOld = indexNew;
        indexProgress = 0;
        bpf = 0;
        transitionType = 0;
    }
    // update lockout
    lockout -= bpf;
    if (lockout < 0) lockout = 0;

    // check for beats
    int targetIndex = targetValue*indexCount;
    if (targetIndex > indexCount-1) targetIndex = indexCount-1;
    if (beatValue >= toggleCutoff && transitionType < 3) {
        // forced toggle style transition
        indexNew = targetIndex;
        indexOld = indexCount-1-targetIndex;
        indexProgress = 0;
        bpf = _bpf*2;
        transitionType = 3;
        //qDebug() << "toggle to " << outputInstant;
    } else if (beatValue >= instantCutoff && transitionType < 2 && !lockout) {
        if (indexOld != targetIndex) {
            indexOld = indexNew = targetIndex;
            indexProgress = 0;
            lockout = 1.0;
            bpf = _bpf;
            transitionType = 2;
        }
    } else if (beatValue >= smoothCutoff && transitionType < 1) {
        indexNew = targetIndex;
        indexProgress = 0;
        bpf = _bpf;
        transitionType = 1;
        // qDebug() << "smooth to " << outputInstant << bpf;
    }

    // some more values to calculate
    indexProgressCompliment = 1.0-indexProgress;
    indexFloat = indexOld*indexProgressCompliment + indexNew*indexProgress;
}

void ldSelector::process2D(float targetValue1, float targetValue2, int indexCount, float beatValue, float smoothCutoff, float instantCutoff, float toggleCutoff, float _bpf) {
    // advance progress counter
    indexProgress += bpf;
    // check if finished transition
    if (indexProgress >= 1) {
        indexOld = indexNew;
        indexProgress = 0;
        bpf = 0;
        transitionType = 0;
    }
    // update lockout
    lockout -= _bpf;
    if (lockout < 0) lockout = 0;

    // check for beats
    int targetIndex1 = targetValue1*indexCount;
    if (targetIndex1 > indexCount-1) targetIndex1 = indexCount-1;
    int targetIndex2 = targetValue2*indexCount;
    if (targetIndex2 > indexCount-1) targetIndex2 = indexCount-1;
    int targetIndex = targetIndex1 * indexCount + targetIndex2;

    if (beatValue >= toggleCutoff && transitionType < 3) {
        // forced toggle style transition
        indexNew = targetIndex;
        indexOld = (indexCount*indexCount)-1-targetIndex;
        indexProgress = 0;
        bpf = _bpf*1;
        transitionType = 3;
        //qDebug() << "toggle to " << indexNew;
        //qDebug() << "2d coords are" << targetIndex1 << targetIndex2;
    } else if (beatValue >= instantCutoff && transitionType < 2 && !lockout) {
        if (indexOld == targetIndex) {
            if (!(indexCount%2)) {
                targetIndex = (targetIndex/2)*2 + ((targetIndex%2)?0:1);
            } else {
                targetIndex = (indexCount*indexCount)-1-targetIndex;
            }
        }
        //qDebug() << targetIndex;
        //indexOld =
                indexNew = targetIndex;
        indexProgress = 0;
        //lockout = 1.0;
        bpf = _bpf/2;
        transitionType = 2;
    } else if (beatValue >= smoothCutoff && transitionType < 1) {
        indexNew = targetIndex;
        indexProgress = 0;
        bpf = _bpf/4;
        transitionType = 1;
        // qDebug() << "smooth to " << outputInstant << bpf;
    }

    // some more values to calculate
    indexProgressCompliment = 1.0-indexProgress;
    indexFloat = 0;//doesnt work in 2d mode
}

ldDiscr::ldDiscr() {
}

void ldDiscr::update(float beatValue, float _bpf, float smoothCutoff, float instantCutoff, float toggleCutoff, bool targetValue) {
    float &os = outputSmooth;
    os += bpf*dir;
    if (os >= 1) {
        outputInstant = true;
        os = 1;
        dir = 0;
    }
    if (os <= 0) {
        outputInstant = false;
        os = 0;
        dir = 0;
    }
    lockout -= _bpf;
    if (lockout < 0) lockout = 0;
    if (beatValue >= toggleCutoff && !lockout) {
        outputInstant = !outputInstant;
        outputSmooth = outputInstant?1:0;
        dir = 0;
        lockout = 0.5;
        //qDebug() << "toggle to " << outputInstant;
    } else if (beatValue >= instantCutoff && dir == 0 && !lockout) {
        // FIXME lockout unused assignment
//        if (outputInstant != targetValue) lockout = 2;
//        if (outputSmooth != (targetValue?1:0)) lockout = 2;
        outputInstant = targetValue;
        outputSmooth = targetValue?1:0;
        dir = 0;
        lockout = 1.0;
        //qDebug() << "instant to " << outputInstant;
    } else if (beatValue >= smoothCutoff) {
        //outputInstant = targetValue;
        bpf = _bpf;
        dir = targetValue?1:-1;
        // qDebug() << "smooth to " << outputInstant << bpf;
    }
}

float ldDiscr::smooth(float a, float b) {
    return outputSmooth*b + (1-outputSmooth)*a;
}

//////////////////////

ldTimeSlower::ldTimeSlower(int _len, float _speed)
    : len(_len)
    , speed(_speed)
{
}

float ldTimeSlower::process(float f) {
    beat = (fpos > (1-speed/2) && f < (speed/2));
    if (beat) ipos = (ipos + 1) % len;
    beatLong = beat && !ipos;
    fpos = f;
    output = (ipos + fpos) / len;
    return output;
}
void ldTimeSlower::changeLen(int newlen) {
    if (newlen == len) return;
    len = newlen;
    float f = output * len;
    ipos = (int) f;
    fpos = f - ipos;
    output = (ipos + fpos) / len;
}



ldAdvancer::ldAdvancer() {
}

float ldAdvancer::process(float walk, float dfreq, float beatness, float ismusic) {
    d1 = walk - lv;
    d2 = dfreq;

    if (d1 < -0.5) d1++;
    if (d1 < 0) d1 = 0;
    if (d1 > 0.5) d1 = 0;
    if (d1 > d2*2) d1 = d2*2;

    float d = tweenf(d2, d1, beatness);
    d = tweenf(0, d, ismusic);

    if (d > d2*2) d = d2*2;
    if (d < 0) d = 0;

    //static int cc = 0; cc++; if (!(cc%100)) qDebug() << d1 << d2 << d;

    value += d;
    if (value > 1) value--;

    lv = value;
    return value;
}

