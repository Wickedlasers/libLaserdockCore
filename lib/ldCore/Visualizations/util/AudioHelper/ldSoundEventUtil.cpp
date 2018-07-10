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
//  ldSoundEventUtil.cpp
//
//  Created by Eric Brugère on 2/10/15.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//

#include "ldSoundEventUtil.h"

#include <QtCore/QDebug>

#include "ldCore/ldCore.h"
#include "ldCore/Visualizations/MusicManager/ldMusicManager.h"

// ldSoundEventUtil
ldSoundEventUtil::ldSoundEventUtil()
{
    debug=false;
    didDjStop=false;
    didDjStart=false;
    isMusicReactor=false;
    isTempoTracker=true;
    isAppakaBeat=false;
    maxChannelSize=0;
    //
    m_musicManager = ldCore::instance()->musicManager();
    previousIsSilent = !m_musicManager->appakaGate->isSilent;
    isSilent = m_musicManager->appakaGate->isSilent;
}

// ~ldSoundEventUtil
ldSoundEventUtil::~ldSoundEventUtil() { }

// addMusicReactor
void ldSoundEventUtil::addMusicReactor()
{
    isMusicReactor=true;
//    // music reactor
//    float t3fa[MRPARAMS] = {0.50,0.00,0.81,0.00,0.24,0.69,0.97,0.18,0.25,0.48,0.34,0.48,0.22,0.85,0.85,0.25,0.25,0.25,0.25,0.25};
//    musicReactor = new MusicReactor();
//    musicReactor->setParams(t3fa);
}

// addTempoTracker
void ldSoundEventUtil::addTempoTracker()
{
    isTempoTracker=true;
    // aubio
//    tempoTracker=new TempoTracker();
}

// addAppakabarBeat
void ldSoundEventUtil::addAppakabarBeat()
{
    isAppakaBeat=true;
    // appakabar beat
//    appakaBeat=new ldAppakaBeat();
}

// initWithMaxChanel
void ldSoundEventUtil::initWithMaxChannel(int max)
{
    maxChannelSize=max;
    soundutilevent_vec_t _systems_channel(SOUND_EVENT_UTIL_ENUM_SIZE, std::vector<SoundEventUtilStruct>(maxChannelSize));
    systems_channel=_systems_channel;
    // init
    for (int i=0; i<SOUND_EVENT_UTIL_ENUM_SIZE; i++) {
        for (int j=0; j<maxChannelSize; j++) {
            systems_channel[i][j].drop=0;
            systems_channel[i][j].dropCount=0;
            systems_channel[i][j].ceil=1.0;
            systems_channel[i][j].isEvent=false;
            systems_channel[i][j].isAnimationBusy=false;
        }
    }
}

// update
void ldSoundEventUtil::update(ldSoundData* pSoundData, float /*delta*/)
{
    if (pSoundData == NULL || !maxChannelSize) return;

    ldMusicManager* m = m_musicManager;

    // isSilent
    isSilent=m->appakaGate->isSilent;

    if (previousIsSilent != m->appakaGate->isSilent) {
        if (!m->appakaGate->isSilent) {
            //qDebug() << "Dj Start";
            didDjStart=true;
        } else {
            //qDebug() << "Dj Stop";
            didDjStop=true;
        }
    } else {
        //qDebug() << "No change";
        didDjStop=false;
        didDjStart=false;
    }
    previousIsSilent = m->appakaGate->isSilent;

    // update events
    for (int i=0; i<maxChannelSize; i++)
    {
        if (isMusicReactor) updateSystem(SOUND_EVENT_UTIL_MUSIC_REACTOR, i);
        if (isTempoTracker) updateSystem(SOUND_EVENT_UTIL_TEMPO_TRACKER, i);
        if (isAppakaBeat) {
            updateSystem(SOUND_EVENT_UTIL_APPAKABAR_BEAT, i);
            updateSystem(SOUND_EVENT_UTIL_APPAKABAR_PEAK, i);
        }
    }
}

