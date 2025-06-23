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

#ifdef LD_CORE_USE_OPENGL
#if QT_VERSION >= 0x060000
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtQuick/QQuickWindow>
#else
#include <QtGui/QOpenGLShaderProgram>
#endif
#endif

#include <ldCore/Data/ldDataDispatcher.h>

#include "ldCore/Simulator/ldSimulatorEngine.h"

#ifdef LD_CORE_USE_OPENGL
namespace {
static bool isOpenGlVersionPrinted = false;
}
#endif

ldSimulatorRenderer::ldSimulatorRenderer(ldDataDispatcher *dataDispatcher, QObject *parent)
    : QObject(parent)
    , m_dataDispatcher(dataDispatcher)
    #ifdef LD_CORE_USE_OPENGL
    , m_program(0)
    #endif
#if QT_VERSION >= 0x060000
    , m_window(nullptr)
#endif
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
#ifdef LD_CORE_USE_OPENGL
    initializeOpenGLFunctions();

    if(!isOpenGlVersionPrinted) {
        const std::string vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        const std::string renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        const std::string version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        qDebug() << "OpenGL vendor: " << QString::fromStdString(vendor) << " "
                 << "renderer: " << QString::fromStdString(renderer) << " "
                 << "version: " << QString::fromStdString(version);
        isOpenGlVersionPrinted = true;
    }

    initShaders();

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);
#endif
}

void ldSimulatorRenderer::paint()
{
#ifdef LD_CORE_USE_OPENGL
#if QT_VERSION >= 0x060000
    // Play nice with the RHI. Not strictly needed when the scenegraph uses
    // OpenGL directly.
    m_window->beginExternalCommands();
#endif

    if(!m_program->bind()) {
        qWarning() << "can't bind";
        return;
    }

    glViewport(m_pos.x(), m_pos.y(), m_viewportSize.width(), m_viewportSize.height());

    // Clear color and depth buffer
    glClearColor(m_clearColor.redF(), m_clearColor.greenF(), m_clearColor.blueF(), m_clearColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_BLEND);

    // Draw laser geometry
    if(m_engine) m_engine->drawLaserGeometry(m_program);

    m_program->release();

#if QT_VERSION >= 0x060000
    m_window->endExternalCommands();
#endif
#endif
}

void ldSimulatorRenderer::resizeGL()
{
#ifdef LD_CORE_USE_OPENGL
    int w = m_viewportSize.width();
    int h =  m_viewportSize.height();

    // Set OpenGL viewport to cover whole widget
    glViewport(m_pos.x(), m_pos.y(), w, h);
#endif
}


void ldSimulatorRenderer::loadEngine()
{
    if(m_engine)
        return;

    m_engine = m_dataDispatcher->simulatorEngine();
    // NOTE: on mobile platforms we had crash because renderer is destroyed after engine
    connect(m_engine,
            &ldSimulatorEngine::destroyed,
            this,
            &ldSimulatorRenderer::disconnectEngine,
            static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::DirectConnection)
            );
    m_engine->addListener();
}

void ldSimulatorRenderer::unloadEngine()
{
    if(m_engine) {
        m_engine->removeListener();
        m_engine = nullptr;
    }
}

void ldSimulatorRenderer::disconnectEngine()
{
    m_engine = nullptr;
}

#ifdef LD_CORE_USE_OPENGL
void ldSimulatorRenderer::initShaders()
{
    m_program = new QOpenGLShaderProgram(this);

    if(QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::NoProfile) {
        if(!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":shaders/es2/shader.vert")) {
            qWarning() << "Can't addShaderFromSourceFile vshader" << m_program->log();
        }
        if(!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":shaders/es2/shader.frag")) {
            qWarning() << "Can't addShaderFromSourceFile fshader" << m_program->log();
        }
    } else {
        if(!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":shaders/gl3/shader.vert")) {
            qWarning() << "Can't addShaderFromSourceFile vshader" << m_program->log();
        }
        if(!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":shaders/gl3/shader.frag")) {
            qWarning() << "Can't addShaderFromSourceFile fshader" << m_program->log();
        }
    }

    if(!m_program->link()) {
        qWarning() << "Can't link" << m_program->log();
    }
}
#endif
