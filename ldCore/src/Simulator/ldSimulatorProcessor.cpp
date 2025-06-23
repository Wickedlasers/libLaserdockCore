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

#include <cmath>

#include "ldSimulatorProcessor.h"

ldSimulatorProcessor::ldSimulatorProcessor()
{    
    m_last.clear();
    m_lastOn.clear();
}

ldSimulatorProcessor::~ldSimulatorProcessor()
{
//    qDebug() << "~ldSimulatorProcessor()";
}

void ldSimulatorProcessor::bigger_dots(ldVertex* inData, ldVertex* outData, unsigned int size, bool isLastPortion) {

    const float mindist = 0.01f; // force lines to have this min length

    for (uint i = 0; i < size; i++) {

        outData[i] = inData[i]; // initial value

        // color logic
        bool isOn = !inData[i].isBlank();

        if (isOn) {
            // remember color of last nonblack point
            m_lastOn = inData[i];

            // distance calcs
            float dx = (outData[i].x() - m_last.x());
            float dy = (outData[i].y() - m_last.y());
            float delta2 = dx*dx + dy*dy;
            float delta = sqrtf(delta2);
            m_moveDist += delta;
            if (delta >= mindist) {
                // get a vector of length mindist, pointed in last direction laser moved
                m_lastDeltaX = dx * mindist / delta;
                m_lastDeltaY = dy * mindist / delta;
            }
        }

        // check for end of a line, make changes to the points here
        if (m_wasOn && !isOn) {

            // force last point to be a color
            //data2[i] = laston;
            outData[i].r() = m_lastOn.r();
            outData[i].g() = m_lastOn.g();
            outData[i].b() = m_lastOn.b();

            // force line length to be a minimum
            if (m_moveDist < mindist) {
                // if it's the first dot and there is no more content we should move it at least a bit
                if(m_lastDeltaX == 0 && m_lastDeltaY == 0)
                    m_lastDeltaX = mindist;

                outData[i].x() += m_lastDeltaX;
                outData[i].y() += m_lastDeltaY;
            }
        }

        // check for start of a new line
        if (!m_wasOn && isOn) {
            m_moveDist = 0;
        }

        // save values for next point's delta calcs
        if(isOn)
            m_last = inData[i];

        m_wasOn = isOn;
    }

    // in the end of frame if the last is on we should make it visible anyway
    if(isLastPortion
        && m_wasOn
        && m_moveDist < mindist
        && size > 0
        ) {
        // check just in case, should never happen, see the previous similar block
        if(m_lastDeltaX == 0 && m_lastDeltaY == 0)
            m_lastDeltaX = mindist;

        // ok, make last point big dot
        uint i = size - 1;
        outData[i].x() += m_lastDeltaX;
        outData[i].y() += m_lastDeltaY;
    }
}

void ldSimulatorProcessor::clear()
{
    m_last.clear();
    m_lastOn.clear();
    m_lastDeltaX = 0;
    m_lastDeltaY = 0;
    m_wasOn = false;
    m_moveDist = 0;
}

