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

#include "ldVisualizationTask.h"

#include <math.h>

#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtQml/QtQml>
#include <QtMultimedia/QAudioInput>

#include "ldCore/ldCore_global.h"
#include "ldCore/ldCore.h"
#include "ldCore/Data/ldFrameBuffer.h"
#include "ldCore/Filter/ldFilterManager.h"
#include "ldCore/Render/ldRendererOpenlase.h"
#include "ldCore/Shape/ldShape.h"
#include "ldCore/Sound/ldSoundDeviceManager.h"
#include "ldCore/Task/ldTaskManager.h"
#include "ldCore/Visualizations/MusicManager/ldMusicManager.h"

#include "ldLogoLaserdock.h"
#include "ldVisualizer.h"

namespace {
    ldRendererOpenlase * openlase;
}

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
    
    openlase = (ldRendererOpenlase *)ldCore::instance()->rendererManager()->getRenderer(OPENLASE);
    ldShape::setGlobalRenderer(openlase);

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

ldVisualizer * ldVisualizationTask::getActiveVis()
{
    ldVisualizer* vis = m_currentVisualizer;
    if (m_tempVisualizer != nullptr) vis = m_tempVisualizer;
    if(!m_logo->isFinished()) vis = m_logo.get();
    return vis;
}

void ldVisualizationTask::update(quint64 delta, ldFrameBuffer * buffer)
{       
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
        openlase->loadIdentity();
        openlase->loadIdentity3();
        openlase->setFrameModes(0);

        vis->visit(); // draw vis with renderer
    }

    // perform openlase rendering on the frame
    if (this->renderState()->renderOpenlase)
        openlase->renderFrame(buffer, fps);
	
	//qDebug() << buffer->getFill() << " - " << buffer->getCapacity();
}

void ldVisualizationTask::setTempVisualizer(ldVisualizer *visualizer)
{
    QString visName = visualizer ? visualizer->visualizerName() : "null";
    qDebug().nospace() << __FUNCTION__ << " \"" << visName << "\"";

    if(m_tempVisualizer) {
        m_tempVisualizer->stop();
    }

    m_tempVisualizer = visualizer;

    if(m_tempVisualizer) {
        m_tempVisualizer->init();
        m_tempVisualizer->prepare(this);
        m_tempVisualizer->start();
    } else {
        setCurrentVisualizer(m_currentVisualizer);
    }
}

void ldVisualizationTask::setCurrentVisualizer(ldVisualizer *visualizer)
{
    QString visName = visualizer ? visualizer->visualizerName() : "null";
    qDebug().nospace() << __FUNCTION__ << " " << visName;

    if(m_currentVisualizer) {
        m_currentVisualizer->stop();
    }

    // remember current visualizer
    m_currentVisualizer = visualizer;

    if(m_currentVisualizer) {
        // init visualizer
        m_currentVisualizer->init();
        m_currentVisualizer->prepare(this);
        m_currentVisualizer->start();
        // call updateWith at least once, to make sure visualizer gets m_sounddata pointer before any draw happens
        if (m_sounddata) m_currentVisualizer->updateWith(m_sounddata.data(), AUDIO_UPDATE_DELTA_S);
    }

    emit currentVisualizerChanged(visualizer);
}

/*!
    \brief Handle recieving of a new block of audio data

    Gets called by signals and slots mechanism to have internal \l ldSoundData object updated with fresh music data.
    Also calls \l ldVisualizer::updateWith, which calls \l ldVisualizer::onUpdate();

*/

void ldVisualizationTask::onUpdateAudio(const AudioBlock &block)
{
    // process a new block of data, send data to music manager, auto random manager, and current visualizer

    // check if running
    if (!m_sounddata) {qDebug() << "TEST FIX CHECK 2";return;}

    // process block into raw sounddata struct
    m_sounddata->Update(block);

    // update music manager
    ldCore::instance()->musicManager()->updateWith(m_sounddata.data(), AUDIO_UPDATE_DELTA_S);

    // update visualizer
    if (m_currentVisualizer) m_currentVisualizer->updateWith(m_sounddata.data(), AUDIO_UPDATE_DELTA_S);
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
//    qDebug()<<"ldVisualizationTask::start";
    ldSoundInterface * soundinterface = ldCore::instance()->soundDeviceManager();
    
    QAudioFormat format = soundinterface->getAudioFormat();
    m_sounddata.reset(new ldSoundData(format));
    connect(soundinterface, &ldSoundInterface::audioBlockUpdated, this, &ldVisualizationTask::onUpdateAudio);

    if(!m_logo) m_logo.reset(new LogoLaserdock);
    // call updateWith at least once, to make sure visualizer gets m_sounddata pointer before any draw happens
    ldVisualizer* vis = getActiveVis();
    if(vis != nullptr) vis->updateWith(m_sounddata.data(), AUDIO_UPDATE_DELTA_S);
}


/*!
    \fn void ldVisualizationTask::stop()
    \brief Disposes of resources needed for this task and disconnect relevant connections.

    Does exact opposite of \l ldVisualizationTask::start();
*/

void ldVisualizationTask::stop()
{
    ldSoundInterface * soundinterface = ldCore::instance()->soundDeviceManager();
    disconnect(soundinterface, 0, this, 0);
    
    m_sounddata.reset();
}

ldVisualizationTask::RenderState *ldVisualizationTask::renderState()
{
    return &m_renderstate;
}

ldVisualizer *ldVisualizationTask::getCurrentVisualizer() const
{
    return m_currentVisualizer;
}
