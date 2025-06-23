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
#include <QtCore/QLoggingCategory>

#ifdef LD_CORE_ENABLE_QGAMEPAD
#if QT_VERSION >= 0x060000
#include <QtGamepadLegacy/QGamepadManager>
#else
#include <QtGamepad/QGamepadManager>
#endif
#endif
#include <QtGui/QtEvents>
#include <QtQml/QQmlEngine>

#include "ldCore/ldCore.h"
#include "ldCore/Visualizations/ldVisualizationTask.h"
#include "ldCore/Games/ldAbstractGame.h"

namespace {
    Q_LOGGING_CATEGORY(ldg, "ld.gamepad")
}

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
#ifdef LD_CORE_ENABLE_QGAMEPAD
    connect(QGamepadManager::instance(), &QGamepadManager::connectedGamepadsChanged, this, &ldGamepad::updateGamepadState);
#else
    qWarning() << "QGamepad support is disabled";
#endif
}

ldGamepad::~ldGamepad()
{
    disconnectGamepadEvents();
}

void ldGamepad::buttonEvent(int button, bool state)
{
#ifdef LD_CORE_ENABLE_QGAMEPAD
    switch(button)
    {
        case QGamepadManager::GamepadButton::ButtonStart :
            pressButton(ldGamepad::Start,state);
        break;
        case QGamepadManager::GamepadButton::ButtonSelect :
            pressButton(ldGamepad::Select,state);
        break;
        case QGamepadManager::GamepadButton::ButtonLeft :
            pressButton(ldGamepad::Left,state);
        break;
        case QGamepadManager::GamepadButton::ButtonRight :
            pressButton(ldGamepad::Right,state);
        break;
        case QGamepadManager::GamepadButton::ButtonUp :
            pressButton(ldGamepad::Up,state);
        break;
        case QGamepadManager::GamepadButton::ButtonDown :
            pressButton(ldGamepad::Down,state);
        break;
        case QGamepadManager::GamepadButton::ButtonA :
            pressButton(ldGamepad::A,state);
        break;
        case QGamepadManager::GamepadButton::ButtonB :
            pressButton(ldGamepad::B,state);
        break;
        case QGamepadManager::GamepadButton::ButtonX :
            pressButton(ldGamepad::X,state);
        break;
        case QGamepadManager::GamepadButton::ButtonY :
            pressButton(ldGamepad::Y,state);
        break;
        default:
        break;
    }
#endif
}

void ldGamepad::gamepadAxisEvent(int deviceId, int axis, double value)
{
#ifdef LD_CORE_ENABLE_QGAMEPAD
    if (get_isActive() && deviceId == m_gamepadId) {
        qCDebug(ldg) << "GamePad axis event:" << deviceId << axis << value;

        if(fabs(value) < 0.1)
            value = 0;

        switch (axis) {
         case QGamepadManager::AxisLeftX:
             emit axisLeftXChanged(value);
             break;
         case QGamepadManager::AxisLeftY:
             emit axisLeftYChanged(-1.0 * value);
             break;
         case QGamepadManager::AxisRightX:
             emit axisRightXChanged(value);
             break;
         case QGamepadManager::AxisRightY:
             emit axisRightYChanged(-1.0 * value);
             break;
         default:
             break;
        }
    }
#endif
}

void ldGamepad::gamepadPressedEvent(int deviceId, int button, double value)
{
    Q_UNUSED(value)

#ifdef LD_CORE_ENABLE_QGAMEPAD
    if (get_isActive() && deviceId == m_gamepadId) {
        qCDebug(ldg) << "GamePad press event:" << deviceId << button << value;

        buttonEvent(button,true);
    }
#endif
}

void ldGamepad::gamepadReleasedEvent(int deviceId, int button)
{
#ifdef LD_CORE_ENABLE_QGAMEPAD
    if (get_isActive() && deviceId == m_gamepadId) {
        qCDebug(ldg) << "GamePad release event:" << deviceId << button;

        buttonEvent(button,false);
    }
#endif
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

void ldGamepad::connectGamepadEvents()
{
#ifdef LD_CORE_ENABLE_QGAMEPAD
    connect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonPressEvent, this, &ldGamepad::gamepadPressedEvent);
    connect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonReleaseEvent, this,&ldGamepad::gamepadReleasedEvent);
    connect(QGamepadManager::instance(), &QGamepadManager::gamepadAxisEvent,this,&ldGamepad::gamepadAxisEvent);
#endif
}

void ldGamepad::disconnectGamepadEvents()
{
#ifdef LD_CORE_ENABLE_QGAMEPAD
    if (m_gamepadId!=-1) {
        m_gamepadId = -1;
        disconnect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonPressEvent, this, &ldGamepad::gamepadPressedEvent);
        disconnect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonReleaseEvent, this,&ldGamepad::gamepadReleasedEvent);
        disconnect(QGamepadManager::instance(), &QGamepadManager::gamepadAxisEvent,this,&ldGamepad::gamepadAxisEvent);
    }
#endif
}


void ldGamepad::updateGamepadState()
{
#ifdef LD_CORE_ENABLE_QGAMEPAD
    if(!get_isActive()) {
        qCDebug(ldg) << __FUNCTION__ << get_isActive() << m_gamepadId;
        if (m_gamepadId!=-1) { // disconnect from gamepad events if we were previously connected to them
            disconnectGamepadEvents();
        }
        return;
    }

    QList<int> connectedGamepads = QGamepadManager::instance()->connectedGamepads();
    qCDebug(ldg) << __FUNCTION__ << get_isActive() << connectedGamepads.size() << m_gamepadId;

    if(connectedGamepads.empty()) { // if all gamepads are disconnected, stop all gamepad events
        if (m_gamepadId!=-1) { // disconnect from gamepad events if we were previously connected to them
            disconnectGamepadEvents();
        }
        return;
    }

    // always use the last gamepad
    // on android there is some issue that if you disconnect/connect gamepads it can detect a lot of ghost gamepads
    // the last id is valid though
    int id = connectedGamepads.last();

    qCDebug(ldg) << __FUNCTION__ << get_isActive() << connectedGamepads.size() << id << m_gamepadId;

    if (id != m_gamepadId) { // new gamepad to connect to?
        int oldId = m_gamepadId;
        m_gamepadId = id;

        if (oldId == -1) { // previously disconnected from gamepad events?
            connectGamepadEvents(); // connect to gamepad events
        }
    }
#endif
}
