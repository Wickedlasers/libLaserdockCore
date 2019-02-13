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

#include <math.h>

#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtQml/QtQml>
#include <QtMultimedia/QAudioInput>

#include "ldCore/ldCore_global.h"
#include "ldCore/ldCore.h"
#include "ldCore/Data/ldFrameBuffer.h"
#include "ldCore/Render/ldRendererOpenlase.h"
#include "ldCore/Shape/ldShape.h"
#include "ldCore/Sound/ldAudioDecoder.h"
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

ldVisualizationTask::ldVisualizationTask(QObject *parent)
: ldAbstractTask(parent) ,
  m_currentVisualizer(NULL) ,
  m_tempVisualizer(NULL)
{
    qDebug() << __FUNCTION__;

#ifdef LD_CORE_ENABLE_QT_QUICK
    qmlRegisterType<ldVisualizer>();
#endif
    
    m_openlase = (ldRendererOpenlase *)ldCore::instance()->rendererManager()->getRenderer(OPENLASE);
    ldShape::setGlobalRenderer(m_openlase);

    qRegisterMetaType<AudioBlock>("AudioBlock");
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

    float fps = 30; // default value

    ldVisualizer* vis = getActiveVis();
    // get fps and draw visualizer
    // make sure visualizer exists
    if (vis) {
        fps = vis->targetFPS();
        m_openlase->loadIdentity();
        m_openlase->loadIdentity3();
        m_openlase->setFrameModes(0);

        vis->visit(); // draw vis with renderer
    }

    // perform openlase rendering on the frame
    if (this->renderState()->renderOpenlase)
        m_openlase->renderFrame(buffer, fps);
	
	//qDebug() << buffer->getFill() << " - " << buffer->getCapacity();
}

void ldVisualizationTask::setTempVisualizer(ldVisualizer *visualizer)
{
    QMutexLocker lock(&m_mutex);
    QString visName = visualizer ? visualizer->visualizerName() : "null";
    qDebug().nospace() << __FUNCTION__ << " \"" << visName << "\"";

    if(m_tempVisualizer) {
        m_tempVisualizer->stop();
    }

    m_tempVisualizer = visualizer;

    if(visualizer) {
        startVisualizer(visualizer);
    } else {
        lock.unlock();
        setCurrentVisualizer(m_currentVisualizer);
    }
}

void ldVisualizationTask::setSoundLevelGate(int value)
{
    m_soundLevelGate = value;
}

int ldVisualizationTask::soundLevelGate() const
{
    return m_soundLevelGate;
}

void ldVisualizationTask::setCurrentVisualizer(ldVisualizer *visualizer)
{
    QMutexLocker lock(&m_mutex);
    QString visName = visualizer ? visualizer->visualizerName() : "null";
    qDebug().nospace() << __FUNCTION__ << " " << visName;

    if(m_currentVisualizer) {
        m_currentVisualizer->stop();
    }

    if(visualizer) {
        startVisualizer(visualizer);
    }

    // remember current visualizer
    m_currentVisualizer = visualizer;

    emit currentVisualizerChanged(visualizer);
}

/*!
    \brief Handle recieving of a new block of audio data

    Gets called by signals and slots mechanism to have internal \l ldSoundData object updated with fresh music data.
    Also calls \l ldVisualizer::updateWith, which calls \l ldVisualizer::onUpdate();

*/

void ldVisualizationTask::onUpdateAudio(const AudioBlock &block)
{
    QMutexLocker lock(&m_mutex);
    // process a new block of data, send data to music manager, auto random manager, and current visualizer

    // check if running
    if (!m_sounddata) {qDebug() << "TEST FIX CHECK 2";return;}

    // process block into raw sounddata struct
    m_sounddata->Update(block);

    ldCore::instance()->musicManager()->setRealSoundLevel(m_sounddata->GetSoundLevel());

    if(m_sounddata->GetSoundLevel() < m_soundLevelGate) {
        m_sounddata->Update(AudioBlock::EMPTY_AUDIO_BLOCK);
    }

    // update music manager
    ldCore::instance()->musicManager()->updateWith(m_sounddata, AUDIO_UPDATE_DELTA_S);

    // update visualizer
    if (m_currentVisualizer && !ldCore::instance()->audioDecoder()->get_isActive()) m_currentVisualizer->updateWith(m_sounddata.get(), AUDIO_UPDATE_DELTA_S);
}

void ldVisualizationTask::onUpdateDecoderAudio(const AudioBlock &block)
{
    QMutexLocker lock(&m_mutex);
    // process a new block of data, send data to music manager, auto random manager, and current visualizer

    // check if running
    if (!m_decoderSoundData) {qDebug() << "TEST FIX CHECK 3";return;}

    // process block into raw sounddata struct
    m_decoderSoundData->Update(block);

    if(!ldCore::instance()->soundDeviceManager()->getDeviceInfo().isValid()) {
        ldCore::instance()->musicManager()->setRealSoundLevel(m_decoderSoundData->GetSoundLevel());

        // update music manager
        ldCore::instance()->musicManager()->updateWith(m_decoderSoundData, AUDIO_UPDATE_DELTA_S);
    }

    // update visualizer
    if (m_currentVisualizer) m_currentVisualizer->updateWith(m_decoderSoundData.get(), AUDIO_UPDATE_DELTA_S);
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

    ldSoundInterface * soundDeviceInterface = ldCore::instance()->soundDeviceManager();
    ldSoundInterface * audioDecoderInterface = ldCore::instance()->audioDecoder();

    m_sounddata.reset(new ldSoundData(soundDeviceInterface->getAudioFormat()));
    m_decoderSoundData.reset(new ldSoundData(audioDecoderInterface->getAudioFormat()));
    connect(soundDeviceInterface, &ldSoundInterface::audioBlockUpdated, this, &ldVisualizationTask::onUpdateAudio);
    connect(audioDecoderInterface, &ldSoundInterface::audioBlockUpdated, this, &ldVisualizationTask::onUpdateDecoderAudio);

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
    ldSoundInterface * soundinterface = ldCore::instance()->soundDeviceManager();
    disconnect(soundinterface, 0, this, 0);
    
    m_sounddata.reset();
    m_decoderSoundData.reset();
}

ldVisualizationTask::RenderState *ldVisualizationTask::renderState()
{
    return &m_renderstate;
}

ldVisualizer *ldVisualizationTask::getCurrentVisualizer() const
{
    QMutexLocker lock(&m_mutex);
    return m_currentVisualizer;
}

ldVisualizer * ldVisualizationTask::getActiveVis() const
{
    ldVisualizer* vis = m_currentVisualizer;
    if (m_tempVisualizer != nullptr) vis = m_tempVisualizer;

#ifdef LD_CORE_SHOW_LOGO
    if(!m_logo->isFinished()) vis = m_logo.get();
#endif
    return vis;
}

void ldVisualizationTask::startVisualizer(ldVisualizer *v)
{
    if(!v)
        return;

    // init visualizer
    v->init();
    m_renderstate.renderOpenlase = true;
    v->start();
    // call updateWith at least once, to make sure visualizer gets m_sounddata pointer before any draw happens
    if (m_sounddata) v->updateWith(m_sounddata.get(), AUDIO_UPDATE_DELTA_S);
}
