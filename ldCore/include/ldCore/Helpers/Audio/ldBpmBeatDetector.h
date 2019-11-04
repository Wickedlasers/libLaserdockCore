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

class ldDropDetector;

class LDCORESHARED_EXPORT ldBpmBeatDetector : public QObject
{
    Q_OBJECT

public:
    ldBpmBeatDetector(QObject *parent = nullptr);
    ~ldBpmBeatDetector();
    
    void process(float bpm, float output, float delta);

    int bpm() const;

    void setDuration(float duration);
    void reset();

    ldDropDetector *dropDetector() const;

signals:
    void beatDetected();

private:
    std::unique_ptr<ldDropDetector> m_dropDetector;

    float m_duration = 1.f;

    float m_msCounter = 0;
    bool m_isRunningBpmCounter = false;
    int m_minCurrentMillis = 500;

    float m_msBpmCounter = 0;
    int m_beatCount = 0;
    int m_bpm = 0;
};

#endif // LDBPMBEATDETECTOR_H


