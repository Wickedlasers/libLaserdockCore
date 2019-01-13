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

// ld3dTextLabelOne.cpp
// Created by Eric Brugère on 12/jan/17.
// Copyright (c) 2017 Wicked Lasers. All rights reserved.

#include "ldCore/Helpers/Text/ld3dTextLabelOne.h"

#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <ldCore/Helpers/BezierCurve/ldBezierCurveFrame.h>

#include "ldCore/Helpers/BezierCurve/ldBezierCurveObject.h"
#include "ldCore/Helpers/SVG/ldSvgReader.h"


// ld3dTextLabelOne
ld3dTextLabelOne::ld3dTextLabelOne(const QString& p_string, float p_fontSize)
    : ldAbstractText()
    , _drawer(new ld3dBezierCurveDrawer)
{
    //setText(p_string);
    m_string = p_string;
    setFontSize(p_fontSize);
}

// ~ld3dTextLabelOne
ld3dTextLabelOne::~ld3dTextLabelOne()
{
}

// setModeRotate
void ld3dTextLabelOne::setModeRotate(bool mode_rotate)
{
    m_mode_rotate = mode_rotate;
}

// innerDraw
void ld3dTextLabelOne::innerDraw(ldRendererOpenlase* p_renderer)
{
    bool test = false;
    if (m_mode_rotate) test = _drawer->innerDrawTwo(p_renderer, m_rotate_step);
    else test = _drawer->innerDraw(p_renderer);
    if (!test) {
        _currentIndice++;
        if(_currentIndice >= _words.size()) {
            _currentIndice = 0;
        }

        m_rotate_step++;
        if (m_rotate_step>3)m_rotate_step=0;

        updateCurrentString();
    }
}

void ld3dTextLabelOne::setSpeedCoeff(float speedCoeff)
{
   _drawer->setSpeedCoeff(speedCoeff);
}

// setText
void ld3dTextLabelOne::setText(const QString &p_string)
{
    m_string = p_string;

    _currentIndice = 0;

    //
    QString qString = p_string;
    qString = qString.replace("\n" , "");
    _words = qString.split(" ");

    //
    updateCurrentString();
}

void ld3dTextLabelOne::updateCurrentString()
{
    if(_currentIndice >= _words.size()) {
        return;
    }

    initLabelLettersFrame(_words[_currentIndice]);

    // recenter
    _labelLettersFrame.moveToCenter();

    // detect new properties
    _drawer->setFrame(_labelLettersFrame.to3d());
}
