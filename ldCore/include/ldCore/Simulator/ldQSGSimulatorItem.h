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

#ifndef LDQSGSIMULATORITEM_H
#define LDQSGSIMULATORITEM_H

#include <QtQuick/QQuickItem>
#include <QQmlHelpers>

#include <ldCore/ldCore_global.h>

class ldSimulatorEngine;

class LDCORESHARED_EXPORT ldQSGSimulatorItem : public QQuickItem
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY(bool, isActive)

    QML_WRITABLE_PROPERTY(bool, isWindow)
    QML_WRITABLE_PROPERTY(int, windowX)
    QML_WRITABLE_PROPERTY(int, windowY)
    QML_WRITABLE_PROPERTY(int, windowW)
    QML_WRITABLE_PROPERTY(int, windowH)

public:
    static void registerMetatypes();

    explicit ldQSGSimulatorItem(QQuickItem *parent = 0);
    ~ldQSGSimulatorItem();

    virtual QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

public slots:
    void start();
    void stop();

private:
    void loadEngine();
    void unloadEngine();
    void disconnectEngine(QObject *obj);

    void startRendering();
    void stopRendering();

    void onIsActiveChanged(bool isActive);
    void onVisibleChanged();
    void updateSimulatorWindowGeometry();
    void handleWindowChanged(QQuickWindow *win);

    std::vector<ldSimulatorEngine*> m_engines;
};

#endif // ldQSGSimulatorItem_H
