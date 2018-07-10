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

#ifndef LDCOREEXAMPLE_H
#define LDCOREEXAMPLE_H

#include <memory>

#include <QQmlHelpers>

class QQmlApplicationEngine;

class ldCore;
class ldSpiralFighterGame;
class ldVisualizer;

class ldCoreExample : public QObject
{
    Q_OBJECT

    QML_CONSTANT_PROPERTY(ldCore*, ldCore)
    QML_CONSTANT_PROPERTY(ldSpiralFighterGame*, game)

public:
    ldCoreExample(QQmlApplicationEngine *engine, QObject *parent = nullptr);
    ~ldCoreExample();

    void init();

public slots:
    void activateVis(int index);

    void setWindow(QObject *window);

private:
    QQmlApplicationEngine *m_qmlEngine;

    std::vector<std::unique_ptr<ldVisualizer>> m_visualizers;
};

#endif // LDCOREEXAMPLE_H
