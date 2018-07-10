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

