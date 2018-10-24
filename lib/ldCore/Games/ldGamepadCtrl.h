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

#ifndef LDGAMEPADCTRL_H
#define LDGAMEPADCTRL_H

#include <memory>

#include <QtCore/QSet>

#include <QQmlHelpers>

#include "ldCore/ldCore_global.h"

#include "ldGamepad.h"

class ldAbstractGame;

/** Virtual gamepad controller for game */
class LDCORESHARED_EXPORT ldGamepadCtrl : public QObject
{
    Q_OBJECT

    /** String button descriptions - first is button label, second is description */
    Q_PROPERTY(QStringList buttonDescriptions READ get_buttonDescriptions CONSTANT)

public:
    /** Constructor/descructor */
    explicit ldGamepadCtrl(ldAbstractGame *game, QObject *parent = 0);
    virtual ~ldGamepadCtrl();

    /** Mandatory init function. Each gamepad button is connected to Qt::Key event internally*/
    void init(const QMap<ldGamepad::Button, Qt::Key> &buttonKeyMap, const QStringList &keyDescriptions = QStringList());

    /** Property accessor */
    QStringList get_buttonDescriptions() const;

public slots:
    /** Press button */
    void pressButton(ldGamepad::Button button, bool isPressed);
    /** Get Qt::Key code from button. Return -1 if not found */
    int getKey(int button);

private:
    QList<ldGamepad::Button> getButtonsFromInt(int button);

    QString buttonToString(const ldGamepad::Button &button) const;
    QString lookForButtonDescription(const ldGamepad::Button &button, const QStringList &keyDescriptions) const;

    ldAbstractGame *m_game = nullptr;

    QMap<ldGamepad::Button, Qt::Key> m_buttonKeyMap;
    QStringList m_buttonDescriptions;

};

#endif // LDGAMEPADCTRL_H


