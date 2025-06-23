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

void fillGeometryFromVertex(QSGGeometry *geometry, const QSizeF &itemSize, const std::vector<ldVertex> &buffer, unsigned int length)
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

}

void ldQSGSimulatorItem::registerMetatypes()
{
    qmlRegisterType<ldQSGSimulatorItem>("WickedLasers", 1, 0, "LdQSGSimulatorItem");
}

ldQSGSimulatorItem::ldQSGSimulatorItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_isActive(false)
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

   // qDebug() << "ldQSGSimulatorItem" ;

    // ldCore::instance()->get_dataDispatcherManager()->dataDispatcherCreated();
    connect(ldCore::instance()->get_dataDispatcherManager(), &ldDataDispatcherManager::dataDispatcherCreated, this, &ldQSGSimulatorItem::loadEngine);
    connect(this, &ldQSGSimulatorItem::isWindowChanged, this, &ldQSGSimulatorItem::updateSimulatorWindowGeometry);
    // connect(this, &ldQSGSimulatorItem::simulatorIndexChanged, this, &ldQSGSimulatorItem::updateSimulatorWindowGeometry);

    qCDebug(sim) << __FUNCTION__ << isVisible() << m_isActive;

    // clean old properties, we could have until of simulators before
    for(int i = 0; i < 7; i++) {
        ldSettings()->remove(QString("simulator_%1").arg(i));
    }
}

ldQSGSimulatorItem::~ldQSGSimulatorItem()
{
   // qDebug() << "~ldQSGSimulatorItem" ;

    if(m_isWindow) {
//        qDebug() << m_windowX << m_windowY << m_windowW << m_windowH;

        ldSettings()->setValue(QString("simulator/windowX"), m_windowX);
        ldSettings()->setValue(QString("simulator/windowY"), m_windowY);
        ldSettings()->setValue(QString("simulator/windowW"), m_windowW);
        ldSettings()->setValue(QString("simulator/windowH"), m_windowH);
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
    // if(m_engines.empty())
    //     return;

    if(!m_isActive)
        return;

    for(int i = m_engines.size(); i < ldCore::instance()->get_dataDispatcherManager()->get_count(); i++) {
        ldDataDispatcherInstance *dd = ldCore::instance()->get_dataDispatcherManager()->get(i);
        if(dd) {
            ldSimulatorEngine *engine = dd->dataDispatcher->simulatorEngine();
            connect(engine,
                    &ldSimulatorEngine::destroyed,
                    this,
                    &ldQSGSimulatorItem::disconnectEngine,
                    static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::DirectConnection)
                    );
            engine->addListener();
            m_engines.push_back(engine);
        }
    }
}

void ldQSGSimulatorItem::unloadEngine()
{
    if(m_engines.empty())
        return;

    for(uint i = 0; i < m_engines.size(); i++) {
        m_engines[i]->removeListener();
    }

    m_engines.clear();
}

void ldQSGSimulatorItem::disconnectEngine(QObject *obj)
{
    auto it = m_engines.begin();
    while(it != m_engines.end()) {
        if(*it == obj)
            it = m_engines.erase(it);
        else
            it++;
    }
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
   // qDebug() << this << __FUNCTION__ << m_isWindow;
    if(!m_isWindow)
        return;

    int windowX = ldSettings()->value(QString("simulator/windowX"), 50).toInt();
    set_windowX(windowX);
    int windowY = ldSettings()->value(QString("simulator/windowY"), 66).toInt();
    set_windowY(windowY);
    int windowW = ldSettings()->value(QString("simulator/windowW"), m_windowW).toInt();
    set_windowW(windowW);
    int windowH = ldSettings()->value(QString("simulator/windowH"), m_windowH).toInt();
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

class SimulatorNode : public QObject, public QSGGeometryNode
{
    Q_OBJECT
public:
    SimulatorNode(QQuickItem *parentItem)
        : m_parentItem(parentItem)
    {
        QSGGeometry *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), 1);
        geometry->setLineWidth(1);
        geometry->setDrawingMode(QSGGeometry::DrawLineStrip);
        setGeometry(geometry);
        setFlag(QSGNode::OwnsGeometry);
        auto *material = new QSGVertexColorMaterial;
        setMaterial(material);
        setFlag(QSGNode::OwnsMaterial);
    }

    void updateFromEngine(ldSimulatorEngine * engine)
    {
        if(!engine)
            return;

        fillGeometryFromVertex(geometry(), m_parentItem->size(), engine->buffer(), engine->bufferSize());
        markDirty(QSGNode::DirtyGeometry);
    }

