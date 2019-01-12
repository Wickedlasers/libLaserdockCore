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
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QQuaternion>

#include "ldCore/ldCore_global.h"
#include "ldCore/Utilities/ldBasicDataStructures.h"

class QOpenGLShaderProgram;

class ldDataDispatcher;
class ldSimulatorEngine;

class LDCORESHARED_EXPORT ldSimulatorRenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit ldSimulatorRenderer(ldDataDispatcher *dataDispatcher, QObject *parent = nullptr);
    ~ldSimulatorRenderer();

    void setViewportPos(const QPoint &pos) { m_pos = pos; }
    void setViewportSize(const QSize &size) { m_viewportSize = size; }

public slots:
    void setClearColor(const QColor &color);

    void init();
    void paint();
    void resizeGL();

    void loadEngine();
    void unloadEngine();

private:
    void initShaders();

    ldDataDispatcher *m_dataDispatcher;

    QColor m_clearColor = QColor(Qt::black);
    QPoint m_pos;
    QSize m_viewportSize;
    QOpenGLShaderProgram *m_program;

    ldSimulatorEngine *m_engine = nullptr;
    QQuaternion rotation;
};


#endif // LDSIMULATORRENDERER_H


