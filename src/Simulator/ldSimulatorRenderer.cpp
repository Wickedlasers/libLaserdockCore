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

#include "ldCore/Simulator/ldSimulatorRenderer.h"

#include <QtGui/QOpenGLShaderProgram>

#include <ldCore/Data/ldDataDispatcher.h>

#include "ldCore/Simulator/ldSimulatorEngine.h"

ldSimulatorRenderer::ldSimulatorRenderer(ldDataDispatcher *dataDispatcher, QObject *parent)
    : QObject(parent)
    , m_dataDispatcher(dataDispatcher)
    , m_program(0)
{
}

ldSimulatorRenderer::~ldSimulatorRenderer()
{
    if(m_engine) unloadEngine();
}

void ldSimulatorRenderer::setClearColor(const QColor &color)
{
    m_clearColor = color;
}

void ldSimulatorRenderer::init()
{
    initializeOpenGLFunctions();

    initShaders();

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);
}

void ldSimulatorRenderer::paint()
{
    if(!m_program->bind()) {
        qWarning() << "can't bind";
        return;
    }

    glViewport(m_pos.x(), m_pos.y(), m_viewportSize.width(), m_viewportSize.height());

    // Clear color and depth buffer
    glClearColor(m_clearColor.redF(), m_clearColor.greenF(), m_clearColor.blueF(), m_clearColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_BLEND);

    // Draw laser geometry
    if(m_engine) m_engine->drawLaserGeometry(m_program);

    m_program->release();
}

void ldSimulatorRenderer::resizeGL()
{
    int w = m_viewportSize.width();
    int h =  m_viewportSize.height();

    // Set OpenGL viewport to cover whole widget
    glViewport(m_pos.x(), m_pos.y(), w, h);
}


void ldSimulatorRenderer::loadEngine()
{
    m_engine = m_dataDispatcher->simulatorEngine();
    m_engine->init();
    m_engine->addListener();
}

void ldSimulatorRenderer::unloadEngine()
{
    if(m_engine) {
        m_engine->removeListener();
        m_engine = nullptr;
    }
    m_engine = nullptr;
}

void ldSimulatorRenderer::initShaders()
{
    m_program = new QOpenGLShaderProgram(this);

    if(!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":shaders/vshader.glsl")) {
        qWarning() << "Can't addShaderFromSourceFile vshader";
    }
    if(!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":shaders/fshader.glsl")) {
        qWarning() << "Can't addShaderFromSourceFile fshader";
    }

    if(!m_program->link()) {
        qWarning() << "Can't link";
    }
}

