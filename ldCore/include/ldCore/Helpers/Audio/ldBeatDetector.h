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

#ifndef LDBEATDETECTOR_H
#define LDBEATDETECTOR_H

#include <memory>

#include <ldCore/Sound/ldSoundData.h>

class LDCORESHARED_EXPORT ldBeatDetector : public QObject
{
    Q_OBJECT

public:
    ldBeatDetector(QObject *parent = nullptr);
    ~ldBeatDetector();
    
    void process(float bpm, float output, float delta);

    void setDuration(float duration);
    void reset();

    float progress() const; // 0..1

signals:
    void beatDetected();

private:
    float m_duration = 1.f;

    // beat detection part
    float m_msCounter = 0;
    bool m_isRunningBpmCounter = false;
    int m_minCurrentMillis = 500;
};

#endif // LDBEATDETECTOR_H


