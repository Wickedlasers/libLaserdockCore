//
// ldAnimationVisualizer.cpp
// LaserdockVisualizer
//
// Created by feldspar on 7/3/14.
// Copyright (c) 2014 Wicked Lasers. All rights reserved.
//
#include "ldAnimationVisualizer.h"

#include <QtCore/QFileInfo>

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

#include "Visualizations/Ctrl/ldVisDirectionCtrl.h"

ldAnimationVisualizer::ldAnimationVisualizer(const QString &filePath)
    : ldVisualizer()
    , m_filePath(filePath)
{
    setPosition(ccp(1, 1));
}

ldAnimationVisualizer::~ldAnimationVisualizer() {}

void ldAnimationVisualizer::setDirectionCtrl(ldVisDirectionCtrl *directionCtrl)
{
    QMutexLocker locker(&m_mutex);
    m_directionCtrl = directionCtrl;
}

QString ldAnimationVisualizer::visualizerName() const
{
    return QFileInfo(m_filePath).baseName();
}

void ldAnimationVisualizer::onShouldStart()
{
    QMutexLocker lock(&m_mutex);
    float frameRate = targetFPS();
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
        loadAnimation();
        m_isLoaded = true;
    }

    if (m_asb.frames().size() == 0) return;

    ldMusicManager* m = ldCore::instance()->musicManager();    


    // bpm
    float bpm = m->bestBpm();

    if(cmpf(bpm, 0))
        bpm = m_asb.sourceBPM;

    // set speed
    float speed = bpm / m_asb.sourceBPM;

    // advance
    m_totalFrameDelta += delta * m_asb.sourceFPS * speed;
    int frameDelta = 0;
    if (m_totalFrameDelta >= 1.0f) {
        frameDelta = static_cast<int>(m_totalFrameDelta);
        m_totalFrameDelta -= frameDelta;
    }

    int currentFrame = m_currentFrame;

    ldVisDirectionCtrl::Direction direction = m_directionCtrl
                                                  ? static_cast<ldVisDirectionCtrl::Direction>(m_directionCtrl->get_direction())
                                                  : ldVisDirectionCtrl::Right;
    if(direction == ldVisDirectionCtrl::Right
        || (direction == ldVisDirectionCtrl::LeftRight && !m_isDirectionBack))
        currentFrame += frameDelta;
    else
        currentFrame -= frameDelta;

    const int lastFrameIndex = static_cast<int>(m_asb.frames().size() - 1);

    if(currentFrame < 0) {
        if(direction == ldVisDirectionCtrl::LeftRight) {
            currentFrame = 0 - currentFrame;
            m_isDirectionBack = false;
        } else {
            currentFrame = lastFrameIndex + currentFrame;
        }
    }

    if(currentFrame > lastFrameIndex) {
        if(direction == ldVisDirectionCtrl::LeftRight) {
            currentFrame = lastFrameIndex - (currentFrame - lastFrameIndex);
            m_isDirectionBack = true;
        } else {
            currentFrame = currentFrame - lastFrameIndex;
        }
    }

    // just to make sure we are in borders
    m_currentFrame = std::max(0, std::min(currentFrame, lastFrameIndex));
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

    // draw once
    m_renderer->loadIdentity3();
    // shimmer a/b, color sector, color squares, color plasma
    m_asb.color = m_asb._c1 = m_asb._c2 = C_RED;
    m_asb.drawFrame(m_renderer, m_currentFrame);
}

void ldAnimationVisualizer::loadAnimation()
{
    m_asb.load(m_filePath);
}

int ldAnimationVisualizer::currentFrame() const
{
    return m_currentFrame;
}

