//
// ldLdva2AnimationVisualizer.cpp
// ldCore
//
// Created by feldspar on 7/3/14.
// Copyright (c) 2014 Wicked Lasers. All rights reserved.
//
#include "ldLdva2AnimationVisualizer.h"

#include "ldCore/ldCore.h"
#include <ldCore/Filter/ldColorUtils.h>
#include "ldCore/Filter/ldFilterColorize.h"
#include <ldCore/Sound/ldSoundData.h>
#include "ldCore/Visualizations/MusicManager/ldMusicManager.h"
#include "ldCore/Visualizations/util/AudioHelper/ldTempoAC.h"
#include "ldCore/Visualizations/util/AudioHelper/ldHybridReactor.h"
#include "ldCore/Visualizations/util/SVGHelper/ldSvgReader.h"
#include "ldCore/Visualizations/util/VisualizerHelper/ldAnimationSequence.h"
#include "ldCore/Visualizations/util/VisualizerHelper/ldLaserFilter.h"

ldLdva2AnimationVisualizer::ldLdva2AnimationVisualizer() {

    // defaults
    m_fps = 42;

    useOldAlg = false;
    doReverse = false;
    doSynchSpeed = false;
    speedAllowSlow = false;
    doWrapOnKeyEnd = false;
    doJumpToKeyStartOnBeat = false;
    jumpBeatMidpoint = false;
    jumpBeatCount = 1;

    doColorCircle = false;
    doColorRise = false;

    doZoom = false;
    doEcho = false;

    isLoaded = false;

    currentFrame = 0;
    echoFrame = 0;
    jumpBeatCounter = 0;       
    rev = 0;
    ckey = 0;


}

void ldLdva2AnimationVisualizer::onAnimLoadSequence() {


    qDebug() << "ERROR you shoudl override void ldLdva2AnimationVisualizer::onAnimLoadSequence() {";
    qDebug() << "loading cheerleader default ";


    // load ldva2
    asb.load2(QString(ldCore::instance()->resourceDir() + "/ldva2/cheerleader.ldva2").toUtf8().constData());

    // apply animation settings and styles
    doSynchSpeed = true;
    doReverse = true;
    doWrapOnKeyEnd = false;
    doJumpToKeyStartOnBeat = true;
    jumpBeatMidpoint = false;
    jumpBeatCount = 2;
    useOldAlg = false;
    doZoom = false;
    doEcho = false;

    
}

void ldLdva2AnimationVisualizer::onAnimBeforeRender() {

}


ldLdva2AnimationVisualizer::~ldLdva2AnimationVisualizer() {}

bool ldLdva2AnimationVisualizer::init()
{
    if (ldVisualizer::init())
    {
        setPosition(ccp(1, 1));
        setContentSize(CCSizeMake(2, 2));
        setAnchorPoint(ccp(0.5, 0.5));
        return true;
    }
    return false;
}
void ldLdva2AnimationVisualizer::onShouldStart() {}
void ldLdva2AnimationVisualizer::clearBuffer() {}

static float getClosestBeat(float r, float t, bool slow) {
    
    for (int i = 0; i < 5; i++) if (r < t) r = r * 2;
    for (int i = 0; i < 5; i++) if (r / 2 > t) r = r / 2;
        
    
    if (!slow) {
        //if (r * 0.667 > t) return r * 0.667;
        if (r * 0.750 > t) return r * 0.750;
        return r;
    }
    
    // slow
    
    if (slow) {
        float bpm = r;
        float bestdiff = fabsf(logf(bpm) - logf(t));

        {
            float test = r / 2;
            float diff = fabsf(logf(test) - logf(t));
            if (diff < bestdiff) { bestdiff = diff; bpm = test; }
        }
        {
            float test = r * 2 / 3;
            float diff = fabsf(logf(test) - logf(t));
            if (diff < bestdiff) {bestdiff = diff; bpm = test;}
        }
        return bpm;
    }

    return r;

}

