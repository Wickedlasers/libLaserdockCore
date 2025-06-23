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


#include <cmath>
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
    , m_clearColor(Qt::black)
    , m_isActive(false)
    , m_isActiveRendering(true)
{
    connect(this, &ldSimulatorItem::isActiveChanged, this, &ldSimulatorItem::onIsActiveChanged);
    connect(this, &QQuickItem::windowChanged, this, &ldSimulatorItem::handleWindowChanged);
    connect(this, &ldSimulatorItem::isActiveRenderingChanged, this, &ldSimulatorItem::onActiveRenderingChanged);
}

ldSimulatorItem::~ldSimulatorItem()
{
}

void ldSimulatorItem::start()
{
//    qDebug() << "Simulator started" << this;

    if(m_renderer) {
        if(m_isActiveRendering)
            activateRendering();
        // delay in order to skip previous visualizer
        QTimer::singleShot(100, m_renderer.data(), &ldSimulatorRenderer::loadEngine);
    }
}

void ldSimulatorItem::stop()
{
//    qDebug() << "Simulator stopped" << this;

    deactivateRendering();
    if(m_renderer)
        m_renderer->unloadEngine();

}

#if QT_VERSION < 0x060000
void ldSimulatorItem::update()
{
    window()->update();
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
    // window()->resetOpenGLState();

   // window()->update();
}
#endif

void ldSimulatorItem::sync()
{
    // sometimes init is not called (on Linux if extraction is done)
    // it is safe to call it from sync() as well
    if(!m_renderer) {
#if QT_VERSION < 0x060000
        init();
    }

    if(!m_isActive)
        return;

    setGlViewport();
}

void ldSimulatorItem::init()
{
    if (!m_renderer) {
#endif
        m_renderer.reset(new ldSimulatorRenderer(ldCore::instance()->get_dataDispatcher()));
        m_renderer->init();
        m_renderer->setClearColor(m_clearColor);

        if(m_isActive) {
            if(m_isActiveRendering)
                activateRendering();
            // delay in order to skip previous visualizer
            QTimer::singleShot(100, m_renderer.data(), &ldSimulatorRenderer::loadEngine);
        }
    }

#if QT_VERSION >= 0x060000
    if(!m_isActive)
        return;

    setGlViewport();
    m_renderer->setWindow(window());
#endif
}

void ldSimulatorItem::cleanup()
{
    m_renderer.reset();
}

void ldSimulatorItem::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
#if QT_VERSION >= 0x060000
        qDebug() << "graphicsApi" << win->graphicsApi();
#endif
        connect(win, &QQuickWindow::beforeSynchronizing, this, &ldSimulatorItem::sync, Qt::DirectConnection);
#if QT_VERSION < 0x060000
        connect(win, &QQuickWindow::sceneGraphInitialized, this, &ldSimulatorItem::init, Qt::DirectConnection);
#endif
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &ldSimulatorItem::cleanup, Qt::DirectConnection);

#if QT_VERSION >= 0x060000
        win->setColor(Qt::black);
#else
        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
        win->setClearBeforeRendering(false);
#endif
    }
}

void ldSimulatorItem::onActiveRenderingChanged(bool isActiveRendering)
{
    if(isActiveRendering)
        activateRendering();
    else
        deactivateRendering();
}

void ldSimulatorItem::onIsActiveChanged(bool isActive)
{
    // qDebug() << this << __FUNCTION__ << isActive;

    if(isActive)
        start();
    else
        stop();
}

void ldSimulatorItem::activateRendering()
{
    if(!m_renderer)
        return;

#if QT_VERSION >= 0x060000
    connect(window(), &QQuickWindow::beforeRenderPassRecording, m_renderer.get(), &ldSimulatorRenderer::paint, Qt::DirectConnection);
    connect(window(), &QQuickWindow::beforeRenderPassRecording, window(), &QQuickWindow::update, Qt::QueuedConnection);
#else
    connect(window(), &QQuickWindow::beforeRendering, this, &ldSimulatorItem::paint, Qt::DirectConnection);
    connect(window(), &QQuickWindow::beforeRendering, this, &ldSimulatorItem::update, Qt::QueuedConnection);
#endif
}

void ldSimulatorItem::deactivateRendering()
{
    if(!m_renderer)
        return;

#if QT_VERSION < 0x060000
    disconnect(window(), &QQuickWindow::beforeRendering, this, &ldSimulatorItem::paint);
    disconnect(window(), &QQuickWindow::beforeRendering, this, &ldSimulatorItem::update);
#endif

#if QT_VERSION >= 0x060000
    disconnect(window(), &QQuickWindow::beforeRenderPassRecording, m_renderer.get(), &ldSimulatorRenderer::paint);
    disconnect(window(), &QQuickWindow::beforeRenderPassRecording, window(), &QQuickWindow::update);
#endif
}

void ldSimulatorItem::setGlViewport()
{
    QPointF pos = mapToScene(position());
    // OpenGL coordinates are started from bottom, not from top
    pos.ry() = window()->height() - pos.y() - height();

    qreal devicePixelRatio = window()->devicePixelRatio();
    m_renderer->setViewportPos((pos * devicePixelRatio).toPoint());
    m_renderer->setViewportSize((QSizeF(width(), height()) * devicePixelRatio).toSize());
    m_renderer->resizeGL();
}


