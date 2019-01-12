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

// ldQSound.h
//  Created by MEO on 20/03/2017 (Sergey's code)
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.

#ifndef LDQSOUND_H
#define LDQSOUND_H

#include <memory>

#include <QtCore/QMutex>
#include <QtCore/QObject>

#include "ldCore/ldCore_global.h"

class QSoundEffect;

/// simple QSound wrapper. It recreates QSound instance on each play() toggle
/// it was created because if you change default sound output when app is already launched QSound won't play anything
class LDCORESHARED_EXPORT ldQSound : public QObject
{
    Q_OBJECT
public:

    // see QSoundEffect
    enum Loop
    {
        Infinite = -2
    };

    explicit ldQSound(const QString &filename);
    ~ldQSound();

    void play();
    void stop();

    void setForce(bool force);

    void setLoops(int loops);

    void setVolumeCoeff(qreal volume); // 0..1
    void setVolumeLevel(int level); // 0..100

private:
    void playImpl();
    void stopImpl();

    QMutex m_mutex;

    bool m_force = true;
    QString m_filename;
    int m_loops = 1;
    qreal m_volumeCoeff = 1;
    int m_volumeLevel = 100;

    std::vector<std::unique_ptr<QSoundEffect>> m_sounds;
};


#endif // LDQSOUND_H
