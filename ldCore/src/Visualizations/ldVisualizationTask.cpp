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

#include "ldCore/Visualizations/ldVisualizationTask.h"

#include <cmath>

#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtQml/QtQml>
#include <QtMultimedia/QAudioInput>

#include "ldCore/ldCore.h"
#include "ldCore/Data/ldFrameBuffer.h"
#include "ldCore/Render/ldRendererOpenlase.h"
#include "ldCore/Shape/ldShape.h"
#include "ldCore/Sound/ldAudioDecoder.h"
#include "ldCore/Sound/ldSoundAnalyzer.h"
#include "ldCore/Sound/ldSoundDeviceManager.h"
#include "ldCore/Task/ldTaskManager.h"
#include "ldCore/Visualizations/ldLogoLaserdock.h"
#include "ldCore/Visualizations/ldVisualizer.h"
#include "ldCore/Visualizations/MusicManager/ldMusicManager.h"

/*!
  \class ldVisualizationTask
  \brief This class is the viusaliztion task class for the music visualization app of the laserdock.
  \inmodule rendering

  The interface defined is extremely simple.
  \l ldAbstractTask::initialize() is used to initialize the task class to an known state right after being

*/

namespace  {
    static const float ROTATION_RANGE = M_PIf;
    static const float DAC_RATE = 30000.0f;
}

ldVisualizationTask::ldVisualizationTask(ldMusicManager *musicManager, ldAudioDecoder *audioDecoder, QObject *parent)
    : ldAbstractTask(parent)
    , m_musicManager(musicManager)
    , m_audioDecoder(audioDecoder)
{
    qDebug() << __FUNCTION__;

#ifdef LD_CORE_ENABLE_QT_QUICK
    qmlRegisterAnonymousType<ldVisualizer>("WickedLasers", 1);
#endif
    
    m_openlase = new ldRendererOpenlase(this);
    ldShape::setGlobalRenderer(m_openlase);

    qRegisterMetaType<AudioBlock>("AudioBlock");
}

ldRendererOpenlase* ldVisualizationTask::get_openlase()
{
    return m_openlase;
}
/*!
    \brief Workhorse of the class. This called by \l ldTaskManager when the task needs to get updated.

    This function calls the underlying visualizer's \l ldShape::visit() function, which recursively draws the graphics.
    Also calls backend openlase render to render the frame for this update after the draw functions were done.
*/

ldVisualizationTask::~ldVisualizationTask()
{
}


/*!
    \brief Render a new frame into buffer

    calls visualizer draw
    uses openlase->renderFrame to write to buffer.

*/

void ldVisualizationTask::update(quint64 delta, ldFrameBuffer * buffer)
{       
    QMutexLocker lock(&m_mutex);
    // check if running
    if (!m_sounddata) {qDebug() << "TEST FIX CHECK 1";return;}

    // prepare variables
    m_renderstate.buffer = buffer;
    m_renderstate.delta = delta;

    int max_fps = 30; // default value for maximum allowed FPS

    ldVisualizer* vis = getActiveVis();
    // get fps and draw visualizer
    // make sure visualizer exists
    if (vis) {
        max_fps = vis->targetFPS();
        m_openlase->loadIdentity();
        m_openlase->loadIdentity3();

        if(vis->is3d()) {
            m_openlase->rotate3X(m_rotY*ROTATION_RANGE);
            m_openlase->rotate3Y(m_rotX*ROTATION_RANGE);
            m_openlase->rotate3Z(m_rotZ*ROTATION_RANGE);
            m_openlase->pushMatrix3();
        }

        m_openlase->setFrameModes(0);

        vis->visit(); // draw vis with renderer

        if(vis->is3d())
            m_openlase->popMatrix3();
    }

    // perform openlase rendering on the frame
    if (this->renderState()->renderOpenlase) {
        m_openlase->renderFrame(buffer, max_fps, vis && vis->is3d());

        if(m_openlase->m_lastFramePointCount) {
            float result = DAC_RATE / m_openlase->m_lastFramePointCount;

            // perform fps averaging, and update current fps if changed
            m_fps_avg+=result;
            m_fps_cnt++;
            if (m_fps_cnt>=10){
                int fps = static_cast<int>( (m_fps_avg / m_fps_cnt) + 0.5f);
                m_fps_cnt = 0;
                m_fps_avg = 0;

                if (fps!=m_current_fps) {
                    m_current_fps = fps;
                    currentFpsChanged(m_current_fps);
                }
            }
        }
    }
	
	//qDebug() << buffer->getFill() << " - " << buffer->getCapacity();
}

