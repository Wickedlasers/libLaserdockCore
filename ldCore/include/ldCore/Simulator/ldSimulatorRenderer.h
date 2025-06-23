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

#ifndef LDSIMULATORRENDERER_H
#define LDSIMULATORRENDERER_H

#include <QtCore/QPointF>
#include <QtCore/QSize>
#include <QtGui/QColor>
#include <QtGui/QMatrix4x4>
#ifdef LD_CORE_USE_OPENGL
#include <QtGui/QOpenGLFunctions>
#endif
#include <QtGui/QQuaternion>

#include "ldCore/ldCore_global.h"

#ifdef LD_CORE_USE_OPENGL
class QOpenGLShaderProgram;
#endif
#if QT_VERSION >= 0x060000
class QQuickWindow;
#endif

class ldDataDispatcher;
class ldSimulatorEngine;

class LDCORESHARED_EXPORT ldSimulatorRenderer : public QObject
        #ifdef LD_CORE_USE_OPENGL
        , protected QOpenGLFunctions
        #endif
{
    Q_OBJECT
public:
    explicit ldSimulatorRenderer(ldDataDispatcher *dataDispatcher, QObject *parent = nullptr);
    ~ldSimulatorRenderer();

    void setViewportPos(const QPoint &pos) { m_pos = pos; }
    void setViewportSize(const QSize &size) { m_viewportSize = size; }
#if QT_VERSION >= 0x060000
    void setWindow(QQuickWindow *window) { m_window = window; }
#endif

public slots:
    void setClearColor(const QColor &color);

    void init();
    void paint();
    void resizeGL();

    void loadEngine();
    void unloadEngine();

private:
    void disconnectEngine();

#ifdef LD_CORE_USE_OPENGL
    void initShaders();
#endif

    ldDataDispatcher *m_dataDispatcher;

    QColor m_clearColor = QColor(Qt::black);
    QPoint m_pos;
    QSize m_viewportSize;
#ifdef LD_CORE_USE_OPENGL
    QOpenGLShaderProgram *m_program;
#endif
#if QT_VERSION >= 0x060000
    QQuickWindow *m_window;
#endif

    ldSimulatorEngine *m_engine = nullptr;
};


#endif // LDSIMULATORRENDERER_H


