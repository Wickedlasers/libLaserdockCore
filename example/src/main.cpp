/**
**	libLaserdockCore
**	Copyright(c) 2018 Wicked Lasers
**
**	GNU Lesser General Public License
**	This file may be used under the terms of the GNU Lesser
**  General Public License version 3 as published by the Free
**  Software Foundation and appearing in the file LICENSE.LGPLv3 included
**  in the packaging of this file. Please review the following information
**  to ensure the GNU Lesser General Public License requirements
**  will be met: https://www.gnu.org/licenses/lgpl.html
**
**/

#include <iostream>

#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>

#include <ldCore/ldCore.h>

#include "ldCoreExample.h"

int main(int argc, char *argv[]) {
    ldCore::initResources();

#ifdef Q_OS_WIN
    QGuiApplication::setAttribute(Qt::AA_UseOpenGLES, true);
#endif
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    ldCoreExample example(&engine, &app);

    return app.exec();
}
