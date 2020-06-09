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

#include "ldCore/Filter/ldFilterBasicData.h"


#include <QtCore/QDebug>

#include "ldCore/Filter/ldFilterBasicGlobal.h"

ldFilterBasicData::ldFilterBasicData()
    : ldFilter()
    , m_rotateFilter(new ldRotateFilter())
    , m_scaleFilter(new ldScaleFilter())
{

}

ldFilterBasicData::~ldFilterBasicData()
{
}


void ldFilterBasicData::process(ldVertex &v) {
    
    bool mode_disable_scale = frameModes & FRAME_MODE_SKIP_TRANSFORM;

    if (!mode_disable_scale) {
        m_rotateFilter->processFilter(v);
        m_scaleFilter->processFilter(v);
    }
}


ldScaleFilter *ldFilterBasicData::scaleFilter() const
{
    return m_scaleFilter.get();
}

ldRotateFilter *ldFilterBasicData::rotateFilter() const
{
    return m_rotateFilter.get();
}


