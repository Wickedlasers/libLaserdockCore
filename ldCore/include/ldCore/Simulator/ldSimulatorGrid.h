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

#ifndef LDSIMULATORGRID_H
#define LDSIMULATORGRID_H

#include <memory>

#include <ldCore/ldCore_global.h>
#include <ldCore/Utilities/ldVertex.h>

class ld3dRotateFilter;

/** Simulator grid class */
class LDCORESHARED_EXPORT ldSimulatorGrid
{
public:
    explicit ldSimulatorGrid();
    virtual ~ldSimulatorGrid();

    bool isEnabled();
    void setEnabled(bool isGridEnabled);

    void setRotX(float x);
    void setRotY(float y);

    const std::vector<ldVertex> &buffer() const;
private:
    void updateBuffer();

    bool m_isEnabled = false;
    std::unique_ptr<ld3dRotateFilter> m_rotateFilter;
    std::vector<ldVertex> m_initialGrid;
    std::vector<ldVertex> m_grid;
};

#endif // ldSimulatorGrid_H


