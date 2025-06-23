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

#ifndef LDSIMULATORITEM_H
#define LDSIMULATORITEM_H

#include <QQuickItem>
#include <QQmlHelpers>

#include "ldCore/ldCore_global.h"

class ldSimulatorRenderer;

class LDCORESHARED_EXPORT ldSimulatorItem : public QQuickItem
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY(QColor, clearColor)

    QML_WRITABLE_PROPERTY(bool, isActive)

    QML_WRITABLE_PROPERTY(bool, isActiveRendering)

public:
    static void registerMetatypes();

    explicit ldSimulatorItem(QQuickItem *parent = 0);
    ~ldSimulatorItem();

public slots:
    void start();
    void stop();

#if QT_VERSION < 0x060000
    void paint();
    void update();
#endif

    void sync();
#if QT_VERSION < 0x060000
    void init();
#endif
    void cleanup();

private slots:
    void handleWindowChanged(QQuickWindow *win);
    void onActiveRenderingChanged(bool isActiveRendering);
    void onIsActiveChanged(bool isActive);
private:
    void activateRendering();
    void deactivateRendering();
    void setGlViewport();

    QScopedPointer<ldSimulatorRenderer> m_renderer;
};

#endif // LDSIMULATORITEM_H
