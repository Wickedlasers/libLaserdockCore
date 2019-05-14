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

#ifndef LDBESTBPMBEATDETECTOR_H
#define LDBESTBPMBEATDETECTOR_H

#include <ldCore/Sound/ldSoundData.h>

class LDCORESHARED_EXPORT ldBestBpmBeatDetector : public QObject
{
    Q_OBJECT
public:
    ldBestBpmBeatDetector(QObject *parent = nullptr);
    ~ldBestBpmBeatDetector();
    
    void processBpm(float bestBpm, float output, float delta);

    int bpm() const;

signals:
    void beatDetected();

private:
    float m_milliSecondsCounter = 0;
    float m_milliSecondsCounter2 = 0;
    int m_minCurrentMillis = 500;
    bool m_isRunningBPMCounter = false;
    int bpmCount = 0;

    int m_bpm = 0;
};

#endif // LDBESTBPMBEATDETECTOR_H


