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

#include "ldCore/Simulator/ldQSGSimulatorItem.h"

#include <QtCore/QLoggingCategory>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGVertexColorMaterial>

#include <ldCore/ldCore.h>
#include <ldCore/Data/ldDataDispatcher.h>
#include <ldCore/Data/ldDataDispatcherManager.h>
#include <ldCore/Simulator/ldSimulatorEngine.h>
#include <ldCore/Settings/ldSettings.h>

namespace  {
    Q_LOGGING_CATEGORY(sim, "ld.simulator")
}

void ldQSGSimulatorItem::registerMetatypes()
{
    qmlRegisterType<ldQSGSimulatorItem>("WickedLasers", 1, 0, "LdQSGSimulatorItem");
}

ldQSGSimulatorItem::ldQSGSimulatorItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_isActive(false)
    , m_simulatorIndex(0)
    , m_isWindow(false)
    , m_windowX(0)
    , m_windowY(0)
    , m_windowW(640)
    , m_windowH(640)
{
    setFlag(ItemHasContents, true);

    connect(this, &ldQSGSimulatorItem::isActiveChanged, this, &ldQSGSimulatorItem::onIsActiveChanged);
    connect(this, &QQuickItem::visibleChanged, this, &ldQSGSimulatorItem::onVisibleChanged);
    connect(this, &QQuickItem::windowChanged, this, &ldQSGSimulatorItem::handleWindowChanged);

//    qDebug() << "ldQSGSimulatorItem" << m_simulatorIndex ;

    connect(this, &ldQSGSimulatorItem::isWindowChanged, this, &ldQSGSimulatorItem::updateSimulatorWindowGeometry);
    connect(this, &ldQSGSimulatorItem::simulatorIndexChanged, this, &ldQSGSimulatorItem::updateSimulatorWindowGeometry);

    qCDebug(sim) << __FUNCTION__ << isVisible() << m_isActive;
}

ldQSGSimulatorItem::~ldQSGSimulatorItem()
{
//    qDebug() << "~ldQSGSimulatorItem" << m_simulatorIndex ;

    if(m_isWindow) {
//        qDebug() << m_windowX << m_windowY << m_windowW << m_windowH;

        ldSettings()->setValue(QString("simulator_%1/windowX").arg(m_simulatorIndex), m_windowX);
        ldSettings()->setValue(QString("simulator_%1/windowY").arg(m_simulatorIndex), m_windowY);
        ldSettings()->setValue(QString("simulator_%1/windowW").arg(m_simulatorIndex), m_windowW);
        ldSettings()->setValue(QString("simulator_%1/windowH").arg(m_simulatorIndex), m_windowH);
    }
}

void ldQSGSimulatorItem::start()
{
    qCDebug(sim) << this << __FUNCTION__ << window() << isVisible();

    loadEngine();

    if(isVisible())
        startRendering();
}

void ldQSGSimulatorItem::stop()
{
    qCDebug(sim) << this << __FUNCTION__;

    unloadEngine();

    if(!isVisible())
        stopRendering();
}

void ldQSGSimulatorItem::loadEngine()
{
    if(m_engine)
        return;

    ldDataDispatcherInstance *dd = ldCore::instance()->get_dataDispatcherManager()->get(m_simulatorIndex);
    if(dd) {
        m_engine = dd->dataDispatcher->simulatorEngine();
        connect(m_engine,
                &ldSimulatorEngine::destroyed,
                this,
                &ldQSGSimulatorItem::disconnectEngine,
                static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::DirectConnection)
                );
        m_engine->addListener();
    }
}

void ldQSGSimulatorItem::unloadEngine()
{
    if(!m_engine)
        return;

    m_engine->removeListener();
    m_engine = nullptr;
}

void ldQSGSimulatorItem::disconnectEngine()
{
    m_engine = nullptr;
}

void ldQSGSimulatorItem::startRendering()
{
    if(!window())
        return;

    qCDebug(sim) << this << __FUNCTION__;

    connect(window(), &QQuickWindow::beforeRendering, this, &ldQSGSimulatorItem::update, static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
}

void ldQSGSimulatorItem::stopRendering()
{
    if(!window())
        return;

    qCDebug(sim) << this << __FUNCTION__;
    disconnect(window(), &QQuickWindow::beforeRendering, this, &ldQSGSimulatorItem::update);
}

void ldQSGSimulatorItem::onIsActiveChanged(bool isActive)
{
    qCDebug(sim) << this << __FUNCTION__ << isActive;

    if(isActive)
        start();
    else
        stop();
}

void ldQSGSimulatorItem::onVisibleChanged()
{
    qCDebug(sim) << this << __FUNCTION__ << isVisible() ;

    if(!m_isActive)
        return;

    if(isVisible())
        startRendering();
    else
        stopRendering();
}

void ldQSGSimulatorItem::updateSimulatorWindowGeometry()
{
//    qDebug() << this << __FUNCTION__ << m_simulatorIndex << m_isWindow;
    if(!m_isWindow)
        return;

    int windowX = ldSettings()->value(QString("simulator_%1/windowX").arg(m_simulatorIndex), m_simulatorIndex*50).toInt();
    set_windowX(windowX);
    int windowY = ldSettings()->value(QString("simulator_%1/windowY").arg(m_simulatorIndex), 66 + m_simulatorIndex*50).toInt();
    set_windowY(windowY);
    int windowW = ldSettings()->value(QString("simulator_%1/windowW").arg(m_simulatorIndex), m_windowW).toInt();
    set_windowW(windowW);
    int windowH = ldSettings()->value(QString("simulator_%1/windowH").arg(m_simulatorIndex), m_windowH).toInt();
    set_windowH(windowH);

//    qDebug() << m_windowX << m_windowY << m_windowW << m_windowH;
}

void ldQSGSimulatorItem::handleWindowChanged(QQuickWindow *win)
{
    qCDebug(sim) << this << __FUNCTION__ << win << m_isActive ;

    if(!m_isActive)
        return;

    if (win) {
#if QT_VERSION >= 0x060000
        qDebug() << "graphicsApi" << win->graphicsApi();
#endif

        if(isVisible())
            startRendering();
    }
}


QSGNode *ldQSGSimulatorItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
//    qCDebug(sim) << "updatePaintNode" << oldNode;

    QSGGeometryNode *node = nullptr;
    QSGGeometry *geometry = nullptr;

    if (!oldNode) {
        node = new QSGGeometryNode;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), 1);
        geometry->setLineWidth(1);
        geometry->setDrawingMode(QSGGeometry::DrawLineStrip);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        auto *material = new QSGVertexColorMaterial;
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    } else {
        node = static_cast<QSGGeometryNode *>(oldNode);
        geometry = node->geometry();
    }

    if(!m_engine)
        return node;

    m_engine->fillGeometry(geometry, size());
    node->markDirty(QSGNode::DirtyGeometry);

    return node;
}
