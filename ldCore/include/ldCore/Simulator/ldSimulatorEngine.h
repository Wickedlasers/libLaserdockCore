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
#include <QMutexLocker>
#include <QtCore/QReadWriteLock>

#ifdef LD_CORE_USE_OPENGL
#include <QtGui/QOpenGLFunctions>
#endif

#include <ldCore/ldCore_global.h>
#include <ldCore/Utilities/ldVertex.h>
#include <ldCore/Utilities/ldCircularBuffer.h>

class QOpenGLShaderProgram;

class ldSimulatorGrid;
class ldSimulatorProcessor;

class QSGGeometry;

/** Simulator engine class */
class LDCORESHARED_EXPORT ldSimulatorEngine
        : public QObject
        #ifdef LD_CORE_USE_OPENGL
        , protected QOpenGLFunctions
        #endif
{
    Q_OBJECT
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

#ifdef LD_CORE_USE_OPENGL
    /** Draw simulator data on surface */
    void drawLaserGeometry(QOpenGLShaderProgram *program);
#endif

    /** Add simulator data */
    void pushVertexData(ldVertex * data, unsigned int size, bool isLastPortion);
    /** inform simulator frame has completed */
    void frame_complete();

    /** Simulator background grid */
    ldSimulatorGrid *grid() const;

    const std::vector<ldVertex> &buffer() const;
    uint bufferSize() const;

    void set3dMode(bool is3dMode);

    int currentFps() const;
    void setCurrentFps(int fps);

signals:
    void bufferUpdated();

private:
#ifdef LD_CORE_USE_OPENGL
    void drawBuffer(QOpenGLShaderProgram *program, const std::vector<ldVertex> &buffer, unsigned length);
    GLuint vboIds[2] = {};
#endif

    void fillGeometryFromVertex(QSGGeometry *geometry, const QSizeF &itemSize, const std::vector<ldVertex> &buffer, unsigned length) const;

    ldVertexCircularBuffer m_buffer;
    mutable QMutex m_mutex;
    std::vector<ldVertex> vbuffer;
    unsigned vbuffer_size{0};
    int m_listenerCount{0};

    std::unique_ptr<ldSimulatorProcessor> m_processor;

    std::unique_ptr<ldSimulatorGrid> m_grid;

    bool m_is3dMode{false};

    int m_currentFps = -1;
};

Q_DECLARE_METATYPE(ldSimulatorEngine*)

#endif // LDSIMULATORENGINE_H


