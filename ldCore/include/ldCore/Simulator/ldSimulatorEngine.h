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

#ifndef LDSIMULATORENGINE_H
#define LDSIMULATORENGINE_H

#include <memory>

#include <QtCore/QReadWriteLock>
#include <QtGui/QOpenGLFunctions>

#include <ldCore/ldCore_global.h>
#include <ldCore/Utilities/ldVertex.h>
#include <ldCore/Utilities/ldCircularBuffer.h>

class QOpenGLShaderProgram;

class ldSimulatorGrid;
class ldSimulatorProcessor;

/** Simulator engine class */
class LDCORESHARED_EXPORT ldSimulatorEngine : protected QOpenGLFunctions
{
public:
    /** Constructor/destructor */
    explicit ldSimulatorEngine();
    virtual ~ldSimulatorEngine();

    /** Engine can have multiple surfaces to draw. It is active if number of listeners > 0 */
    void addListener();
    void removeListener();

    /** If engine is active*/
    bool hasListeners() const;

    /** OpenGL initialization */
    void init();

    /** OpenGL uninitialization */
    void uninit();

    /** Draw simulator data on surface */
    void drawLaserGeometry(QOpenGLShaderProgram *program);

    /** Add simulator data */
    void pushVertexData(ldVertex * data, unsigned int size);
    /** inform simulator frame has completed */
    void frame_complete();

    /** Simulator background grid */
    ldSimulatorGrid *grid() const;

private:
    void drawBuffer(QOpenGLShaderProgram *program, const std::vector<ldVertex> &buffer, unsigned length = 0);

    GLuint vboIds[2] = {};

    ldVertexCircularBuffer m_buffer;
    QReadWriteLock m_lock;
    std::vector<ldVertex> vbuffer;
    unsigned vbuffer_size{0};
    int m_listenerCount{0};

    std::unique_ptr<ldSimulatorProcessor> m_processor;

    std::unique_ptr<ldSimulatorGrid> m_grid;
};

Q_DECLARE_METATYPE(ldSimulatorEngine*)

#endif // LDSIMULATORENGINE_H


