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

#include "ldCore/Helpers/Audio/ldMultipleBeatDetector.h"

#include <QtCore/QDebug>
#include "ldCore/ldCore.h"

#include <ldCore/Helpers/Audio/ldBeatDetector.h>

ldMultipleBeatDetector::ldMultipleBeatDetector(const ldBeatDetector *beatDetector, QObject *parent)
    : QObject(parent)
    , m_isActive(false)
    , m_beatDetector(beatDetector)
    , m_src(ldCore::instance()->musicManager()->getBpmSource())
{
    connect(this, &ldMultipleBeatDetector::isActiveChanged, this, &ldMultipleBeatDetector::onActiveChanged);
    //connect(m_beatDetector, &ldBeatDetector::beatDetected, this, &ldMultipleBeatDetector::onBeatDetected);

    // keep track of the source of the incoming beats
    connect(ldCore::instance()->musicManager(),&ldMusicManager::beatSourceChanged,this,[&](ldMusicManager::bpmSource src){
        m_src = src;
        m_synced = false; // always resync on beat source change
    });

    // if any ableton params are changed we need to restart the quantum 1st beat syncing
    connect(ldCore::instance()->musicManager(),&ldMusicManager::abletonParamsUpdated,this,[&](){ m_synced = false;});

    connect(ldCore::instance()->musicManager(),&ldMusicManager::beatDetected,this,&ldMultipleBeatDetector::onBeatDetected);
    connect(ldCore::instance()->musicManager(),&ldMusicManager::barDetected,this,[&](){ // quantum beat (ie. 1st beat of a bar)
        if (m_isActive && (m_src==ldMusicManager::BpmSourceAbletonLink)) {
            if (!m_synced) { // waiting for sync to ableton first quantum beat?
                m_synced = true;
                emit isSyncing(false);
                //qDebug() << "start of bar detected";
                m_detectedBeats = 0;
                emit beatDetected();
            }
            else {
                onBeatDetected();
            }
        }
    });

}

ldMultipleBeatDetector::~ldMultipleBeatDetector()
{
}

void ldMultipleBeatDetector::setBeatCount(int beatCount)
{
    m_beatCount = beatCount;
    if(m_isActive)
        reset();
}

void ldMultipleBeatDetector::reset()
{
    m_detectedBeats = 0;
}

float ldMultipleBeatDetector::progress() const
{
//    qDebug() << m_detectedBeats << m_beatCount;
    Q_ASSERT(m_beatCount != 0);

    if(m_detectedBeats >= m_beatCount)
        return 1;

    float progressF = static_cast<float>(m_detectedBeats) / m_beatCount;
    float partialProgress = m_beatDetector->progress() / m_beatCount;
    return progressF + partialProgress;
}

void ldMultipleBeatDetector::onActiveChanged(bool active)
{
    m_syncFlash = false;

    if(!active) {
        reset();
        emit isSyncing(false);
    }
}

void ldMultipleBeatDetector::onBeatDetected()
{
    if(!m_isActive)
        return;

    // check if we are waiting to sync to the first quantum beat
    // when ableton link is the beat source
    if (m_src==ldMusicManager::BpmSourceAbletonLink) {
        if (!m_synced) {
            m_syncFlash=!m_syncFlash;
            emit isSyncing(m_syncFlash); // toggle for icon sync flash
            return;
        }
    }

    m_detectedBeats++;

    //qDebug() << __FUNCTION__ << "beat" << m_detectedBeats << "detected";

    if(m_detectedBeats >= m_beatCount) {
        //qDebug() << "******* advance";
        m_detectedBeats = 0;
        emit beatDetected();
    }
}

