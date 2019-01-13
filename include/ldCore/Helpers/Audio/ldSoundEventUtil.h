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

#include "ldCore/Helpers/Visualizer/ldVisualizerHelper.h"

class LDCORESHARED_EXPORT ldSoundEventUtil
{
public:
    enum System {
        MusicReactor,
        TempoTracker,
        AppakabarBeat,
        AppakabarPeak,
        EnumSize
    };

    ldSoundEventUtil();
    ~ldSoundEventUtil();

    void addMusicReactor();
    void addTempoTracker();
    void addAppakabarBeat();
    void initWithMaxChannel(int max);

    void update(ldSoundData* pSoundData, float delta);

    void setSystemDropAndSensibilityForChannel(System system, int channel, int drop, float ceil);
    void setSystemEventBusy(System system, int channel, bool busy);
    bool isSystemEvent(System system, int channel);
    bool isSystemEventBusy(System system, int channel);

    bool isSilent = false;
    bool didDjStop = false;
    bool didDjStart = false;

private:
    struct SoundEventInfo {
        int drop;
        int dropCount;
        float ceil;
        bool isEvent;
        bool isAnimationBusy;
    };
    typedef std::vector<std::vector<SoundEventInfo> > soundeventinfo_vec_t;

    void updateSystem(System system, int channel);

    bool m_debug = false;
    bool m_isMusicReactor = false;
    bool m_isTempoTracker = true;
    bool m_isAppakaBeat = false;
    int m_maxChannelSize = 0;
    bool m_previousIsSilent = false;
    soundeventinfo_vec_t m_systems_channel;

    //    MusicReactor *musicReactor;
    //    TempoTracker *tempoTracker;
    //    ldAppakaBeat *appakaBeat;

};

#endif // LDSOUNDEVENTUTIL_H