void ldLdva2AnimationVisualizer::onUpdate(ldSoundData* /*pSoundData*/, float /*delta*/) {
    
    if (!isLoaded) {
        isLoaded = true;
        this->onAnimLoadSequence();
    }

    if (asb.frames.size() == 0) return;

    ldMusicManager* m = ldCore::instance()->musicManager();    
        
    if (useOldAlg) {

        int* keyHead = asb.keyStart;
        int* keyTail = asb.keyEnd;
        // get key and offset from anima
        int key = 0;
        int offset = 0;
        key = m->hybridAnima->outputBaseFrameIndex;
        offset = m->hybridAnima->outputTrackPosition * (keyTail[key] - keyHead[key] + 1);
        // loop offset within key
        if (offset < 0) offset = 0;
        // set index
        currentFrame = keyHead[key] + offset;
        if (currentFrame >= keyTail[key]) currentFrame = keyTail[key] - 1;

    } else {
        
//        ldMusicManager* m = ldCore::instance()->musicManager();
        // test reverse        
        rev = false;        
        if (doReverse) {
            if (m->tempoACSlower->phaseSmooth < 0.50) rev = true;
            if (m->tempoACSlower->phaseSmooth < 0.25) rev = false;
            if (m->tempoACSlower->phaseReactive > 0.50) rev = false;
        }
        // set speed                
        float speed = 1;        
        //{
            float bpm = m->tempoACSlower->bpmSmooth;
            bpm = getClosestBeat(bpm, asb.sourceBPM, speedAllowSlow);
            speed = bpm / asb.sourceBPM;            
        //}
        if (!doSynchSpeed) speed = 1;
        // advance
        frx += AUDIO_UPDATE_DELTA_S * asb.sourceFPS * speed;
        int irx = 0;
        if (frx >= 1.0f) {
            irx = (int)frx;
            frx -= irx;
        }
        if (rev) irx = -irx;
        currentFrame += irx;
        // wrap
        if (doWrapOnKeyEnd) {
            // loop under
            if (currentFrame < asb.keyStart[ckey]) {                
                currentFrame = asb.keyEnd[ckey];
            }
            // loop over
            if (currentFrame > asb.keyEnd[ckey]) {                
                ckey++;
                ckey %= 8;
                currentFrame = asb.keyStart[ckey];
            }
        }    
        // keep in size
        currentFrame += asb.frames.size();
        currentFrame %= asb.frames.size();
        // check clip restart
        if (doJumpToKeyStartOnBeat) {
            if (m->tempoACSlower->phaseSmooth == 1 && m->tempoACSlow->phaseSmooth <= 1) {
                jumpBeatCounter++;
                if (jumpBeatCounter >= jumpBeatCount) jumpBeatCounter = 0;
                if (jumpBeatCounter == 0) {                    
                    ckey = rand() % 8;
                    currentFrame = asb.keyStart[ckey];
                    if (jumpBeatMidpoint) {
                        float oo = speed * asb.sourceFPS / (bpm / 60.0);
                        currentFrame = (asb.keyStart[ckey] + asb.keyEnd[ckey]) / 2 - oo;
                        currentFrame = MAX(currentFrame, asb.keyStart[ckey]);
                    }
                }
            }
        }

    }
        
}



void ldLdva2AnimationVisualizer::draw()
{
    // load if needed
    if (!isLoaded) return;
    if (asb.frames.size() == 0) return;

    //qDebug() currentFrame;

    // render settings

    float frameRate = m_fps;
    int frameLen = m_renderer->rate() / (frameRate * 1.05f); //

    OLRenderParams params;
    memset(&params, 0, sizeof params);
    params.rate = m_renderer->rate();
    params.on_speed = 1.0f / 40.0f;
    params.off_speed = 1.0f / 40.0f;
    params.start_wait = 2; // 16
    params.start_dwell = 1;
    params.curve_dwell = 1;
    params.corner_dwell = 2;
    params.curve_angle = cosf(30.0f*(M_PIf / 180.0f)); // 30 deg
    params.end_dwell = 1;
    params.end_wait = 1;
    params.snap = 1 / 1000.0f;
    params.render_flags = RENDER_NOREVERSE | RENDER_NOREORDER;    
    params.flatness = 0.00001f; //for beziers to work (thanks Alec!)
    params.max_framelen = frameLen;
    m_renderer->setRenderParams(&params);

    m_renderer->loadIdentity();
    m_renderer->loadIdentity3();
        

    // color filter    s
    static ldFilterColorFade colorFilter;
    ldMusicManager*m = m_musicManager;
    colorFilter.offset = 1 - m_musicManager->hybridAnima->outputTrackPosition;
    colorFilter.huebase = m_musicManager->hybridAutoColor2->selectorColorHue1.indexFloat / 4.0f + 0.17f;
    colorFilter.huerange = (m_musicManager->hybridAutoColor2->selectorColorHue2.indexFloat / 5.0f + 0.33f) / 4.0f / 4.0f;
    colorFilter.freq = 0.33f;

    static ldFilterColorDrop colorFilter2;

    static ldFilterColorLift colorFilter3;
    //colorFilter.customize = false; // nyi
    //colorFilter3.cde = m_musicManager->musicFeature1->statMoodFunky * 250;//m_musicManager->mrSlowTreb->spinOutput4 * 180;
    //colorFilter3.cde2 = m_musicManager->hybridAnima->outputTrackPosition;
    //colorFilter3.cde3 = 1 - m_musicManager->tempoACFast->phaseSmooth;
    //(no)//colorFilter3.nde = m_musicManager->musicFeature1->statMoodMelodic * 250;// m_musicManager->mrSlowTreb->spinOutput4*250;
    //colorFilter3.nde = m_musicManager->mrSlowTreb->spinOutput4*250;

    if (doColorCircle) {
        setPixelShader((ldShader*)&colorFilter2);
    } else if (doColorRise) {
        setPixelShader((ldShader*)&colorFilter3);
    } else {
        setPixelShader((ldShader*)&colorFilter);
    }
        
    this->onAnimBeforeRender();

    // get zoom values
    float beat1 = m->tempoTrackerSlow->output;
//    float beat2 = m->tempoTrackerFast->output;
    float zoom1 = 1;
    float zoom2 = 1;    
    if (doZoom) {
        zoom1 = zoom2 = 0.80 + 0.20 * beat1;
        if (doEcho) zoom1 = 0.75;
    }
    
    // draw once
    m_renderer->loadIdentity3();
    m_renderer->scale3(zoom1, zoom1, 1);
    asb.color = asb._c1 = asb._c2 = C_GREY(255);
    asb.drawFrame(m_renderer, currentFrame);

    // draw again
    if (doEcho) {        
        if (echoFrame == -1) echoFrame = currentFrame;        

        m_renderer->loadIdentity3();
        m_renderer->scale3(zoom2, zoom2, 1);        
        asb.color = asb._c1 = asb._c2 = C_GREY(100);
        asb.drawFrame(m_renderer, echoFrame);

        echoFrame = currentFrame;
    }

}


