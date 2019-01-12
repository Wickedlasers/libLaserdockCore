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

#include "ldCore/Games/ldGamepad.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QtDebug>
#include <QtGamepad/QGamepad>
#include <QtGamepad/QGamepadManager>
#include <QtGui/QtEvents>
#include <QtQml/QQmlEngine>

#include "ldCore/ldCore.h"
#include "ldCore/Visualizations/ldVisualizationTask.h"
#include "ldCore/Games/ldAbstractGame.h"

void ldGamepad::registerMetaTypes()
{
#ifdef LD_CORE_ENABLE_QT_QUICK
    qmlRegisterUncreatableType<ldGamepad>("WickedLasers", 1, 0, "LdGamepadButton", "LdGamepadButton enum can't be created");
#endif
}

ldGamepad::ldGamepad(QObject *parent)
    : QObject(parent)
    , m_isActive(false)
{
    connect(this, &ldGamepad::isActiveChanged, this, &ldGamepad::updateGamepadState);
    connect(QGamepadManager::instance(), &QGamepadManager::connectedGamepadsChanged, this, &ldGamepad::updateGamepadState);
}

ldGamepad::~ldGamepad()
{
}

void ldGamepad::moveAxis(double x, double y)
{
    emit axisLeftXChanged(x);
    emit axisLeftYChanged(y);
}

void ldGamepad::pressButton(int button, bool isPressed)
{
    emit buttonPressed(Button(button), isPressed);
}

void ldGamepad::updateGamepadState()
{
    if(m_gamepad) {
        m_gamepad.reset();
    }

    if(!get_isActive())
        return;

    QList<int> connectedGamepads = QGamepadManager::instance()->connectedGamepads();
    if(connectedGamepads.empty())
        return;

    m_gamepad.reset(new QGamepad(connectedGamepads.first()));
    qDebug() << "Gamepad detected:" << m_gamepad->name() << m_gamepad->deviceId();

    // control buttons
    connect(m_gamepad.get(), &QGamepad::buttonStartChanged, this, [&](bool value) {
        pressButton(Button::Start, value);
    });
    connect(m_gamepad.get(), &QGamepad::buttonSelectChanged, this, [&](bool value) {
        pressButton(Button::Select, value);
    });

    // left axis
    connect(m_gamepad.get(), &QGamepad::axisLeftXChanged, this, [&](double value) {
        if(fabs(value) < 0.1)
            value = 0;
        emit axisLeftXChanged(value);
    });
    connect(m_gamepad.get(), &QGamepad::axisLeftYChanged, this, [&](double value) {
        if(fabs(value) < 0.1)
            value = 0;
        emit axisLeftYChanged(-1.0 * value);
    });

    // right axis
    connect(m_gamepad.get(), &QGamepad::axisRightXChanged, this, [&](double value) {
        if(fabs(value) < 0.1)
            value = 0;
        emit axisRightXChanged(value);
    });
    connect(m_gamepad.get(), &QGamepad::axisRightYChanged, this, [&](double value) {
        if(fabs(value) < 0.1)
            value = 0;
        emit axisRightYChanged(-1.0 * value);
    });

    // arrows
    connect(m_gamepad.get(), &QGamepad::buttonLeftChanged, this, [&](bool value) {
       pressButton(Button::Left, value);
    });
    connect(m_gamepad.get(), &QGamepad::buttonRightChanged, this, [&](bool value) {
       pressButton(Button::Right, value);
    });
    connect(m_gamepad.get(), &QGamepad::buttonUpChanged, this, [&](bool value) {
       pressButton(Button::Up, value);
    });
    connect(m_gamepad.get(), &QGamepad::buttonDownChanged, this, [&](bool value) {
       pressButton(Button::Down, value);
    });
    // action buttons
    connect(m_gamepad.get(), &QGamepad::buttonAChanged, this, [&](bool value) {
       pressButton(Button::A, value);
    });
    connect(m_gamepad.get(), &QGamepad::buttonBChanged, this, [&](bool value) {
       pressButton(Button::B, value);
    });
    connect(m_gamepad.get(), &QGamepad::buttonXChanged, this, [&](bool value) {
       pressButton(Button::X, value);
    });
    connect(m_gamepad.get(), &QGamepad::buttonYChanged, this, [&](bool value) {
       pressButton(Button::Y, value);
    });
}
