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

#ifndef LDDROPDETECTOR_H
#define LDDROPDETECTOR_H

#include <QtCore/QObject>

#include <ldCore/ldCore_global.h>

/** advanced drop detector,  probably not finished */
class LDCORESHARED_EXPORT ldDropDetector : public QObject
{
    Q_OBJECT

public:
    ldDropDetector(QObject *parent = nullptr);
    ~ldDropDetector();

    void process(float delta);

    // drop detect settings
    void setDropDetectEnabledValue(bool value);
    void setDropDetectSensValue(int value);

signals:
    void dropDetected();

private:

    float m_dropDetectorLockout = 0;
    bool m_dropDetectorEnabled = false;
    float m_dropDetectorSens = 0.5f;
};


#endif // LDDROPDETECTOR_H


