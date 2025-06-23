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

#include <iostream>

#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>

#include <ldCore/ldCore.h>
#include <ldLuaGame/ldLuaGameVisualizer.h>

#include "ldCoreExample.h"

#if QT_VERSION >= 0x060000
#include <QQuickWindow>
#endif // QT_VERSION >= 0x060000

#if QT_VERSION < 0x060000
#ifdef Q_OS_IOS
#include <QtPlugin>

Q_IMPORT_PLUGIN(QtQuick2Plugin)
Q_IMPORT_PLUGIN(QtQuickControls2Plugin)
Q_IMPORT_PLUGIN(QtQuickLayoutsPlugin)
Q_IMPORT_PLUGIN(QtQuick2WindowPlugin)
Q_IMPORT_PLUGIN(QtQuickTemplates2Plugin)
#endif // Q_OS_IOS
#endif // QT_VERSION < 0x060000

int main(int argc, char *argv[]) {
    ldCore::initResources();
    ldLuaGameVisualizer::initBaseResources();

#if QT_VERSION >= 0x060000
#ifdef LD_CORE_USE_OPENGL
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGLRhi);
#endif
#endif

#if QT_VERSION < 0x060000
#ifdef Q_OS_WIN
    QGuiApplication::setAttribute(Qt::AA_UseOpenGLES, true);
#endif
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    ldCoreExample example(&engine, &app);

    return app.exec();
}
