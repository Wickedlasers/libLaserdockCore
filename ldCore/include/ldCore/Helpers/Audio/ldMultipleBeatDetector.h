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

#ifndef LDBEATDETECTORCOUNTER_H
#define LDBEATDETECTORCOUNTER_H

#include <memory>

#include <QQmlHelpers>

#include <ldCore/Sound/ldSoundData.h>

class ldBeatDetector;

class LDCORESHARED_EXPORT ldMultipleBeatDetector : public QObject
{
    Q_OBJECT

    QML_WRITABLE_PROPERTY(bool, isActive)
public:
    ldMultipleBeatDetector(const ldBeatDetector *beatDetector, QObject *parent = nullptr);
    ~ldMultipleBeatDetector();
    
    void setBeatCount(int beatCount);
    void reset();

    float progress() const; // 0..1

signals:
    void beatDetected();

private slots:
    void onActiveChanged(bool active);
    void onBeatDetected();

private:
    const ldBeatDetector *m_beatDetector;
    int m_detectedBeats = 0;
    int m_beatCount = 1;
};

#endif // LDBEATDETECTORCOUNTER_H


