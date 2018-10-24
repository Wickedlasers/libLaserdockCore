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

#ifndef LDGAMEPAD_H
#define LDGAMEPAD_H

#include <memory>

#include <QtCore/QSet>

#include <QQmlHelpers>

#include "ldCore/ldCore_global.h"

class QGamepad;

class ldAbstractGame;

/** Virtual gamepad controller for game */
class LDCORESHARED_EXPORT ldGamepad : public QObject
{
    Q_OBJECT

    QML_READONLY_PROPERTY(bool, isActive)

public:
    /** Gamepad button enum */
    enum Button {
        No = 0x00,
        Left = 0x01,
        Right = 0x02,
        Up = 0x04,
        Down = 0x08,
        A = 0x10,
        B = 0x20,
        X = 0x40,
        Y = 0x80,
        Start = 0x100,
        Select = 0x200
    };
    Q_DECLARE_FLAGS(Buttons, Button)
    Q_FLAG(Buttons)

    /** Register QML types */
    static void registerMetaTypes();

    /** Constructor/descructor */
    explicit ldGamepad(QObject *parent = 0);
    virtual ~ldGamepad();

public slots:
    void moveAxis(double x, double y);
    /** Press button */
    void pressButton(int button, bool isPressed);

signals:
    void axisLeftXChanged(double axisX);
    void axisLeftYChanged(double axisX);
    void axisRightXChanged(double axisX);
    void axisRightYChanged(double axisX);
    void buttonPressed(Button button, bool pressed);

private:
    void updateGamepadState();

    QSet<Button> m_pressedButtons;

    std::unique_ptr<QGamepad> m_gamepad;
};

#endif // LDGAMEPAD_H


