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

#ifndef LDBPMBEATDETECTOR_H
#define LDBPMBEATDETECTOR_H

#include <memory>

#include <ldCore/Sound/ldSoundData.h>

class ldBeatDetector;

class LDCORESHARED_EXPORT ldBpmBeatDetector : public QObject
{
    Q_OBJECT

public:
    ldBpmBeatDetector(ldBeatDetector *beatDetector, QObject *parent = nullptr);
    ~ldBpmBeatDetector();
    
    int bpm() const;

public slots:
    void process(float delta);
    void reset();

private:
    ldBeatDetector *m_beatDetector;

    // bpm counter part
    float m_msBpmCounter = 0;
    int m_beatCount = 0;
    int m_bpm = 0;
};

#endif // LDBPMBEATDETECTOR_H