private:
    QQuickItem *m_parentItem = nullptr;
};

class BorderNode : public QObject, public QSGGeometryNode
{
    Q_OBJECT
public:
    BorderNode(QQuickItem *parentItem)
        : m_parentItem(parentItem)
    {
        QSGGeometry *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), 1);
        geometry->setLineWidth(1);
        geometry->setDrawingMode(QSGGeometry::DrawLineStrip);
        setGeometry(geometry);
        setFlag(QSGNode::OwnsGeometry);
        auto *material = new QSGVertexColorMaterial;
        setMaterial(material);
        setFlag(QSGNode::OwnsMaterial);

        float c = 0.5f;

        m_initialGrid.push_back(ldVertex{{-1,-1}, {0,0,0}});
        m_initialGrid.push_back(ldVertex{{-1,-1}, {c,c,c}});
        m_initialGrid.push_back(ldVertex{{-1,1}, {c,c,c}});
        m_initialGrid.push_back(ldVertex{{1,1}, {c,c,c}});
        m_initialGrid.push_back(ldVertex{{1,-1}, {c,c,c}});
        m_initialGrid.push_back(ldVertex{{-1,-1}, {c,c,c}});
        m_initialGrid.push_back(ldVertex{{-1,-1}, {0,0,0}});
    }

    void updateFromEngine(bool visible)
    {
        if(visible)
            fillGeometryFromVertex(geometry(), m_parentItem->size(), m_initialGrid, m_initialGrid.size());
        else
           geometry()->allocate(0);

        markDirty(QSGNode::DirtyGeometry);
    }

private:
    std::vector<ldVertex> m_initialGrid;

    QQuickItem *m_parentItem = nullptr;
};


QSGNode *ldQSGSimulatorItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
//    qCDebug(sim) << "updatePaintNode" << oldNode;

    QSGNode *node = nullptr;

    if (!oldNode) {
        node = new QSGNode;
    } else {
        node = static_cast<QSGNode *>(oldNode);
    }


    // create new nodes
    for(int i = node->childCount(); i < m_engines.size(); i++) {
        QSGClipNode *clipNode = new QSGClipNode;

        clipNode->setClipRect(QRectF(0, 0, width(), height()));
        clipNode->setIsRectangular(true);

        QSGTransformNode *transformNode = new QSGTransformNode;
        BorderNode *borderNode = new BorderNode(this);


        SimulatorNode *simulatorNode = new SimulatorNode(this);

        borderNode->appendChildNode(simulatorNode);
        transformNode->appendChildNode(borderNode);
        clipNode->appendChildNode(transformNode);
        node->appendChildNode(clipNode);
    }


    // remove unnecessary nodes
    int childIndex = m_engines.size();
    while(childIndex < node->childCount()) {
        QSGNode *childNode = node->childAtIndex(childIndex);
        node->removeChildNode(childNode);
        // node->markDirty(QSGNode::DirtyNodeRemoved);
    }

    // grid view, max 16 now
    int gridSize = 1;
    if(node->childCount() > 4) {
        gridSize = 3;
    } else if(node->childCount() > 1) {
        gridSize = 2;
    }

    double nodeWidth = width()/gridSize;
    double nodeHeight = height()/gridSize;

    for(int i = 0; i < node->childCount() && i < m_engines.size(); i++) {
        int row = i / gridSize;
        int column = i % gridSize;

        QSGClipNode *clipNode = static_cast<QSGClipNode *>(node->childAtIndex(i));
        clipNode->setClipRect(QRectF(column * nodeWidth, row * nodeHeight, nodeWidth, nodeHeight));

        QSGTransformNode *transformNode = static_cast<QSGTransformNode *>(clipNode->childAtIndex(0));

        QMatrix4x4 mat;
        mat.translate(column * nodeWidth, row * nodeHeight, 0);
        mat.scale(1./gridSize);
        transformNode->setMatrix(mat);
        transformNode->markDirty(QSGNode::DirtyMatrix);

        BorderNode *borderNode = static_cast<BorderNode *>(transformNode->childAtIndex(0));
        borderNode->updateFromEngine(node->childCount() > 1);

        SimulatorNode *simulatorNode = static_cast<SimulatorNode *>(borderNode->childAtIndex(0));
        simulatorNode->updateFromEngine(m_engines[i]);
    }

    return node;
}

#include "ldQSGSimulatorItem.moc"
