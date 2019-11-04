//
// ldAnimationVisualizer.cpp
// LaserdockVisualizer
//
// Created by feldspar on 7/3/14.
// Copyright (c) 2014 Wicked Lasers. All rights reserved.
//
#include "ldAnimationVisualizer.h"

#include "ldCore/ldCore.h"
#include "ldCore/Filter/ldFilterColorize.h"
#include <ldCore/Helpers/ldRandomGenerator.h>
#include "ldCore/Helpers/Audio/ldAppakPeaks.h"
#include "ldCore/Helpers/Audio/ldTempoAC.h"
#include "ldCore/Helpers/Audio/ldTempoTracker.h"
#include "ldCore/Helpers/Audio/ldHybridReactor.h"
#include <ldCore/Helpers/Color/ldColorUtil.h>
#include "ldCore/Helpers/SVG/ldSvgReader.h"
#include "ldCore/Helpers/Visualizer/ldAnimationSequence.h"
#include "ldCore/Helpers/Visualizer/ldLaserFilter.h"
#include <ldCore/Sound/ldSoundData.h>
#include "ldCore/Visualizations/MusicManager/ldMusicManager.h"

namespace {
static float getClosestBeat(float currentBpm, float sourceBpm, bool slow) {

    for (int i = 0; i < 5; i++)
        if (currentBpm < sourceBpm)
            currentBpm = currentBpm * 2;

    for (int i = 0; i < 5; i++)
        if (currentBpm / 2 > sourceBpm)
            currentBpm = currentBpm / 2;


    if (!slow) {
        //if (r * 0.667 > t) return r * 0.667;
        if (currentBpm * 0.750 > sourceBpm)
            return currentBpm * 0.750;
        return currentBpm;
    } else {
        // slow
        float bpm = currentBpm;
        float bestDiff = fabsf(logf(bpm) - logf(sourceBpm));

        {
            float test = currentBpm / 2;
            float diff = fabsf(logf(test) - logf(sourceBpm));
            if (diff < bestDiff) {
                bestDiff = diff;
                bpm = test;
            }
        }
        {
            float test = currentBpm * 2 / 3;
            float diff = fabsf(logf(test) - logf(sourceBpm));
            if (diff < bestDiff) {
                bestDiff = diff;
                bpm = test;
            }
        }
        return bpm;
    }

    return currentBpm;
}
}

ldAnimationVisualizer::ldAnimationVisualizer()
{
    setPosition(ccp(1, 1));
}

ldAnimationVisualizer::~ldAnimationVisualizer() {}

void ldAnimationVisualizer::onShouldStart()
{
    QMutexLocker lock(&m_mutex);
    float frameRate = m_fps;
    int frameLen = m_renderer->rate() / (frameRate*1.05f);

    OLRenderParams params;
    memset(&params, 0, sizeof params);
    params.rate = m_renderer->rate();
    params.on_speed = 1.0f / 40.0f;
    params.off_speed = 1.0f / 40.0f;
    params.start_wait = 2; // 16
    params.start_dwell = 1;
    params.curve_dwell = 1;
    params.corner_dwell = 2;
    params.curve_angle = cosf(30.0f*(M_PIf / 180.0)); // 30 deg
    params.end_dwell = 1;
    params.end_wait = 1;
    params.snap = 1 / 1000.0f;
    params.render_flags = RENDER_NOREVERSE | RENDER_NOREORDER;
    params.flatness = 0.00001f; //for beziers to work (thanks Alec!)
    params.max_framelen = frameLen;
    m_renderer->setRenderParams(&params);
}

