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

#include "ldCore/Helpers/Audio/ldSoundEventUtil.h"

#include <QtCore/QDebug>

#include "ldCore/ldCore.h"
#include "ldCore/Helpers/Audio/ldAppakPeaks.h"
#include "ldCore/Visualizations/MusicManager/ldMusicManager.h"

// ldSoundEventUtil
ldSoundEventUtil::ldSoundEventUtil()
{
//    m_debug=true;

    m_previousIsSilent = !ldCore::instance()->musicManager()->appakaGate->isSilent();
    isSilent = ldCore::instance()->musicManager()->appakaGate->isSilent();
}

// ~ldSoundEventUtil
ldSoundEventUtil::~ldSoundEventUtil() { }

// addMusicReactor
void ldSoundEventUtil::addMusicReactor()
{
    m_isMusicReactor = true;
//    // music reactor
//    float t3fa[MRPARAMS] = {0.50,0.00,0.81,0.00,0.24,0.69,0.97,0.18,0.25,0.48,0.34,0.48,0.22,0.85,0.85,0.25,0.25,0.25,0.25,0.25};
//    musicReactor = new MusicReactor();
//    musicReactor->setParams(t3fa);
}

// addTempoTracker
void ldSoundEventUtil::addTempoTracker()
{
    m_isTempoTracker=true;
    // aubio
//    tempoTracker=new TempoTracker();
}

// addAppakabarBeat
void ldSoundEventUtil::addAppakabarBeat()
{
    m_isAppakaBeat=true;
    // appakabar beat
//    appakaBeat=new ldAppakaBeat();
}

// initWithMaxChanel
void ldSoundEventUtil::initWithMaxChannel(int max)
{
    m_maxChannelSize=max;
    soundeventinfo_vec_t _systems_channel(System::EnumSize, std::vector<SoundEventInfo>(m_maxChannelSize));
    m_systems_channel=_systems_channel;
    // init
    for (int i=0; i<System::EnumSize; i++) {
        for (int j=0; j<m_maxChannelSize; j++) {
            m_systems_channel[i][j].drop=0;
            m_systems_channel[i][j].dropCount=0;
            m_systems_channel[i][j].ceil=1.0;
            m_systems_channel[i][j].isEvent=false;
            m_systems_channel[i][j].isAnimationBusy=false;
        }
    }
}

// update
void ldSoundEventUtil::update(ldSoundData* pSoundData, float /*delta*/)
{
    if (pSoundData == NULL || !m_maxChannelSize) return;

    ldMusicManager* m = ldCore::instance()->musicManager();

    // isSilent
    isSilent=m->appakaGate->isSilent();

    if (m_previousIsSilent != m->appakaGate->isSilent()) {
        if (!m->appakaGate->isSilent()) {
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
    m_previousIsSilent = m->appakaGate->isSilent();

    // update events
    for (int i=0; i<m_maxChannelSize; i++)
    {
        if (m_isMusicReactor) updateSystem(MusicReactor, i);
        if (m_isTempoTracker) updateSystem(TempoTracker, i);
        if (m_isAppakaBeat) {
            updateSystem(AppakabarBeat, i);
            updateSystem(AppakabarPeak, i);
        }
    }
}

// updateSystem
void ldSoundEventUtil::updateSystem(System system, int channel)
{
    ldMusicManager* m = ldCore::instance()->musicManager();

    bool isEvent=false;
    m_systems_channel[system][channel].isEvent=isEvent;
    //
    switch (system) {
    case MusicReactor:
        if ( m->mrSlowBass->statOutput > m_systems_channel[system][channel].ceil ) isEvent=true;
        break;
    case TempoTracker:
        if ( m->tempoTrackerSlow->output() >= 1.0 ) isEvent=true;
        break;
    case AppakabarBeat:
            if ( m->peaks()->output() >= 0.9 ) {
        //if ( m->appakaBeat->outputRealTimePeak >= 1.0) {
        //if ( m->appakaBeat->output >= 1.0) {
            //if (channel==0) qDebug() << "ldSoundEventUtil::System::AppakabarBeat" << systems_channel[system][channel].dropCount;
            isEvent=true;
        }
        break;
    case AppakabarPeak:
        //if ( m->appakaBeat->outputRealTimePeak >= 1.0) isEvent=true;
            if ( m->peaks()->output() >= 0.9f ) isEvent=true;
            
        break;
    default:
        break;
    }
    if (didDjStop || didDjStart) isEvent=true; // help to update if Dj event occurs..

    if (!isEvent) return;

    //if (debug) qDebug() << "updateSystem system"<< system << "channel"<< channel ;
    //if (debug) qDebug() << " drop"<< systems_channel[system][channel].drop << "dropCount" << systems_channel[system][channel].dropCount ;

    if (m_systems_channel[system][channel].dropCount==0) {
        if (m_debug) qDebug() << "event ok *****";
        m_systems_channel[system][channel].dropCount = 1-m_systems_channel[system][channel].drop;
        m_systems_channel[system][channel].isEvent=isEvent;
        //if (channel==0)  qDebug() << "event ok *****"  << system <<"-"<< systems_channel[system][channel].dropCount << "/" << systems_channel[system][channel].drop;
    } else {
        if (m_debug) qDebug() << "event dropped";
        m_systems_channel[system][channel].dropCount++;
    }

    if (m_isTempoTracker && m->tempoTrackerSlow->bpm() > 0 && m_isAppakaBeat && m->appakaBeat->bpm > 0) {
       // qDebug() <<"tempoTracker " << tempoTracker->bpm<<" | appakaBeat bpm" << appakaBeat->bpm<<"bpmPeak" << appakaBeat->bpmPeak ;
    }
}

// setSystemDropAndSensibilityForChannel
void ldSoundEventUtil::setSystemDropAndSensibilityForChannel(System system, int channel, int drop, float ceil)
{
    if (channel >= m_maxChannelSize) return;
    m_systems_channel[system][channel].drop = drop;
    m_systems_channel[system][channel].ceil = ceil;
}

// setSystemEventBusy
void ldSoundEventUtil::setSystemEventBusy(System system, int channel, bool busy)
{
    if (channel >= m_maxChannelSize) return;
    //if (debug) qDebug() << "setSystemEventBusy system"<< system << "channel"<< channel ;
    //if (debug) qDebug() << "  isAnimationBusy" << systems_channel[system][channel].isAnimationBusy << "  busy" << busy;
    m_systems_channel[system][channel].isAnimationBusy=busy;
}

// isSystemEvent
bool ldSoundEventUtil::isSystemEvent(System system, int channel)
{
    if (channel >= m_maxChannelSize) return false;
    return m_systems_channel[system][channel].isEvent;
}

// isSystemEventBusy
bool ldSoundEventUtil::isSystemEventBusy(System system, int channel)
{
    if (channel >= m_maxChannelSize) return false;
    return m_systems_channel[system][channel].isAnimationBusy;
}
