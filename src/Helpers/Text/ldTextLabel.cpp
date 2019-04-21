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

// ldTextLabel.cpp
// Created by Eric Brugère on 10/dec/16.
// Copyright (c) 2016 Wicked Lasers. All rights reserved.

#include "ldCore/Helpers/Text/ldTextLabel.h"

#include <QtCore/QDebug>
#include "ldCore/Helpers/Draw/ldBezierCurveDrawer.h"
#include "ldCore/Helpers/SVG/ldSvgReader.h"
#include "ldCore/Helpers/Maths/ldMaths.h"

// ldTextLabel
ldTextLabel::ldTextLabel(const QString &text, float fontSize, const ldVec2 &p_position)
    : ldAbstractText(text, fontSize)
    , m_position(p_position)
    , m_drawer(new ldBezierCurveDrawer)
{
    m_drawer->setMaxPoints(30);
}

// ~ldTextLabel
ldTextLabel::~ldTextLabel()
{
}

// updateString
void ldTextLabel::setText(const QString &text)
{
    ldAbstractText::setText(text);

    initTextFrame(text);
}

void ldTextLabel::clear()
{
    setText("");
}

// setPosition
void ldTextLabel::setPosition(const ldVec2 &p_p)
{
    m_textFrame.translate(p_p - m_position);

    m_position = p_p;
}

ldVec2 ldTextLabel::getPosition() const
{
    return m_position;
}

// setIncrementXPositionOrLoop
bool ldTextLabel::setIncrementXPositionOrLoop(float delta)
{
    //qDebug() << _position.x << " this->getWidth()" << this->getWidth();

    float x = (m_position.x < -getWidth())
            ? 1.f
            : m_position.x - delta;

    setPosition(ldVec2(x, m_position.y));

    return m_position.x != 1.f;
}

// getColor
uint32_t ldTextLabel::getColor() const
{
    return m_color;
}

// setColor
void ldTextLabel::setColor(uint32_t p_color)
{
    m_color = p_color;
    m_textFrame.colorize(m_color);
}

// innerDraw
void ldTextLabel::innerDraw(ldRendererOpenlase* p_renderer)
{
    m_drawer->innerDraw(p_renderer, m_textFrame);
}

std::vector<std::vector<OLPoint> > ldTextLabel::getDrawingData() const
{
    return m_drawer->getDrawingData(m_textFrame);
}

void ldTextLabel::initTextFrame(const QString &word)
{
    ldAbstractText::initTextFrame(word);

    m_textFrame.translate(m_position);
    m_textFrame.colorize(m_color);
}
