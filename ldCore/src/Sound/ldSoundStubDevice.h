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

#ifndef LDSOUNDSTUBDEVICE_H
#define LDSOUNDSTUBDEVICE_H

#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QTimer>

class ldSoundStubDevice : public QObject
{
    Q_OBJECT
public:
    explicit ldSoundStubDevice(QObject *parent);
    ~ldSoundStubDevice();
        
    void start();
    virtual void stop();
    
signals:
    void soundUpdated(const char * data, qint64 len);

protected slots:
    virtual void timerSlot();

protected:
    QMutex m_mutex;

private:
    static const int STUBFPS = 30*2;

    static const int FAKEDATA_LEN = 44100 / STUBFPS * 2 * 2;  // stereo int
    char m_fakeData[FAKEDATA_LEN];

    QTimer m_timer;
};

#endif //LDSOUNDSTUBDEVICE_H