// updateSystem
void ldSoundEventUtil::updateSystem(int system, int channel)
{
    ldMusicManager* m = m_musicManager;

    bool isEvent=false;
    systems_channel[system][channel].isEvent=isEvent;
    //
    switch (system) {
    case SOUND_EVENT_UTIL_MUSIC_REACTOR:
        if ( m->mrSlowBass->statOutput > systems_channel[system][channel].ceil ) isEvent=true;
        break;
    case SOUND_EVENT_UTIL_TEMPO_TRACKER:
        if ( m->tempoTrackerSlow->output >= 1.0 ) isEvent=true;
        break;
    case SOUND_EVENT_UTIL_APPAKABAR_BEAT:
            if ( m->appakaPeak->output >= 0.9 ) {
        //if ( m->appakaBeat->outputRealTimePeak >= 1.0) {
        //if ( m->appakaBeat->output >= 1.0) {
            //if (channel==0) qDebug() << "SOUND_EVENT_UTIL_APPAKABAR_BEAT" << systems_channel[system][channel].dropCount;
            isEvent=true;
        }
        break;
    case SOUND_EVENT_UTIL_APPAKABAR_PEAK:
        //if ( m->appakaBeat->outputRealTimePeak >= 1.0) isEvent=true;
            if ( m->appakaPeak->output >= 0.9 ) isEvent=true;
            
        break;
    default:
        break;
    }
    if (didDjStop || didDjStart) isEvent=true; // help to update if Dj event occurs..

    if (!isEvent) return;

    //if (debug) qDebug() << "updateSystem system"<< system << "channel"<< channel ;
    //if (debug) qDebug() << " drop"<< systems_channel[system][channel].drop << "dropCount" << systems_channel[system][channel].dropCount ;

    if (systems_channel[system][channel].dropCount==0) {
        if (debug) qDebug() << "event ok *****";
        systems_channel[system][channel].dropCount = 1-systems_channel[system][channel].drop;
        systems_channel[system][channel].isEvent=isEvent;
        //if (channel==0)  qDebug() << "event ok *****"  << system <<"-"<< systems_channel[system][channel].dropCount << "/" << systems_channel[system][channel].drop;
    } else {
        if (debug) qDebug() << "event dropped";
        systems_channel[system][channel].dropCount++;
    }

    if (isTempoTracker && m->tempoTrackerSlow->bpm > 0 && isAppakaBeat && m->appakaBeat->bpm > 0) {
       // qDebug() <<"tempoTracker " << tempoTracker->bpm<<" | appakaBeat bpm" << appakaBeat->bpm<<"bpmPeak" << appakaBeat->bpmPeak ;
    }
}

// setSystemDropAndSensibilityForChannel
void ldSoundEventUtil::setSystemDropAndSensibilityForChannel(int system, int channel, int drop, float ceil)
{
    if (channel >= maxChannelSize) return;
    systems_channel[system][channel].drop = drop;
    systems_channel[system][channel].ceil = ceil;
}

// setSystemEventBusy
void ldSoundEventUtil::setSystemEventBusy(int system, int channel, bool busy)
{
    if (channel >= maxChannelSize) return;
    //if (debug) qDebug() << "setSystemEventBusy system"<< system << "channel"<< channel ;
    //if (debug) qDebug() << "  isAnimationBusy" << systems_channel[system][channel].isAnimationBusy << "  busy" << busy;
    systems_channel[system][channel].isAnimationBusy=busy;
}

// isSystemEvent
bool ldSoundEventUtil::isSystemEvent(int system, int channel)
{
    if (channel >= maxChannelSize) return false;
    return systems_channel[system][channel].isEvent;
}

// isSystemEventBusy
bool ldSoundEventUtil::isSystemEventBusy(int system, int channel)
{
    if (channel >= maxChannelSize) return false;
    return systems_channel[system][channel].isAnimationBusy;
}

// bpm
float ldSoundEventUtil::bpm()
{
    ldMusicManager* m = m_musicManager;
    return m->bestBpm;
}