void ldVisualizationTask::setSoundLevelGate(int value)
{
    m_soundLevelGate = value;
}

int ldVisualizationTask::soundLevelGate() const
{
    return m_soundLevelGate;
}

void ldVisualizationTask::setIsShowLogo(bool showLogo)
{
    m_isShowLogo = showLogo;
}

void ldVisualizationTask::setSoundSource(const ldVisualizationTask::SoundSource &source)
{
//    QMutexLocker lock(&m_mutex); // deadlock from ldTimelineVisualizer::onShouldStart
    m_soundSource = source;
}

void ldVisualizationTask::setVisualizer(ldVisualizer *visualizer, int priority)
{
    QMutexLocker lock(&m_mutex);

    // debug info
    QString visName = visualizer ? visualizer->visualizerName() : "null";
    qDebug().nospace() << __FUNCTION__ << " " << visName << priority;

    bool isReplaceCurrent = !m_visualizers.isEmpty() && (priority == m_visualizers.lastKey());
    bool isOverride = visualizer && (m_visualizers.isEmpty() || priority > m_visualizers.lastKey());
    bool isChange = isReplaceCurrent || isOverride;

    if(isChange) {
        // stop previous visualizer
        if(!m_visualizers.isEmpty())
            m_visualizers.last()->stop();

        // start new visualizer if it should be replaced
        if(visualizer)
            startVisualizer(visualizer);
    }

    // remember current visualizer
    if(visualizer)
        m_visualizers[priority] = visualizer;
    else
        m_visualizers.remove(priority);

    // if priority visualizer was disabled, activate the previous one
    if(isChange && !visualizer && priority > 0 && !m_visualizers.isEmpty())
        setVisualizer(m_visualizers.last(), m_visualizers.lastKey());

    if(isChange && priority == 0)
        emit baseVisualizerChanged(visualizer);
}

/*!
    \brief Handle recieving of a new block of audio data

    Gets called by signals and slots mechanism to have internal \l ldSoundData object updated with fresh music data.
    Also calls \l ldVisualizer::updateWith, which calls \l ldVisualizer::onUpdate();

*/

void ldVisualizationTask::onUpdateAudio(const AudioBlock &block)
{
    QMutexLocker lock(&m_mutex);

    // check if running
    if (!m_sounddata)
        return;

    if(m_soundSource == SoundSource::SoundInput) {
        // process block into raw sounddata struct
        m_sounddata->Update(block);

        m_musicManager->setRealSoundLevel(m_sounddata->GetSoundLevel());

        if(m_sounddata->GetSoundLevel() < m_soundLevelGate) {
            m_sounddata->Update(AudioBlock::EMPTY_AUDIO_BLOCK);
        }

        // update music manager
        m_musicManager->updateWith(m_sounddata, AUDIO_UPDATE_DELTA_S);
    }

    // update visualizer
    if (getActiveVis() && !m_audioDecoder->get_isActive()) getActiveVis()->updateWith(m_sounddata.get(), AUDIO_UPDATE_DELTA_S);
}

