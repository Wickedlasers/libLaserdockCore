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

//
//  ldSquareVisualizer.cpp
//  ldCore
//
//  Created by Sergey Gavrushkin on 25/10/17.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#include "ldSquareVisualizer.h"

ldSquareVisualizer::ldSquareVisualizer()
    : ldVisualizer()
{
    m_rate = 20000;
    init();
}

ldSquareVisualizer::~ldSquareVisualizer() {}

bool ldSquareVisualizer::init()
{
    if (ldVisualizer::init())
    {
        setPosition(ccp(1, 1));
        return true;
    }
    return false;
}



void ldSquareVisualizer::draw()
{
    m_renderer->begin(OL_LINESTRIP);

    const float MAX_VAL = 0.99f;

    m_renderer->vertex(-1. * MAX_VAL, -1. * MAX_VAL, C_WHITE, 3);
    m_renderer->vertex(-1. * MAX_VAL, 1. * MAX_VAL, C_WHITE, 6);
    m_renderer->vertex(1. * MAX_VAL, 1. * MAX_VAL, C_WHITE, 6);
    m_renderer->vertex(1. * MAX_VAL, -1. * MAX_VAL, C_WHITE, 6);
    m_renderer->vertex(-1. * MAX_VAL, -1. * MAX_VAL, C_WHITE, 3);

    m_renderer->end();
}

