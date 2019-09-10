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
//  ldVisualizerHelper.h
//  ldCore
//
//  Created by feldspar on 8/4/14.
//  Copyright (c) 2014 Wicked Lasers. All rights reserved.
//

#ifndef __ldCore__ldVisualizerHelper__
#define __ldCore__ldVisualizerHelper__

#include <memory>

#include <ldCore/Sound/ldSoundData.h>
#include "ldCore/Helpers/Maths/ldGeometryUtil.h"


// --------------------------------- BeatSignal ----------------------------

class ldBeatTracker;

static const int overdrive = AUDIO_OVERDRIVE_FACTOR;

class LDCORESHARED_EXPORT ldBeatSignal {
public:
    float value;// = 0;
    float mult;// = 1.55;
    float floor;// = 0.1;
    float fade;// = 0.01;
    float flip;// = 1;
    bool flipping;// = false;
    ldBeatSignal() {
        value = 0;
        mult = 1.55f;
        floor = 0.1f;
        fade = 0.01f;
        flip = 1;
        flipping = false;
    }
    float process(float input, float delta);
};


// --------------------------------- StatValue ----------------------------


#define STRIDE_SKIP overdrive
//#define STRIDE_SKIP 1
static const int maxstathistory = 2048*overdrive;
class LDCORESHARED_EXPORT ldStatValue {
public:
    float baseValue = 0.0f;
    float averageValue = 0.0f;
    float scaledValue = 0.0f;
    float probabilityValue = 0.0f;
    float combinedValue = 0.0f;
    float history[maxstathistory];
    int historysize = 0;
    int nfull = 0;
    ldStatValue() {
                 for (int i = 0; i < maxstathistory; i++) history[i] = 0;
                 }
    float add(float f);
    void set(int historysize);
};


// --------------------------------- PresetSignal ----------------------------


class LDCORESHARED_EXPORT ldPresetSignal {
public:
    ldStatValue stat;
    float statOutput;
    ldBeatSignal signal;
    float signalOutput;
    std::unique_ptr<ldBeatTracker> tracker;
    float trackerOutput;
    ldBeatSignal trackerSignal;
    float trackerSignalOutput;

    bool spinning;
    float spinspeed;
    float spindir;
    float spinTurns;
    float spinTurns4;
    float spinOutput;

    float walkCounter;
    float walkWobbleFactor;
    float owc;

    float walkerOutput;
    bool didClick;

    float output;

    float statTime, statType, statParam;
    float signalMult, signalFloor, signalFade;
    float trackerTime, trackerRangeStart, trackerRange;
    float trackerSignalMult, trackerSignalFloor, trackerSignalFade;
    float trackerSpinSpeedMult;
    float trackerSpinThreshold;
    void init();
    void set(float f[9]) {
        statTime = f[0];
        statType = f[1];
        statParam = f[2];
        signalMult = f[3];
        signalFloor = f[4];
        signalFade = f[5];
        trackerTime = f[6];
        trackerRangeStart = f[7];
        trackerRange = f[8];
        init();
    }
    void set(float f0,
             float f1,
             float f2,
             float f3,
             float f4,
             float f5,
             float f6,
             float f7,
             float f8) {
        statTime = f0;
        statType = f1;
        statParam = f2;
        signalMult = f3;
        signalFloor = f4;
        signalFade = f5;
        trackerTime = f6;
        trackerRangeStart = f7;
        trackerRange = f8;
        init();
    }


    float processSignal(float f);
    ldPresetSignal (/*float statTime = 0.5, float statType = 0.5, float statScale = 0.5,
                  float signalMult = 0.5, float signalFloor = 0.5, float signalFade = 0.5,
                  float trackerTime = 0.5, float trackerRangeStart = 0.5, float trackerRange = 0.5,
                  float trackerSignalMult = 0.5, float trackerSignalFloor = 0.5, float trackerSignalFade = 0.5
            */);

    ~ldPresetSignal();


};


// --------------------------------- MusicReactor ----------------------------


#define MRPARAMS 20

class LDCORESHARED_EXPORT ldMusicReactor
{

public:
    static QString labels[MRPARAMS];

    ldMusicReactor();
    ~ldMusicReactor();

    void setParams(float _params[MRPARAMS]);
    float process(ldSoundData* p);

    float output;

    bool isSilent2 = true;
    float isSilent2float = 0.f;

    float statOutput;

    float signalOutput;

    std::unique_ptr<ldBeatTracker> tracker;

    float spindir;
    float spinTurns;
    float spinOutput;
    float spinOutput4;

    float walkerOutput;
    float walkerClickOutput;

    float spinSpeedMult, spinThreshold, spinAngleThreshold;

private:
    void processStat(ldSoundData* p, float d);
    void processSignal(float delta);
    void processTracker(float d);
    void processSpin();
    void processWalk();

    void updateParams();

    float params[MRPARAMS];

    float lastInput;
    ldStatValue stat;
    ldStatValue statLog;
    ldStatValue statLinear;

    ldBeatSignal signal;

    float trackerOutput;

    bool spinning;
    float spinspeed;

    float spinTurns4;

    float walkCounter;
    float owc;

    float outputType;
    float frequencyMid, frequencyRange, slopeFactor, logFactor;
    float statTime, statType, statParam;
    float signalMult, signalFloor, signalFade, signalPostFloor;
    float trackerTime, trackerRangeStart, trackerRange;
    float walkerWobbleFactor, walkerAdvance;
};



#endif /* defined(__ldCore__ldVisualizerHelper__) */
