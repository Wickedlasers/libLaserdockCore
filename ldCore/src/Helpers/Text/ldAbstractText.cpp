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
//  ldAbstractText.cpp
//
//  Created by Eric Brugère on 09/20/17.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//
#include "ldCore/Helpers/Text/ldAbstractText.h"

#include "ldCore/ldCore.h"
#include "ldCore/Helpers/SVG/ldSvgReader.h"
#include "ldCore/Helpers/Text/ldTextSvgHelper.h"
#include "ldCore/Helpers/Text/ldSvgFontManager.h"

// ldAbstractText()
ldAbstractText::ldAbstractText(const QString &text, float fontSize, const ldVec2 &p_position)
    : m_text(text)
    , m_fontSize(fontSize)
    , m_position(p_position)
{
    reloadAllSvgLetters();
}

// ~ldAbstractText()
ldAbstractText::~ldAbstractText()
{
    //
}

void ldAbstractText::setText(const QString &p_string)
{
    m_text = p_string;
}

QString ldAbstractText::getText() const
{
    return m_text;
}

// setText to override
// void ldAbstractText::setText(const std::string &p_string) { }

// setFontSize
void ldAbstractText::setFontSize(float fontSize)
{
    m_fontSize = fontSize;
    reloadAllSvgLetters();
}

// getFontSize
float ldAbstractText::getFontSize() const
{
    return m_fontSize;
}

// setFont
void ldAbstractText::setFont(int font)
{
    m_fontFamily = font;
    reloadAllSvgLetters();
}

// font
int ldAbstractText::font() const
{
    return m_fontFamily;
}

double ldAbstractText::letterSpace() const
{
    return m_letterSpaceScale;
}

void ldAbstractText::setLetterSpace(double letterSpace)
{
    m_letterSpaceScale = letterSpace;
    setText(m_text);
}

// setPosition
void ldAbstractText::setPosition(const ldVec2 &p_p)
{
    m_textFrame.translate(p_p - m_position);

    m_position = p_p;
}

ldVec2 ldAbstractText::getPosition() const
{
    return m_position;
}

const ldBezierCurveFrame &ldAbstractText::getFrame() const
{
    return m_textFrame;
}

// getWidth
float ldAbstractText::getWidth() const
{
    return m_textFrame.dim().width();
}

// getHeight
float ldAbstractText::getHeight() const
{
    return m_textFrame.dim().height();
}


void ldAbstractText::initTextFrame(const QString &word)
{
    m_textFrame.clear();

    // here in _allSvgLetters
    const float interLetter = getInterLetterWidth();
    const float spaceWidth = getSpaceWidth();

    // _labelLetters build
    QString qString = word;
    float letterOffsetX = 0.f;
    float letterOffsetY = 0.f;

    for (const QChar &strChar : qString) {
        if (strChar == ' ') {
            letterOffsetX += spaceWidth + interLetter;
            continue;
        } else if (strChar == '\t') {
            letterOffsetX += 3*spaceWidth + interLetter;
            continue;
        } else if (strChar == '\n') {
            letterOffsetX = 0;
            letterOffsetY += m_fontSize*1.4;
            continue;
        }

        int index = ldTextSvgHelper::instance()->indexForSvgValidChars(strChar, m_fontFamily);
        if (index==-1) index = (int)m_allSvgLetters.size()-1;

        ldBezierCurveObject letterObject = m_allSvgLetters[index].data();
        // move letter to position in word
        letterObject.translate(ldVec2(letterOffsetX, -letterOffsetY));

        m_textFrame.add(letterObject);
        // next letter offset
        letterOffsetX += letterObject.dim().width() + interLetter;
    }
}

// getLetterAWidth
float ldAbstractText::getLetterAWidth()
{
    int a_upper_index = ldTextSvgHelper::instance()->indexForSvgValidChars('A', m_fontFamily);
    // should not happen
    if (a_upper_index < 0 || a_upper_index >= (int) m_allSvgLetters.size())
        return 0.10f;
    //
    ldBezierCurveObject a_upper_svg = m_allSvgLetters[a_upper_index].data();
    float width = a_upper_svg.dim().width();
    if(cmpf(width, 0))
        return 0.10f;

    return width;
}

// getInterLetterWidth
float ldAbstractText::getInterLetterWidth()
{
    float interLetterSuffix = ldCore::instance()->svgFontManager()->font(m_fontFamily).interLetterSuffix();
    float interLetter = interLetterSuffix*getLetterAWidth();

    return interLetter * m_letterSpaceScale;
}

// getSpaceWidth
float ldAbstractText::getSpaceWidth()
{
    const float space = 0.8f*getLetterAWidth();
    //
    return space;
}

void ldAbstractText::reloadAllSvgLetters()
{
    m_allSvgLetters = ldTextSvgHelper::instance()->resizedSvgLetters(m_fontSize, m_fontFamily);

    setText(m_text);
}


