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

#include "ldCore/Render/ldAbstractRenderer.h"

#include <QtDebug>

/*!
  \class ldAbstractRenderer
  \brief Abstract class for a renderer.
  \inmodule rendering
*/

const int ldAbstractRenderer::DEFAULT_RATE = 30000;

/*!
    \brief overloaded constructor, with additional parameter specifying the type.
*/
ldAbstractRenderer::ldAbstractRenderer(QObject *parent) :
    QObject(parent)
{
}

void ldAbstractRenderer::setRate(int rate)
{
    if(!(rate == 20000 || rate == 30000 || rate == 35000 || rate == 40000)) {
        qWarning() << "Wrong rate value " << rate;
        return;
    }

    m_rate = rate;
}

int ldAbstractRenderer::rate() const
{
    return m_rate;
}