void ldVisualizationTask::onUpdateDecoderAudio(const AudioBlock &block)
{
    QMutexLocker lock(&m_mutex);

    // check if running
    if (!m_decoderSoundData)
        return;

    // process block into raw sounddata struct
    m_decoderSoundData->Update(block);

    if(m_soundSource == SoundSource::AudioDecoder) {
        m_musicManager->setRealSoundLevel(m_decoderSoundData->GetSoundLevel());

        // update music manager
        m_musicManager->updateWith(m_decoderSoundData, AUDIO_UPDATE_DELTA_S);
    }

    // update visualizer
    if (getActiveVis()) getActiveVis()->updateWith(m_decoderSoundData.get(), AUDIO_UPDATE_DELTA_S);
}

/*!
    \fn void ldVisualizationTask::start()
    \brief Initializes various components neccessary for the execution of this task.

    First it fetches \l ldSoundInferface form \l ldSoundmanager using \l soundManager()::getSoundInterface(ldSoundInputSource).
    Second it instantiates a \l ldSoundData for sound signal processing.
    Lastly, it setups signals and slots connection to have our task class notified of audio update.
*/

void ldVisualizationTask::start()
{
    QMutexLocker lock(&m_mutex);

    m_sounddata.reset(new ldSoundData());
    m_decoderSoundData.reset(new ldSoundData());

    ldSoundAnalyzer * soundAnalyzer = ldCore::instance()->soundDeviceManager()->analyzer();
    ldSoundAnalyzer * audioDecoderAnalyzer = m_audioDecoder->analyzer();
    connect(soundAnalyzer, &ldSoundAnalyzer::audioBlockUpdated, this, &ldVisualizationTask::onUpdateAudio);
    connect(audioDecoderAnalyzer, &ldSoundAnalyzer::audioBlockUpdated, this, &ldVisualizationTask::onUpdateDecoderAudio);

    if(!m_logo) m_logo.reset(new ldLogoLaserdock);
    // call updateWith at least once, to make sure visualizer gets m_sounddata pointer before any draw happens
    ldVisualizer* vis = getActiveVis();
    if(vis != nullptr) vis->updateWith(m_sounddata.get(), AUDIO_UPDATE_DELTA_S);
}


/*!
    \fn void ldVisualizationTask::stop()
    \brief Disposes of resources needed for this task and disconnect relevant connections.

    Does exact opposite of \l ldVisualizationTask::start();
*/

void ldVisualizationTask::stop()
{
    QMutexLocker lock(&m_mutex);
    ldSoundAnalyzer * soundAnalyzer = ldCore::instance()->soundDeviceManager()->analyzer();
    ldSoundAnalyzer * audioDecoderAnalyzer = m_audioDecoder->analyzer();
    disconnect(soundAnalyzer, nullptr, this, nullptr);
    disconnect(audioDecoderAnalyzer, nullptr, this, nullptr);

    m_sounddata.reset();
    m_decoderSoundData.reset();
}

ldVisualizationTask::RenderState *ldVisualizationTask::renderState()
{
    return &m_renderstate;
}

ldVisualizer *ldVisualizationTask::getBaseVisualizer() const
{
    QMutexLocker lock(&m_mutex);
    return m_visualizers.contains(0) ? m_visualizers[0] : nullptr;
}

void ldVisualizationTask::setRotX(float x)
{
    m_rotX = x;
}

void ldVisualizationTask::setRotY(float y)
{
    m_rotY = y;
}

void ldVisualizationTask::setRotZ(float z)
{
    m_rotZ = z;
}

ldVisualizer * ldVisualizationTask::getActiveVis() const
{
    ldVisualizer* vis = m_visualizers.isEmpty() ? nullptr : m_visualizers.last();

    if(m_isShowLogo && !m_logo->isFinished()) vis = m_logo.get();
    return vis;
}

void ldVisualizationTask::startVisualizer(ldVisualizer *v)
{
    if(!v)
        return;

    // init visualizer
    m_renderstate.renderOpenlase = true;
    v->start();
    // call updateWith at least once, to make sure visualizer gets m_sounddata pointer before any draw happens
    if (m_sounddata) v->updateWith(m_sounddata.get(), AUDIO_UPDATE_DELTA_S);
}
