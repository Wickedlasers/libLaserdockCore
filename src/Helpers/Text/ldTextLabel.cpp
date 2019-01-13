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
ldTextLabel::ldTextLabel(const QString& p_string, float p_fontSize, const Vec2 &p_position)
    : ldAbstractText()
    , _color(0xFFFFFF)
    , _drawer(new ldBezierCurveDrawer)
{
    _position = p_position;

    setFontSize(p_fontSize);
    this->setText(p_string);

    _drawer->setMaxPoints(30);
}

// ~ldTextLabel
ldTextLabel::~ldTextLabel()
{
}

// updateString
void ldTextLabel::setText(const QString &p_string)
{
    m_string = p_string;

    initLabelLettersFrame(p_string);

    _labelLettersFrame.translate(_position);
    _labelLettersFrame.colorize(_color);
}

void ldTextLabel::clear()
{
    setText("");
}

QString ldTextLabel::getText() const
{
    return m_string;
}

// setPosition
void ldTextLabel::setPosition(const Vec2 &p_p)
{
    _labelLettersFrame.translate(p_p - _position);

    _position = p_p;
}

Vec2 ldTextLabel::getPosition() const
{
    return _position;
}

// setIncrementXPositionOrLoop
bool ldTextLabel::setIncrementXPositionOrLoop(float delta)
{
    //qDebug() << _position.x << " this->getWidth()" << this->getWidth();

    float x = 0.f;
    if (_position.x < -getWidth()) {
        x = 1.f;
    } else {
        x = _position.x - delta;
    }
    //
    setPosition(Vec2(x, _position.y));
    return _position.x != 1.f;
}

// getColor
uint32_t ldTextLabel::getColor() const
{
    return _color;
}

// setColor
void ldTextLabel::setColor(uint32_t p_color)
{
    _color = p_color;
    _labelLettersFrame.colorize(_color);
}

// innerDraw
void ldTextLabel::innerDraw(ldRendererOpenlase* p_renderer)
{
    _drawer->innerDraw(p_renderer, _labelLettersFrame);
}

std::vector<std::vector<OLPoint> > ldTextLabel::getDrawingData() const
{
    return _drawer->getDrawingData(_labelLettersFrame);
}
