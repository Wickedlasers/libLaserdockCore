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

#include "ldCore/Simulator/ldSimulatorEngine.h"

#include <cmath>

#include <QtGui/QOpenGLShaderProgram>

#include <ldCore/Simulator/ldSimulatorGrid.h>

#include "ldSimulatorProcessor.h"

namespace  {
    const unsigned int DEFAULT_SIZE_FOR_VBUFFER = 1000;
    const unsigned int HISTORY_SAMPLE_COUNT = 1000;
}

ldSimulatorEngine::ldSimulatorEngine()
    : m_buffer(DEFAULT_SIZE_FOR_VBUFFER)
    , m_processor(new ldSimulatorProcessor)
    , m_grid(new ldSimulatorGrid())
{
    vbuffer.resize(DEFAULT_SIZE_FOR_VBUFFER);
}

ldSimulatorEngine::~ldSimulatorEngine()
{
//    qDebug() << "~ldSimulatorEngine()";
}

void ldSimulatorEngine::addListener()
{
    m_listenerCount++;
}

void ldSimulatorEngine::removeListener()
{
    m_listenerCount--;
}

bool ldSimulatorEngine::hasListeners() const
{
    return m_listenerCount > 0;
}

void ldSimulatorEngine::init()
{
    initializeOpenGLFunctions();

    glGenBuffers(1, vboIds);
}

void ldSimulatorEngine::uninit()
{
    glDeleteBuffers(1, vboIds);
}

void ldSimulatorEngine::drawLaserGeometry(QOpenGLShaderProgram *program)
{
    if (!m_lock.tryLockForRead())
        return;

    drawBuffer(program, vbuffer);
    if(m_grid->isEnabled())
        drawBuffer(program, m_grid->buffer());

    m_lock.unlock();
}

void ldSimulatorEngine::pushVertexData(ldVertex * data, unsigned int size) {

    // need lock first
    if (!m_lock.tryLockForWrite())
        return;

    // create temp buffer for dots processing
    const int maxsize = 2048;
    ldVertex data_processed[maxsize];
    if (size > maxsize) size = maxsize;

    // process dots
    m_processor->bigger_dots(data, data_processed, size);

    // send processed data to the drawing engine
    m_buffer.Push(data_processed, size);
    m_buffer.Get(&vbuffer[0], HISTORY_SAMPLE_COUNT);

    // done
    m_lock.unlock();
}

ldSimulatorGrid *ldSimulatorEngine::grid() const
{
    return m_grid.get();
}

void ldSimulatorEngine::drawBuffer(QOpenGLShaderProgram *program, const std::vector<ldVertex> &buffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<qopengl_GLsizeiptr>(buffer.size() * sizeof(ldVertex)),
                 &buffer[0],
                 GL_DYNAMIC_DRAW);

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    Q_ASSERT(vertexLocation >= 0);
    program->enableAttributeArray(vertexLocation);
    glVertexAttribPointer(static_cast<GLuint>(vertexLocation),
                          ldVertex::POS_COUNT,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(ldVertex),
                          reinterpret_cast<const void *>(offset));

    offset += sizeof(float) * ldVertex::POS_COUNT;

    int colorLocation = program->attributeLocation("a_color");
    Q_ASSERT(colorLocation >= 0);
    program->enableAttributeArray(colorLocation);
    glVertexAttribPointer(static_cast<GLuint>(colorLocation),
                          ldVertex::COLOR_COUNT,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(ldVertex),
                          reinterpret_cast<const void *>(offset));

    // Draw
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(buffer.size()));
}
