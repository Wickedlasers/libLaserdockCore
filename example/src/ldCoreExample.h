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

#ifndef LDCOREEXAMPLE_H
#define LDCOREEXAMPLE_H

#include <memory>

#include <QQmlHelpers>

#include <ldCore/ldCore.h>

#include "ldAngryLasers.h"
#include "ldArrow.h"
#include "ldSerpent.h"
#include "ldSpiralFighterGame.h"

class QQmlApplicationEngine;

class ldVisualizer;

#ifdef Q_OS_ANDROID
class ldResourcesExtractor;
#endif

class ldCoreExample : public QObject
{
    Q_OBJECT

    QML_CONSTANT_PROPERTY(ldCore*, ldCore)
    QML_READONLY_PROPERTY(ldAbstractGame*, game)

public:
    ldCoreExample(QQmlApplicationEngine *engine, QObject *parent = nullptr);
    ~ldCoreExample();

    void init();

    /** QObject */
    virtual bool eventFilter(QObject *obj, QEvent *ev) override;

public slots:
    void activateVis(int index);

    void setWindow(QObject *window);

private:
    void startApp();

    QQmlApplicationEngine *m_qmlEngine;

    std::vector<std::unique_ptr<ldVisualizer>> m_visualizers;

#ifdef Q_OS_ANDROID
    ldResourcesExtractor* m_resExtractor;
#endif

    ldAngryLasers* m_angryLasersGame = nullptr;
    ldArrow* m_arrowGame = nullptr;
    ldSerpent* m_serpentGame = nullptr;
    ldSpiralFighterGame* m_spiralFigtherGame = nullptr;
};

#endif // LDCOREEXAMPLE_H
