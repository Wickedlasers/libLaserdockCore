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

#include "ldCore/Helpers/Text/ldTextSvgHelper.h"
#include "ldCore/Helpers/SVG/ldSvgReader.h"

// ldAbstractText()
ldAbstractText::ldAbstractText()
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
    initLabelLettersFrame(p_string);
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
void ldAbstractText::setFont(const ldFont::Family &family)
{
    m_fontFamily = family;
    reloadAllSvgLetters();
}

// font
ldFont::Family ldAbstractText::font() const
{
    return m_fontFamily;
}

double ldAbstractText::letterSpace() const
{
    return m_letterSpace;
}

void ldAbstractText::setLetterSpace(double letterSpace)
{
    m_letterSpace = letterSpace;
    setText(m_string);
}

const ldBezierCurveFrame &ldAbstractText::getFrame() const
{
    return _labelLettersFrame;
}

// getWidth
float ldAbstractText::getWidth()
{
    return _labelLettersFrame.dim().width();
}

// getHeight
float ldAbstractText::getHeight()
{
    return _labelLettersFrame.dim().height();
}


void ldAbstractText::initLabelLettersFrame(const QString &word)
{
    _labelLettersFrame.clear();

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

        int index = ldTextSvgHelper::instance()->indexForSvgValidChars(strChar);
        if (index==-1) index = (int)m_allSvgLetters.size()-1;

        ldBezierCurveObject letterObject = m_allSvgLetters[index].data();
        // move letter to position in word
        letterObject.translate(Vec2(letterOffsetX, -letterOffsetY));

        _labelLettersFrame.add(letterObject);
        // next letter offset
        letterOffsetX += letterObject.dim().width() + interLetter;
    }
}

// getLetterAWidth
float ldAbstractText::getLetterAWidth()
{
    int a_upper_index = ldTextSvgHelper::instance()->indexForSvgValidChars('A');
    // should not happen
    if (a_upper_index >= (int)m_allSvgLetters.size()) return 0.10f;
    //
    ldBezierCurveObject a_upper_svg = m_allSvgLetters[a_upper_index].data();
    return a_upper_svg.dim().width();
}

// getInterLetterWidth
float ldAbstractText::getInterLetterWidth()
{
    float interLetter = 0.36f*getLetterAWidth();

    switch (m_fontFamily) {
    case ldFont::Family::Elixia:
         interLetter = getLetterAWidth();
        break;
    default:
        break;
    }

    return interLetter * m_letterSpace;
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

    setText(m_string);
}


