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

#include <QtQuick/QSGGeometry>

#include <ldCore/Data/ldFrameBuffer.h>
#include <ldCore/Simulator/ldSimulatorGrid.h>

#include "ldSimulatorProcessor.h"

#ifdef LD_CORE_USE_OPENGL
#if QT_VERSION >= 0x060000
#include <QtOpenGL/QOpenGLShaderProgram>
#else
#include <QtGui/QOpenGLShaderProgram>
#endif
#endif

ldSimulatorEngine::ldSimulatorEngine()
    : QObject()
    , m_buffer(ldFrameBuffer::FRAMEBUFFER_CAPACITY)
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
    if(!hasListeners())
        init();

    m_listenerCount++;
}

void ldSimulatorEngine::removeListener()
{
    m_listenerCount--;

    if(!hasListeners())
        uninit();
}

bool ldSimulatorEngine::hasListeners() const
{
    return m_listenerCount > 0;
}

void ldSimulatorEngine::init()
{
#ifdef LD_CORE_USE_OPENGL
    initializeOpenGLFunctions();

    glGenBuffers(1, vboIds);
#endif
}

void ldSimulatorEngine::uninit()
{
#ifdef LD_CORE_USE_OPENGL
    glDeleteBuffers(1, vboIds);
#endif
}

#ifdef LD_CORE_USE_OPENGL
void ldSimulatorEngine::drawLaserGeometry(QOpenGLShaderProgram *program)
{
    QMutexLocker lock(&m_mutex);
    drawBuffer(program, vbuffer,vbuffer_size);
    if(m_grid->isEnabled())
            drawBuffer(program, m_grid->buffer(), m_grid->buffer().size());

}

#endif

void ldSimulatorEngine::fillGeometry(QSGGeometry *geometry, const QSizeF &itemSize) const
{
    QMutexLocker lock(&m_mutex);
    fillGeometryFromVertex(geometry, itemSize, vbuffer,vbuffer_size);
    if(m_grid->isEnabled())
            fillGeometryFromVertex(geometry, itemSize, m_grid->buffer(), m_grid->buffer().size());
}

void ldSimulatorEngine::pushVertexData(ldVertex * data, unsigned int size, bool isLastPortion) {

    QMutexLocker lock(&m_mutex);

    // create temp buffer for dots processing
    const int maxsize = 2048;
    if (size > maxsize) size = maxsize;

    // process dots
    ldVertex data_processed[maxsize];
    m_processor->bigger_dots(data, data_processed, size, isLastPortion);
    data = data_processed;

    // buffer up partial frame points (until frame_complete() func is called).
    m_buffer.Push(data, size);
}

void ldSimulatorEngine::frame_complete()
{
    QMutexLocker lock(&m_mutex);

    m_processor->clear();

    vbuffer_size = m_buffer.GetLevel();

    if (vbuffer_size>0) {        
        m_buffer.Get(&vbuffer[0], vbuffer_size );
        m_buffer.Reset();
    }

    emit bufferUpdated();
}

ldSimulatorGrid *ldSimulatorEngine::grid() const
{
    return m_grid.get();
}

const std::vector<ldVertex> &ldSimulatorEngine::buffer() const
{
    return vbuffer;
}

uint ldSimulatorEngine::bufferSize() const
{
    return vbuffer_size;
}

#ifdef LD_CORE_USE_OPENGL
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
#endif

void ldSimulatorEngine::fillGeometryFromVertex(QSGGeometry *geometry, const QSizeF &itemSize, const std::vector<ldVertex> &buffer, unsigned int length) const
{
    Q_ASSERT(buffer.size() >= length);

    if(length == 0)
        return;

    geometry->allocate(length);

    QSGGeometry::ColoredPoint2D *vertices = geometry->vertexDataAsColoredPoint2D();
    for (size_t i = 0; i < length; ++i) {
        auto p = buffer[i];
        float x = p.x() * itemSize.width()/2 + itemSize.width()/2;
        float y = p.y() * itemSize.height()/2 * -1 + itemSize.height()/2;

        vertices[i].set(x, y, p.r()*255, p.g() * 255, p.b() * 255, p.a() * 255);
    }
}
