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

#ifndef LDMIDITYPES_H
#define LDMIDITYPES_H

#include <QtCore/QObject>

#include "ldCore/ldCore_global.h"

class LDCORESHARED_EXPORT ldMidiNote {
public:
    uint8_t note = 0;
    uint8_t velocity = 0;
    bool onset = false;

    static QString noteToString(uint8_t note);
    static uint8_t noteFromString(const QString &noteStr, bool &ok);
};
Q_DECLARE_METATYPE(ldMidiNote)

class LDCORESHARED_EXPORT ldMidiCCMessage {
public:
    static const uint8_t MAX_VALUE = 127;

    uint8_t faderNumber = 0;
    uint8_t value = 0;
};
Q_DECLARE_METATYPE(ldMidiCCMessage)

#endif // LDMIDITYPES_H
