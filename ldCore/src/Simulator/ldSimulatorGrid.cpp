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

#include "ldCore/Simulator/ldSimulatorGrid.h"

#include <cmath>

#include <QtGui/QOpenGLShaderProgram>

#include <ldCore/Filter/ldBasicFilters.h>
#include "ldSimulatorProcessor.h"


ldSimulatorGrid::ldSimulatorGrid()
    : m_rotateFilter(new ld3dRotateFilter())
{
    int rowCount = 8;
    float color = 0.2f;
    float brightColor = color * 2.f;

    auto isCenter = [&](int i) {
        return i == rowCount / 2;
    };
    auto isTopLeft = [&](int i) {
        return i == rowCount / 4;
    };

    for(int i = 0 ; i < rowCount+1; i++) {
        float startX = -1;
        float endX = 1;
        float y = i/static_cast<float>(rowCount)*2 - 1;

        float c1 = isCenter(i) || isTopLeft(rowCount - i) ? brightColor : color;
        float c2 = isCenter(i) ? brightColor : color;

        m_initialGrid.push_back(ldVertex{{startX,y}, {0,0,0}});
        m_initialGrid.push_back(ldVertex{{startX,y}, {c1,c1,c1}});
        m_initialGrid.push_back(ldVertex{{0,y}, {c1,c1,c1}});
        m_initialGrid.push_back(ldVertex{{0,y}, {c2,c2,c2}});
        m_initialGrid.push_back(ldVertex{{endX,y}, {c2,c2,c2}});
        m_initialGrid.push_back(ldVertex{{endX,y}, {0,0,0}});

    }
    for(int i = 0 ; i < rowCount+1; i++) {
        float startY = -1;
        float endY = 1;
        float x = i/static_cast<float>(rowCount)*2 - 1;

        float c1 = isCenter(i) ? brightColor : color;
        float c2 = isCenter(i) || isTopLeft(i) ? brightColor : color;

        m_initialGrid.push_back(ldVertex{{x,startY}, {0,0,0}});
        m_initialGrid.push_back(ldVertex{{x,startY}, {c1,c1,c1}});
        m_initialGrid.push_back(ldVertex{{x,0}, {c1,c1,c1}});
        m_initialGrid.push_back(ldVertex{{x,0}, {c2,c2,c2}});
        m_initialGrid.push_back(ldVertex{{x,endY}, {c2,c2,c2}});
        m_initialGrid.push_back(ldVertex{{x,endY}, {0,0,0}});
    }
    m_grid = m_initialGrid;

}

ldSimulatorGrid::~ldSimulatorGrid()
{
    //    qDebug() << "~ldSimulatorGrid()";
}

bool ldSimulatorGrid::isEnabled()
{
    return  m_isEnabled;
}

void ldSimulatorGrid::setEnabled(bool isGridEnabled)
{
    m_isEnabled = isGridEnabled;
}

void ldSimulatorGrid::setRotX(float x)
{
    m_rotateFilter->setX(x);
    updateBuffer();
}

void ldSimulatorGrid::setRotY(float y)
{
    m_rotateFilter->setY(y);
    updateBuffer();
}

const std::vector<ldVertex> &ldSimulatorGrid::buffer() const
{
    return m_grid;
}

void ldSimulatorGrid::updateBuffer()
{
    m_grid = m_initialGrid;
    for(ldVertex &v : m_grid)
        m_rotateFilter->processFilter(v);
}


