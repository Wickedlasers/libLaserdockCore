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

#include "ldCore/Filter/ldSoundLevelFilter.h"

#include <ldCore/ldCore.h>
#include <ldCore/Visualizations/MusicManager/ldMusicManager.h>
#include <ldCore/Helpers/Visualizer/ldLaserFilter.h>

ldSoundLevelFilter::ldSoundLevelFilter()
{
}

void ldSoundLevelFilter::process(Vertex &input)
{
    if(!m_enabled)
        return;

    bool isBorder = ldCore::instance()->musicManager()->soundLevel() < m_borderValue;
    if(isBorder) {
        input.clear();
    }
}

bool ldSoundLevelFilter::enabled() const
{
    return m_enabled;
}

void ldSoundLevelFilter::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

void ldSoundLevelFilter::setBorderValue(int value)
{
    m_borderValue = value;
}

