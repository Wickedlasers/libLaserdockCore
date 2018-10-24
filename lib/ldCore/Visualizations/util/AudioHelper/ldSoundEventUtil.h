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
//  ldSoundEventUtil.h
//
//  Created by Eric Brugère on 2/10/15.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//

#ifndef LDSOUNDEVENTUTIL_H
#define LDSOUNDEVENTUTIL_H

#include "ldTempoTracker.h"
#include "ldCore/Visualizations/util/VisualizerHelper/ldVisualizerHelper.h"

class ldMusicManager;

enum SoundEventUtilEnum {
    SOUND_EVENT_UTIL_MUSIC_REACTOR = 0,
    SOUND_EVENT_UTIL_TEMPO_TRACKER = 1,
    SOUND_EVENT_UTIL_APPAKABAR_BEAT = 2,
    SOUND_EVENT_UTIL_APPAKABAR_PEAK = 3,
    SOUND_EVENT_UTIL_ENUM_SIZE = 4
};

struct LDCORESHARED_EXPORT SoundEventUtilStruct {
    int drop;
    int dropCount;
    float ceil;
    bool isEvent;
    bool isAnimationBusy;
};

typedef std::vector< std::vector<SoundEventUtilStruct> > soundutilevent_vec_t;

class LDCORESHARED_EXPORT ldSoundEventUtil
{
public:
    bool debug,isMusicReactor,isTempoTracker,isAppakaBeat;
    bool isSilent;
    bool didDjStop;
    bool didDjStart;
//    MusicReactor *musicReactor;
//    TempoTracker *tempoTracker;
//    ldAppakaBeat *appakaBeat;

    //
    ldSoundEventUtil();
    ~ldSoundEventUtil();
    void addMusicReactor();
    void addTempoTracker();
    void addAppakabarBeat();
    void initWithMaxChannel(int max);

    void update(ldSoundData* pSoundData, float delta);
    void updateSystem(int system, int channel);

    void setSystemDropAndSensibilityForChannel(int system, int channel, int drop, float ceil);
    void setSystemEventBusy(int system, int channel, bool busy);
    bool isSystemEvent(int system, int channel);
    bool isSystemEventBusy(int system, int channel);

private:
    int maxChannelSize;
    bool previousIsSilent;
    soundutilevent_vec_t systems_channel;
};

#endif // LDSOUNDEVENTUTIL_H
