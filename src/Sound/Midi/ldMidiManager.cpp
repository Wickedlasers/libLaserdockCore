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

#include "ldCore/Sound/Midi/ldMidiManager.h"

#include <QtCore/QDebug>

ldMidiManager::ldMidiManager()
{
    qRegisterMetaType<ldMidiNote>();
    qRegisterMetaType<ldMidiCCMessage>();
    reset();
}

ldMidiManager::~ldMidiManager() {}

// singleton
ldMidiManager* ldMidiManager::getSharedManager() {
    static ldMidiManager me;
    return &me;
}

std::vector<ldMidiNotePlayed> ldMidiManager::getNotesPlayed() {
    QMutexLocker locker(&m_mutex);
    update();
    return notePlayedArray;
}

void ldMidiManager::addEvent(ldMidiNote e) {
    //qDebug() << "add";
    QMutexLocker locker(&m_mutex);

    qint64 time = etimer.elapsed();

    // release noteplayed objects for this note
    for (uint i = 0; i < notePlayedArray.size(); i++) {
        ldMidiNotePlayed &p = notePlayedArray[i];
        if (p.note == e.note) {
            if (p.isStillPressed) {
                p.timeSinceReleased = 0;
                p.timeStampReleased = time;
                p.isStillPressed = false;
            }
        }
    }

    // add if it's an onset
    if (e.onset) {

        // resize array
        int newsize = notePlayedArray.size() + 1;
        if (newsize > MAX_MIDI_EVENTS) newsize = MAX_MIDI_EVENTS;
        notePlayedArray.resize(newsize);

        // shift array
        for (int i = newsize-1; i >= 1; i--) {
            notePlayedArray[i] = notePlayedArray[i-1];
        }

        // create a noteplayed object
        ldMidiNotePlayed t;
        t.note = e.note; // midi note index
        t.velocity = e.velocity / 128.0f; // convert to 0-1 range
        t.isStillPressed = true;
        t.timeSincePressed = 0;
        t.timeSinceReleased = -1;
        t.timeStampPressed = time;
        t.timeStampReleased = -1;

        // add noteplayed object to front of array
        notePlayedArray[0] = t;
    }

    emit eventAdded(e);

    //qDebug() << "midi notes size is " << notePlayedArray.size();
}


void ldMidiManager::addEvents(QList<ldMidiNote> event_queue)
{
    for (int i = 0; i < event_queue.length(); i++) {
        addEvent(event_queue[i]);
    }
}

void ldMidiManager::reset()
{
    notePlayedArray.reserve(MAX_MIDI_EVENTS);
    notePlayedArray.resize(0);
    etimer.start();
}

void ldMidiManager::update()
{
    qint64 time = etimer.elapsed();

    for (uint i = 0; i < notePlayedArray.size(); i++) {
        ldMidiNotePlayed& p = notePlayedArray[i];
        p.timeSincePressed = (time-p.timeStampPressed)/1000.0f;
        if (p.isStillPressed) {
            p.duration = p.timeSincePressed;
        } else {
            p.timeSinceReleased = (time-p.timeStampReleased)/1000.0f;
            p.duration = (p.timeStampReleased-p.timeStampPressed)/1000.0f;
        }
    }
}
