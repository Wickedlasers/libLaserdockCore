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
ld3dTextLabelOne::ld3dTextLabelOne(const QString& text, float fontSize)
    : ldAbstractText(text, fontSize)
    , m_drawer(new ld3dBezierCurveDrawer)
{
}

// ~ld3dTextLabelOne
ld3dTextLabelOne::~ld3dTextLabelOne()
{
}

void ld3dTextLabelOne::setText(const QString &text)
{
    ldAbstractText::setText(text);
    m_currentIndex = 0;

    QString wordsText = text;
    wordsText = wordsText.replace("\n" , "");
    m_words = wordsText.split(" ");

    if(m_currentIndex <= m_words.size())
        initTextFrame(m_words[m_currentIndex]);
}

void ld3dTextLabelOne::innerDraw(ldRendererOpenlase* p_renderer)
{
    bool isInProgress = m_mode_rotate
            ? m_drawer->innerDrawTwo(p_renderer, m_rotate_step)
            : m_drawer->innerDraw(p_renderer);

    if (!isInProgress) {
        m_currentIndex++;
        if(m_currentIndex >= m_words.size()) {
            m_currentIndex = 0;
        }

        if(m_mode_rotate) {
            m_rotate_step++;
            if (m_rotate_step > 3)
                m_rotate_step = 0;
        }

        if(m_currentIndex <= m_words.size())
            initTextFrame(m_words[m_currentIndex]);
    }
}

void ld3dTextLabelOne::setSpeedCoeff(float speedCoeff)
{
   m_drawer->setSpeedCoeff(speedCoeff);
}

void ld3dTextLabelOne::setModeRotate(bool mode_rotate)
{
    m_mode_rotate = mode_rotate;
}

void ld3dTextLabelOne::setPosition(const ldVec2 &p_p)
{
    if(getPosition().isNull()) {
        m_textFrame.moveTo(getPosition());
    }
    ldAbstractText::setPosition(p_p);
    m_drawer->setFrame(m_textFrame.to3d());
}

void ld3dTextLabelOne::initTextFrame(const QString &word)
{
    ldAbstractText::initTextFrame(word);
    // recenter
    if(getPosition().isNull())
        m_textFrame.moveToCenter();
    else
        m_textFrame.translate(getPosition());
    // set to drawer
    m_drawer->setFrame(m_textFrame.to3d());
}
