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

#ifndef LDSIMULATORPROCESSOR_H
#define LDSIMULATORPROCESSOR_H

#include "ldCore/Utilities/ldVertex.h"

class ldSimulatorProcessor
{
public:
    explicit ldSimulatorProcessor();
    virtual ~ldSimulatorProcessor();

    // function for altering laser data to make points bigger before being sent to simulator
    void bigger_dots(ldVertex *inData, ldVertex *outData, unsigned int size, bool isLastPortion);

    void clear();

private:
    ldVertex m_last;
    ldVertex m_lastOn;
    float m_lastDeltaX = 0;
    float m_lastDeltaY = 0;
    bool m_wasOn = false;
    float m_moveDist = 0;
};

#endif // LDSIMULATORPROCESSOR_H


