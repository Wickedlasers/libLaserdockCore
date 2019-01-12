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

#ifndef LDMIDIMANAGER_H
#define LDMIDIMANAGER_H

#include <vector>

#include <QtCore/QString>
#include <QtCore/QElapsedTimer>
#include <QtCore/QMutex>

#include "ldCore/Sound/Midi/ldMidiTypes.h"

#define MAX_MIDI_EVENTS 64

struct LDCORESHARED_EXPORT ldMidiNotePlayed {
    int note;
    bool isStillPressed;
    float velocity;
    float duration;
    float timeSincePressed;
    float timeSinceReleased;
    qint64 timeStampPressed;
    qint64 timeStampReleased;
};

class LDCORESHARED_EXPORT ldMidiManager : public QObject
{
    Q_OBJECT
public:
    // class stuff
    static ldMidiManager* getSharedManager();
    ~ldMidiManager();


    // accessor
    std::vector<ldMidiNotePlayed> getNotesPlayed();

    //
public slots:
    void addEvent(ldMidiNote);
    void addEvents(QList<ldMidiNote>);

signals:
    void eventAdded(const ldMidiNote &note);

private:
    ldMidiManager();
    // control
    void reset();
    void update();

    QMutex m_mutex;
    std::vector<ldMidiNotePlayed> notePlayedArray;
    QElapsedTimer etimer;
};


#endif // LDMIDIMANAGER_H
