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

#include "ldCore/Sound/Midi/ldMidiTypes.h"


QString ldMidiNote::noteToString(uint8_t note)
{
    QString noteStr;
    switch(note % 12) {
    case 0: noteStr = "C";
        break;
    case 1: noteStr = "C#";
        break;
    case 2: noteStr =  "D";
        break;
    case 3: noteStr = "D#";
        break;
    case 4: noteStr = "E";
        break;
    case 5: noteStr = "F";
        break;
    case 6: noteStr = "F#";
        break;
    case 7: noteStr = "G";
        break;
    case 8: noteStr = "G#";
        break;
    case 9: noteStr = "A";
        break;
    case 10: noteStr = "A#";
        break;
    case 11: noteStr = "B";
        break;
    }

    int octave = note / 12;

    noteStr += QString::number(octave);

    return noteStr;
}

uint8_t ldMidiNote::noteFromString(const QString &noteStr, bool &ok)
{
    ok = false;

    uint8_t result = 0;
    if(noteStr.length() < 2) {
        return result;
    }

    int indexToCheck = 0;
    QChar mainNote = noteStr[indexToCheck].toUpper();
    indexToCheck++;
    QChar sharpChar = noteStr[indexToCheck];
    if(mainNote == 'C') {
        result += 0;
        if(sharpChar == "#") {
            indexToCheck++;
            result += 1;
        }
    } else if(mainNote == 'D') {
        result += 2;
        if(sharpChar == "#") {
            indexToCheck++;
            result += 1;
        }
    } else if(mainNote == 'E') {
        result += 4;
    } else if(mainNote == 'F') {
        result += 5;
        if(sharpChar == "#") {
            indexToCheck++;
            result += 1;
        }
    } else if(mainNote == 'G') {
        result += 7;
        if(sharpChar == "#") {
            indexToCheck++;
            result += 1;
        }
    } else if(mainNote == 'A') {
        result += 9;
        if(sharpChar == "#") {
            indexToCheck++;
            result += 1;
        }
    } else if(mainNote == 'B') {
        result += 11;
    } else {
        return result;
    }

    if(indexToCheck >= noteStr.length()) {
        return result;
    }

    bool octaveOk;
    int octave = noteStr.mid(indexToCheck).toInt(&octaveOk);
    if(!octaveOk) {
        return result;
    }

    result += octave * 12;

    ok = true;
    return result;
}
