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

#include <math.h>

#include <QtGui/QOpenGLShaderProgram>

namespace  {
    const unsigned int default_size_for_vbuffer = 1000;
    const unsigned int history_sample_count = 1000;
}

ldSimulatorEngine::ldSimulatorEngine()
    : m_buffer(default_size_for_vbuffer)
{
    vbuffer.resize(default_size_for_vbuffer);

    m_last.clear();
    m_lastOn.clear();
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

bool ldSimulatorEngine::isActive() const
{
    return m_listenerCount > 0;
}

void ldSimulatorEngine::init()
{
    initializeOpenGLFunctions();

    // Generate 2 VBOs
    glGenBuffers(2, vboIds);
}

void ldSimulatorEngine::uninit()
{
    glDeleteBuffers(2, vboIds);
}

void ldSimulatorEngine::drawLaserGeometry(QOpenGLShaderProgram *program)
{
    if (m_lock.tryLockForRead()) {
        glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
        glBufferData(GL_ARRAY_BUFFER, history_sample_count * sizeof(Vertex), (const void*) &vbuffer[0], GL_DYNAMIC_DRAW);

        // Offset for position
        quintptr offset = 0;

        // Tell OpenGL programmable pipeline how to locate vertex position data
        int vertexLocation = program->attributeLocation("a_position");
        Q_ASSERT(vertexLocation >= 0);
        program->enableAttributeArray(vertexLocation);
        glVertexAttribPointer((GLuint) vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offset);

        offset += sizeof(float) * 3;

        int colorLocation = program->attributeLocation("a_color");
        Q_ASSERT(colorLocation >= 0);
        program->enableAttributeArray(colorLocation);
        glVertexAttribPointer((GLuint) colorLocation, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offset);

        // TODO draw points separately in release mode too
//        glDrawArrays(GL_POINTS, 0, history_sample_count);
        glDrawArrays(GL_LINE_STRIP, 0, history_sample_count);

        m_lock.unlock();
    }
}

void ldSimulatorEngine::pushVertexData(Vertex * data, unsigned int size) {

    // need lock first
    if (m_lock.tryLockForWrite()) {

        // create temp buffer for dots processing
        const int maxsize = 2048;
        Vertex data_processed[maxsize];
        if (size > maxsize) size = maxsize;

        // process dots
        bigger_dots(data, data_processed, size);

        // send processed data to the drawing engine
        m_buffer.Push(data_processed, size);
        m_buffer.Get(&vbuffer[0], history_sample_count);

        // done
        m_lock.unlock();
    }
}

// function for altering laser data to make points bigger before being sent to simulator
void ldSimulatorEngine::bigger_dots(Vertex* inData, Vertex* outData, unsigned int size) {

    const float mindist = 0.005f; // force lines to have this min length

    for (uint i = 0; i < size; i++) {

        outData[i] = inData[i]; // initial value

        // color logic
        bool ison = false;
        ison |= inData[i].color[0] != 0;
        ison |= inData[i].color[1] != 0;
        ison |= inData[i].color[2] != 0;

        // remember color of last nonblack point
        if (ison) m_lastOn = inData[i];

        // distance calcs
        if (ison) {
            float dx = (outData[i].x() - m_last.x());
            float dy = (outData[i].y() - m_last.y());
            float delta2 = dx*dx + dy*dy;
            float delta = sqrtf(delta2);
            m_moveDist += delta;
            if (delta >= mindist) {
                // get a vector of length mindist, pointed in last direction laser moved
                m_lastDeltaX = dx * mindist / delta;
                m_lastDeltaY = dy * mindist / delta;
            }
        }

        // check for end of a line, make changes to the points here
        if (m_wasOn && !ison) {

            // force last point to be a color
            //data2[i] = laston;
            outData[i].color[0] = m_lastOn.color[0];
            outData[i].color[1] = m_lastOn.color[1];
            outData[i].color[2] = m_lastOn.color[2];

            // force line length to be a minimum
            if (m_moveDist < mindist) {
                outData[i].x() += m_lastDeltaX;
                outData[i].y() += m_lastDeltaY;
            }
        }

        // check for start of a new line
        if (!m_wasOn && ison) {
            m_moveDist = 0;
        }

        // save static values for next point's delta calcs
        if(!inData[i].isBlank())
            m_last = inData[i];

        m_wasOn = ison;

    }
}

