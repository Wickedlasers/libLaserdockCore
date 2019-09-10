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

#include "ldCore/Simulator/ldSimulatorItem.h"


#include <math.h>
#include <locale.h>

#include <QtCore/QTimer>
#include <QtGui/QMouseEvent>
#include <QtQuick/QQuickWindow>

#include <ldCore/ldCore.h>
#include <ldCore/Data/ldDataDispatcher.h>
#include <ldCore/Simulator/ldSimulatorRenderer.h>

void ldSimulatorItem::registerMetatypes()
{
    qmlRegisterType<ldSimulatorItem>("WickedLasers", 1, 0, "LdSimulatorItem");
}

ldSimulatorItem::ldSimulatorItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_autostart(false)
    , m_clearColor(QColor("#131823"))
    , m_isActive(false)
{
    connect(this, &QQuickItem::windowChanged, this, &ldSimulatorItem::handleWindowChanged);
}

ldSimulatorItem::~ldSimulatorItem()
{
}

void ldSimulatorItem::start()
{
    if(m_isActive)
        return;

    update_isActive(true);

//    qDebug() << "Simulator started" << this;

    if(m_renderer) {
        connect(window(), &QQuickWindow::beforeRendering, this, &ldSimulatorItem::paint, Qt::DirectConnection);
        // delay in order to skip previous visualizer
        QTimer::singleShot(100, m_renderer.data(), &ldSimulatorRenderer::loadEngine);
    }
}

void ldSimulatorItem::stop()
{
    if(!m_isActive)
        return;

    update_isActive(false);

//    qDebug() << "Simulator stopped" << this;

    disconnect(window(), &QQuickWindow::beforeRendering, this, &ldSimulatorItem::paint);
    if(m_renderer) {
        m_renderer->unloadEngine();
    }
}

void ldSimulatorItem::paint()
{
    if(!m_renderer) {
        qDebug() << "NICE CRASH IN PAINT";
        return;
    }

    m_renderer->paint();

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
//    window()->resetOpenGLState();

    window()->update();
}

void ldSimulatorItem::sync()
{
    // sometimes init is not called (on Linux if extraction is done)
    // it is safe to call it from sync() as well
    if(!m_renderer) {
        init();
    }

    QPointF pos = mapToScene(position());
    // OpenGL coordinates are started from bottom, not from top
    pos.ry() = window()->height() - pos.y() - height();

    m_renderer->setViewportPos((pos * m_devicePixelRatio).toPoint());
    m_renderer->setViewportSize((QSizeF(width(), height()) * m_devicePixelRatio).toSize());
    m_renderer->resizeGL();
}

void ldSimulatorItem::init()
{
    if (!m_renderer) {
        m_renderer.reset(new ldSimulatorRenderer(ldCore::instance()->dataDispatcher()));
        m_renderer->init();
        m_renderer->setClearColor(m_clearColor);

        if(m_isActive) {
            connect(window(), &QQuickWindow::beforeRendering, this, &ldSimulatorItem::paint, Qt::DirectConnection);
            // delay in order to skip previous visualizer
            QTimer::singleShot(100, m_renderer.data(), &ldSimulatorRenderer::loadEngine);
        }
        if(m_autostart) {
            start();
        }
    }
}

void ldSimulatorItem::cleanup()
{
    m_renderer.reset();
}

void ldSimulatorItem::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        m_devicePixelRatio = win->devicePixelRatio(); // warning in mac cocoa that should be used from main thread only
        
        connect(win, &QQuickWindow::beforeSynchronizing, this, &ldSimulatorItem::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInitialized, this, &ldSimulatorItem::init, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &ldSimulatorItem::cleanup, Qt::DirectConnection);

        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
        win->setClearBeforeRendering(false);
    }
}


