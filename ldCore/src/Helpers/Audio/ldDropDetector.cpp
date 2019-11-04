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

#include "ldCore/Helpers/Audio/ldDropDetector.h"

#include <QtCore/QDebug>

#include "ldCore/ldCore.h"
#include "ldCore/Helpers/Maths/ldMathStat.h"
#include "ldCore/Helpers/Visualizer/ldVisualizerHelper.h"
#include "ldCore/Visualizations/MusicManager/ldMusicManager.h"

ldDropDetector::ldDropDetector(QObject *parent)
    : QObject(parent)
{
}

ldDropDetector::~ldDropDetector()
{
}

void ldDropDetector::process(float delta)
{

    // drop detector
    float dropDetectValue = 0;
    {
        float tf2 = (0.5f-ldCore::instance()->musicManager()->mrSlowBass->walkerOutput);
        tf2 = tf2*tf2 / 0.25f;
        float tf3 = (0.5f-ldCore::instance()->musicManager()->mrFastBass->walkerOutput);
        tf3 = tf3*tf3 / 0.25f;

        float dropDetectValue2;
        dropDetectValue2 = 0;
        dropDetectValue2 += clampf(ldCore::instance()->musicManager()->mrSlowBass->output * 1.25f - 0.25f, 0, 1);
        dropDetectValue2 += clampf(ldCore::instance()->musicManager()->mrSlowTreb->output * 1.25f - 0.25f, 0, 1);
        dropDetectValue2 += tf2 / 2;
        dropDetectValue2 += tf3 / 2;
        dropDetectValue2 *= 1.0f/3.0f;
        clampfp(dropDetectValue2, 0, 1);
        dropDetectValue2 *= clampf(ldCore::instance()->musicManager()->soundLevel() / 100.0f, 0.125f, 0.5f) / 0.5f;
        clampfp(dropDetectValue2, 0, 1);

        dropDetectValue = dropDetectValue2;
    }
    // apply drop detect
    {
        bool change = false;
        if (m_dropDetectorLockout > 0) {
            m_dropDetectorLockout -= delta;
        } else if (m_dropDetectorEnabled) {
            if (dropDetectValue > (1.1f - m_dropDetectorSens)) change = true;
            if (change) {
                float lockout = 10;
                float bpm = 120; // todo
                float secondsPerBeat = 60.0f / bpm;
                lockout = 3.75f * secondsPerBeat;
                clampfp(lockout, 0.5f, 4.0f);
                m_dropDetectorLockout = lockout;
//                qDebug() << "drop detected, value " << int(dropDetectValue*100) << " cooldown is " << m_dropDetectorLockout;
                emit dropDetected();
            }
        }
    }
}

void ldDropDetector::setDropDetectEnabledValue(bool value)
{
    m_dropDetectorEnabled = value;
}

void ldDropDetector::setDropDetectSensValue(int value)
{
    m_dropDetectorSens = clampf(value / 100.0f, 0, 1);
}
