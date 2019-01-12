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
    vbuffer = (Vertex *) calloc(default_size_for_vbuffer, sizeof(Vertex));
}

ldSimulatorEngine::~ldSimulatorEngine()
{
//    qDebug() << "~ldSimulatorEngine()";

//    glDeleteBuffers(2, vboIds);
    free(vbuffer);
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

void ldSimulatorEngine::drawLaserGeometry(QOpenGLShaderProgram *program)
{
    if (m_lock.tryLockForRead()) {
        glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
        glBufferData(GL_ARRAY_BUFFER, history_sample_count * sizeof(Vertex), (const void*) vbuffer, GL_DYNAMIC_DRAW);

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



// function for altering laser data to make points bigger before being sent to simulator

static void bigger_dots(Vertex* inData, Vertex* outData, unsigned int size) {

    const float mindist = 0.005f; // force lines to have this min length

    for (uint i = 0; i < size; i++) {

        outData[i] = inData[i]; // initial value

        // some data to remember
        static Vertex last;
        static Vertex laston;
        static float lastdeltax = 1;
        static float lastdeltay = 1;
        static bool wason = false;
        static float movedist = 0;

        // color logic
        bool ison = false;
        ison |= inData[i].color[0] != 0;
        ison |= inData[i].color[1] != 0;
        ison |= inData[i].color[2] != 0;

        // remember color of last nonblack point
        if (ison) laston = inData[i];

        // distance calcs
        if (ison) {
            float dx = (outData[i].position[0] - last.position[0]);
            float dy = (outData[i].position[1] - last.position[1]);
            float delta2 = dx*dx + dy*dy;
            float delta = sqrtf(delta2);
            movedist += delta;
            if (delta >= mindist) {
                // get a vector of length mindist, pointed in last direction laser moved
                lastdeltax = dx * mindist / delta;
                lastdeltay = dy * mindist / delta;
            }
        }

        // check for end of a line, make changes to the points here
        if (wason && !ison) {

            // force last point to be a color
            //data2[i] = laston;
            outData[i].color[0] = laston.color[0];
            outData[i].color[1] = laston.color[1];
            outData[i].color[2] = laston.color[2];

            // force line length to be a minimum
            if (movedist < mindist) {
                outData[i].position[0] += lastdeltax;
                outData[i].position[1] += lastdeltay;
            }
        }

        // check for start of a new line
        if (!wason && ison) {
            movedist = 0;
        }

        // save static values for next point's delta calcs
        last = inData[i];
        wason = ison;

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
        m_buffer.Get(vbuffer, history_sample_count);

        // done
        m_lock.unlock();
    }
}