void ldAnimationVisualizer::onUpdate(ldSoundData* /*pSoundData*/, float delta) {
    
    QMutexLocker lock(&m_mutex);

    if (!m_isLoaded) {
        this->loadAnimation();
        m_isLoaded = true;
    }

    if (m_asb.frames().size() == 0) return;

    ldMusicManager* m = ldCore::instance()->musicManager();    


    //        ldMusicManager* m = ldCore::instance()->musicManager();
    // test reverse
    bool isReverse = false;
    if (m_doReverse) {
        if (m->tempoACSlower->phaseSmooth < 0.50) isReverse = true;
        if (m->tempoACSlower->phaseSmooth < 0.25) isReverse = false;
        if (m->tempoACSlower->phaseReactive > 0.50) isReverse = false;
    }

    // bpm
    float bpm = 0;
    if(m_usePeakBpm) {
        //            bpm = m->peaks()->bpm();
        //        bpm = m->appakaPeak->bpm;
        //        bpm = m->appakaBeat->bpm;
        bpm = m->bestBpm();
        //        qDebug() << m->appakaPeak->lastBmpApproximation;
        //        qDebug() << m->tempoACSlower->bpmSmooth << m->tempoTrackerSlow->bpm() << m->appakaBeat->bpm << m->bestBpm();
        //        qDebug() << bpm << asb.sourceBPM;
    } else {
        bpm = m->slowBpm();
        bpm = getClosestBeat(bpm, m_asb.sourceBPM, m_speedAllowSlow);
    }

    if(bpm == 0) {
        bpm = m_asb.sourceBPM;
    }

    // set speed
    float speed = 1;
    if(m_doSynchSpeed)
        speed = bpm / m_asb.sourceBPM;

    // advance
    m_totalFrameDelta += delta * m_asb.sourceFPS * speed;
    int frameDelta = 0;
    if (m_totalFrameDelta >= 1.0f) {
        frameDelta = (int) m_totalFrameDelta;
        m_totalFrameDelta -= frameDelta;
    }
    if (isReverse) frameDelta = -frameDelta;
    m_currentFrame += frameDelta;
    // wrap
    if (m_doWrapOnKeyEnd) {
        // loop under
        if (m_currentFrame < m_asb.keyStart[m_ckey]) {
            m_currentFrame = m_asb.keyEnd[m_ckey];
        }
        // loop over
        if (m_currentFrame > m_asb.keyEnd[m_ckey]) {
            m_ckey++;
            m_ckey %= 8;
            m_currentFrame = m_asb.keyStart[m_ckey];
        }
    }
    // keep in size
    m_currentFrame += m_asb.frames().size();
    m_currentFrame %= m_asb.frames().size();
    // check clip restart
    if (m_doJumpToKeyStartOnBeat) {
        if (m->tempoACSlower->phaseSmooth == 1 && m->tempoACSlow->phaseSmooth <= 1) {
            m_jumpBeatCounter++;
            if (m_jumpBeatCounter >= m_jumpBeatCount) m_jumpBeatCounter = 0;
            if (m_jumpBeatCounter == 0) {
                m_ckey = rand() % 8;
                m_currentFrame = m_asb.keyStart[m_ckey];
                if (m_jumpBeatMidpoint) {
                    float oo = speed * m_asb.sourceFPS / (bpm / 60.0);
                    m_currentFrame = (m_asb.keyStart[m_ckey] + m_asb.keyEnd[m_ckey]) / 2 - oo;
                    m_currentFrame = MAX(m_currentFrame, m_asb.keyStart[m_ckey]);
                }
            }
        }
    }
}



void ldAnimationVisualizer::draw()
{
    QMutexLocker lock(&m_mutex);
    // load if needed
    if (!m_isLoaded) return;
    if (m_asb.frames().size() == 0) return;

//    qDebug() << m_currentFrame;

    // render settings

    m_renderer->loadIdentity();
    m_renderer->loadIdentity3();


    this->prepareBeforeRender();

    // get zoom values
    float beat1 = m_musicManager->tempoTrackerSlow()->output();
//    float beat2 = m->tempoTrackerFast->output;
    float zoom1 = 1;
    float zoom2 = 1;    
    if (m_doZoom) {
        zoom1 = zoom2 = 0.80 + 0.20 * beat1;
        if (m_doEcho) zoom1 = 0.75;
    }
    
    // draw once
    m_renderer->loadIdentity3();
    m_renderer->scale3(zoom1, zoom1, 1);
    // shimmer a/b, color sector, color squares, color plasma
    m_asb.color = m_asb._c1 = m_asb._c2 = C_RED;
    m_asb.drawFrame(m_renderer, m_currentFrame);

    doEcho(zoom2);
}

void ldAnimationVisualizer::doEcho(float zoom2)
{
    // draw again
    if (m_doEcho) {
        if (m_echoFrame == -1) m_echoFrame = m_currentFrame;

        m_renderer->loadIdentity3();
        m_renderer->scale3(zoom2, zoom2, 1);

        m_asb.color = m_asb._c1 = m_asb._c2 = C_RED_I(0.3*255);
        m_asb.drawFrame(m_renderer, m_echoFrame);

        m_echoFrame = m_currentFrame;
    }
}


