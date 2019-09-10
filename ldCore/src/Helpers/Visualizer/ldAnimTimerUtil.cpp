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
//  ldAnimTimerUtil.cpp
//
//  Created by Eric Brugère on 2/10/15.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//
#include "ldCore/Helpers/Visualizer/ldAnimTimerUtil.h"
#include <QtCore/QDebug>

// ldAnimTimerUtil
ldAnimTimerUtil::ldAnimTimerUtil()
{
    m_maxAnims=0;
    m_maxFamilies=0;
}

//
ldAnimTimerUtil::~ldAnimTimerUtil() { }

// initWithAnimsAndStepFamilies
void ldAnimTimerUtil::initWithAnimsAndStepFamilies(int maxAnims, int maxFamilies)
{
    m_maxAnims=maxAnims;
    m_maxFamilies=maxFamilies;
    anim_timer_struct_vec_t _timers(m_maxAnims, std::vector<AnimTimerStruct>(m_maxFamilies));
    m_timers=_timers;
    // init
    for (int a=0; a<m_maxAnims; a++) {
        for (int f=0; f<m_maxFamilies; f++) {
            initAnimForFamily(a,f);
        }
    }
}

// reinit
void ldAnimTimerUtil::reinit()
{
    // init
    for (int a=0; a<m_maxAnims; a++) {
        for (int f=0; f<m_maxFamilies; f++) {
            m_timers[a][f].duration=0.1f;
        }
    }
}

// update
void ldAnimTimerUtil::update(float delta)
{
    if (!m_maxAnims) return;
    for (int a=0; a<m_maxAnims; a++) {
        for (int f=0; f<m_maxFamilies; f++) {
            if (!m_timers[a][f].isAvailable) {
                m_timers[a][f].elapsed+=delta;
                m_timers[a][f].gradient=m_timers[a][f].elapsed/m_timers[a][f].duration;
                if ( (m_timers[a][f].elapsed+delta) > m_timers[a][f].duration) {
                    initAnimForFamily(a,f);
                }
                // qDebug() << "a/f" << a <<"/"<< f << " timers[a][f].gradient " << timers[a][f].gradient;
            }
        }
    }
}

// startTimer
void ldAnimTimerUtil::startTimer(int animId, int stepFamilyId, float duration)
{
    //qDebug() << "startTimer a/f" << animId <<"/"<< stepFamilyId << " duration " << duration;
    if (!m_maxAnims) return;
    if (animId >= m_maxAnims || stepFamilyId>=m_maxFamilies) return;
    //qDebug() << "           startTimer ok";

    initAnimForFamily(animId,stepFamilyId);
    if (duration <= 0) duration=1.0;
    m_timers[animId][stepFamilyId].duration=duration;
    m_timers[animId][stepFamilyId].isAvailable=false;
    m_timers[animId][stepFamilyId].elapsed=0;
    m_timers[animId][stepFamilyId].gradient=0;
}

// isAnimAvailable
bool ldAnimTimerUtil::isAnimAvailable(int animId, int stepFamilyId)
{
    if (!m_maxAnims) return false;
    if (animId >= m_maxAnims || stepFamilyId>=m_maxFamilies) return false;
    return m_timers[animId][stepFamilyId].isAvailable;
}

// gradientForAnimAndFamily
float ldAnimTimerUtil::gradientForAnimAndFamily(int animId, int stepFamilyId)
{
    if (!m_maxAnims) return 1.0;
    if (animId >= m_maxAnims || stepFamilyId>=m_maxFamilies) return 1.0;
    return m_timers[animId][stepFamilyId].gradient;
}

// durationForAnimAndFamily
float ldAnimTimerUtil::durationForAnimAndFamily(int animId, int stepFamilyId)
{
    if (!m_maxAnims) return 1.0;
    if (animId >= m_maxAnims || stepFamilyId>=m_maxFamilies) return 1.0;
    return m_timers[animId][stepFamilyId].duration;
}

// initAnimForFamily
void ldAnimTimerUtil::initAnimForFamily(int animId, int stepFamilyId)
{
    if (!m_maxAnims) return;
    //if (animId==0)  qDebug() << "initAnimForFamily anim 0";
    m_timers[animId][stepFamilyId].isAvailable=true;
    m_timers[animId][stepFamilyId].elapsed=0;
    m_timers[animId][stepFamilyId].duration=0;
    m_timers[animId][stepFamilyId].gradient=0;
    //
}

