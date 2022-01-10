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

#include <ldCore/Data/ldFrameBuffer.h>

ldSimulatorEngine::ldSimulatorEngine()
    : m_buffer(ldFrameBuffer::FRAMEBUFFER_CAPACITY)
    , m_processor(new ldSimulatorProcessor)
    , m_grid(new ldSimulatorGrid())
{
    vbuffer.resize(ldFrameBuffer::FRAMEBUFFER_CAPACITY);
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
    QMutexLocker lock(&m_mutex);
    drawBuffer(program, vbuffer,vbuffer_size);
    if(m_grid->isEnabled())
            drawBuffer(program, m_grid->buffer());
}

void ldSimulatorEngine::pushVertexData(ldVertex * data, unsigned int size) {

    QMutexLocker lock(&m_mutex);

    // create temp buffer for dots processing
    const int maxsize = 2048;
    ldVertex data_processed[maxsize];
    if (size > maxsize) size = maxsize;

    // process dots
    m_processor->bigger_dots(data, data_processed, size);


    // buffer up partial frame points (until frame_complete() func is called).
     m_buffer.Push(data_processed, size);
}

void ldSimulatorEngine::frame_complete()
{
    QMutexLocker lock(&m_mutex);

    vbuffer_size = m_buffer.GetLevel();

    if (vbuffer_size>0) {        
        m_buffer.Get(&vbuffer[0], vbuffer_size );
        m_buffer.Reset();
    }
}

ldSimulatorGrid *ldSimulatorEngine::grid() const
{
    return m_grid.get();
}

void ldSimulatorEngine::drawBuffer(QOpenGLShaderProgram *program, const std::vector<ldVertex> &buffer, unsigned length)
{    
    //if (length==0) length = buffer.size(); // if length is specified use it, otherwise use buffer size
    if (length==0) return; // should always have length, if not then dont draw.

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<qopengl_GLsizeiptr>(length * sizeof(ldVertex)),
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
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(length));
}
