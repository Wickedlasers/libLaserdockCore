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
#include <ldCore/Helpers/Draw/ldBezierCurveDrawer.h>
#include "ldCore/Helpers/SVG/ldSvgReader.h"
#include "ldCore/Helpers/Text/ldTextSvgHelper.h"
#include "ldCore/Helpers/Text/ldSvgFontManager.h"

// ldAbstractText()
ldAbstractText::ldAbstractText(const QString &text, float fontSize, const ldVec2 &p_position)
    : m_text(text)
    , m_fontSize(fontSize)
    , m_position(p_position)
{
    m_fontFamily = ldCore::instance()->svgFontManager()->defaultFont().title();
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
void ldAbstractText::setFont(const QString &fontFamily)
{
    m_fontFamily = fontFamily;
    reloadAllSvgLetters();
}

// font
QString ldAbstractText::font() const
{
    return m_fontFamily;
}

float ldAbstractText::letterSpace() const
{
    return m_letterSpaceScale;
}

void ldAbstractText::setLetterSpace(float letterSpace)
{
    m_letterSpaceScale = letterSpace;
    setText(m_text);
}

void ldAbstractText::setVertical(bool isVertical)
{
    m_isVertical = isVertical;
    setText(m_text);
}

bool ldAbstractText::isVertical() const
{
    return m_isVertical;
}

// setPosition
void ldAbstractText::setPosition(const ldVec2 &p_p)
{
    if(!m_textFrame.data().empty())
        m_textFrame.translate(p_p - m_position);

    if(!m_textPointObject.data().empty())
        m_textPointObject.translate(ldVec2(p_p - m_position)*2.f);

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

ldOLPointObject ldAbstractText::getOLObject() const
{
    return ldOLPointObject(ldBezierCurveDrawer().getDrawingData(getFrame()));
}

// getWidth
float ldAbstractText::getWidth() const
{
    if(!m_textFrame.data().empty())
        return m_textFrame.dim().width();

    if(!m_textPointObject.data().empty())
        return m_textPointObject.dim().width()/2.f;

    return 0.f;
}

// getHeight
float ldAbstractText::getHeight() const
{
    if(!m_textFrame.data().empty())
        return m_textFrame.dim().height();

    if(!m_textPointObject.data().empty())
        return m_textPointObject.dim().height()/2.f;

    return 0.f;
}

ldVec2 ldAbstractText::getFirstLetterSize()
{
    return getLetterSize(m_text.isEmpty() ? 'A' : m_text.at(0));
}

void ldAbstractText::cloneProperties(ldAbstractText *other) const
{
    other->setText(getText());
    other->setFont(font());
    other->setLetterSpace(letterSpace());
    other->setVertical(isVertical());
    other->setFontSize(getFontSize());
    other->setPosition(getPosition());
}

#include <QPainterPath>
#include <QTextLayout>

ldOLPointObject ldAbstractText::generatePointObject(const QString &text, const QString &fontFamily, float fontSize, float letterSpace, bool isVertical) {
//    qDebug() << __FUNCTION__ << text << fontFamily << fontSize << letterSpace << isVertical;

    ldOLPointObject result;

    QTextLayout textLayout(text);


#if QT_VERSION >= 0x060000
    if(QFontDatabase::hasFamily(fontFamily)) {
#else
    if(QFontDatabase().hasFamily(fontFamily)) {
#endif
        QFont font(fontFamily, fontSize*130.f); // magic number to have size similar to internal one
//        letterSpace =  ldMaths::adjustToRange(letterSpace, 0.5, 1, 5, 0.85, 1.08, 2.9);
//        font.setLetterSpacing(QFont::PercentageSpacing, letterSpace*100.f);
        if(!isVertical)
            letterSpace = ldMaths::adjustToRange(letterSpace, 0.5, 1, 5, -1, 0.7, 15); // magic numbers again ✨, see ldFontControl. 2.76 = 0
        else
            letterSpace = ldMaths::adjustToRange(letterSpace, 0.5, 1, 5, 0.1, 0.4, 3); // magic numbers again ✨, see ldFontControl. 2.76 = 0
        font.setLetterSpacing(QFont::AbsoluteSpacing, letterSpace);
        textLayout.setFont(font);
    } else {
        qWarning() << __FUNCTION__ << "can't find font family" << fontFamily << text;
    }

    textLayout.beginLayout();
    while(true) {
        QTextLine textLine = textLayout.createLine();
        if(!textLine.isValid())
            break;
    }
    textLayout.endLayout();

    auto glyphRuns = textLayout.glyphRuns();
//    if(glyphRuns.empty()) {
//        qWarning() << fontFamily << "no glyph runs";
//        return result;
//    }

    float xOffset = 0;
    float yOffset = 0;

    for(auto glyphRun : glyphRuns) {
        auto rawFont = glyphRun.rawFont();
//        if(!rawFont.isValid()) {
//            qWarning() << fontFamily << "raw font is not valid";
//            continue;
//        }

//        if(glyphRun.glyphIndexes().empty()) {
//            qWarning() << fontFamily << "no glyph indexes";
//            continue;
//        }

//        qDebug() << text << glyphRun.boundingRect() << glyphRuns.size();

        for(int i = 0; i < glyphRun.glyphIndexes().size(); i++) {
            float xLetterOffset = 0;
            float yLetterOffset = 0;

            auto glyphIndex = glyphRun.glyphIndexes()[i];
            auto position = glyphRun.positions()[i];
//            qDebug() << position;
//            qDebug() << glyphIndex;
            auto painterPath = rawFont.pathForGlyph(glyphIndex);

            auto polygons = painterPath.toSubpathPolygons();

            for(auto polygon : polygons) {

//                if(polygon.empty()) {
//                    qWarning() << fontFamily << "no polygon points";
//                    continue;
//                }

//                qDebug() << polygon;

                for(auto point : polygon) {
//                    qDebug() << point;

                    auto x = static_cast<float>(point.x() / 50.);
                    auto y = static_cast<float>(point.y() / -50.);

                    yLetterOffset = y > yLetterOffset
                            ? y
                            : yLetterOffset;
                    xLetterOffset = x > xLetterOffset
                            ? x
                            : xLetterOffset;
                }
            }

            for(auto polygon : polygons) {

//                if(polygon.empty()) {
//                    qWarning() << fontFamily << "no polygon points";
//                    continue;
//                }

//                qDebug() << polygon;

                PointVector pv;

                for(auto point : polygon) {
//                    qDebug() << point;

                    auto x = static_cast<float>(point.x() / 50.);
                    auto y = static_cast<float>(point.y() / -50.);

                    if(isVertical) {
                        xOffset = static_cast<float>(getLetterSize('W').x - xLetterOffset/2);

//                        yOffset = yLetterOffset;
                    } else {
                        xOffset = static_cast<float>(position.x()/50.);
                    }


                    pv.push_back(OLPoint{x + xOffset,
                                         y + yOffset,
                                         0.f,
                                         C_WHITE});

                }

                // do not add 1 dot vector
                if(pv.size() > 1)
                    result.add(pv);
            }




            if(isVertical)
                yOffset -= yLetterOffset + yLetterOffset*letterSpace;
        }


    }


    return result;
}



void ldAbstractText::initTextFrame(const QString &word)
{
//    qDebug() << __FUNCTION__ << word;
    m_textFrame.clear();
    m_textPointObject.clear();

    ldSvgFont svgFont =  ldCore::instance()->svgFontManager()->font(m_fontFamily);
    if(svgFont.isInternalFont()) {
        const float interLetter = getInterLetterWidth();
        const float spaceWidth = getSpaceWidth();

        float letterOffsetX = 0.f;
        float letterOffsetY = 0.f;
        for (const QChar &strChar : word) {
            if (strChar == ' ') {
                if(m_isVertical)
                    letterOffsetY += spaceWidth + interLetter;
                else
                    letterOffsetX += spaceWidth + interLetter;
                continue;
            } else if (strChar == '\t') {
                if(m_isVertical)
                    letterOffsetY += 3*spaceWidth + interLetter;
                else
                    letterOffsetX += 3*spaceWidth + interLetter;
                continue;
            } else if (strChar == '\n') {
                if(m_isVertical) {
                    letterOffsetY += 5*spaceWidth + interLetter;
                } else {
                    letterOffsetX = 0;
                    letterOffsetY += m_fontSize*1.4;
                }
                continue;
            }

            int index = ldTextSvgHelper::instance()->indexForSvgValidChars(strChar, m_fontFamily);
            if (index==-1) index = (int)m_allSvgLetters.size()-1;

            ldBezierCurveObject letterObject = m_allSvgLetters[index].data();

            // center each letter in vertical alignment
            if(m_isVertical)
                letterOffsetX = getLetterSize('W').x/2 - letterObject.dim().width()/2;

            // move letter to position in word
            float yOffset = m_isVertical ? -getFirstLetterSize().y : 0;
            letterObject.translate(ldVec2(letterOffsetX, yOffset-letterOffsetY));

            m_textFrame.add(letterObject);
            if(m_isVertical) {
                letterOffsetY += letterObject.dim().height() + interLetter;
            } else {
                letterOffsetX += letterObject.dim().width() + interLetter;
            }
        }

        if(m_isVertical)
            m_textFrame.translate(ldVec2(0, getHeight()));

    } else {
        if(m_isVertical) {
            const float interLetter = getInterLetterWidth();
            const float spaceWidth = getSpaceWidth();

            float letterOffsetX = 0.f;
            float letterOffsetY = 0.f;
            for (const QChar &strChar : word) {
                if (strChar == ' ') {
                    letterOffsetY += spaceWidth + interLetter;
                    continue;
                } else if (strChar == '\t') {
                    letterOffsetY += 3*spaceWidth + interLetter;
                    continue;
                } else if (strChar == '\n') {
                    letterOffsetY += 5*spaceWidth + interLetter;
                    continue;
                }

                ldOLPointObject letterObject = generatePointObject(strChar, ldCore::instance()->svgFontManager()->font(m_fontFamily).title(), m_fontSize, m_letterSpaceScale, m_isVertical);

                letterOffsetY += letterObject.dim().height();

                letterObject.translate(ldVec2(letterOffsetX, -letterOffsetY));

                letterOffsetY += interLetter;

                for(auto data : letterObject.data())
                    m_textPointObject.add(data);

            }

            m_textPointObject.translate(ldMaths::unitedToLaserCoords(ldVec2(0, getHeight())));

//            ldOLPointObject letterObject = generatePointObject(word,  ldCore::instance()->svgFontManager()->font(m_fontFamily).title(), m_fontSize, m_letterSpaceScale, m_isVertical);
//            float yOffset = m_isVertical ? letterObject.dim().height()/2.f
//                                           -getFirstLetterSize().y : 0;
//            letterObject.translate(ldMaths::unitedToLaserCoords(ldVec2(0, yOffset)));
//            m_textPointObject = letterObject;
        } else {
            ldOLPointObject letterObject = generatePointObject(word,  ldCore::instance()->svgFontManager()->font(m_fontFamily).title(), m_fontSize, m_letterSpaceScale, m_isVertical);
            letterObject.translate(ldMaths::unitedToLaserCoords(ldVec2(0, 0)));
            m_textPointObject = letterObject;

        }
    }
}

ldVec2 ldAbstractText::getLetterSize(const QChar &ch)
{
    ldVec2 res(0.1f, 0.1f);

    ldSvgFont svgFont =  ldCore::instance()->svgFontManager()->font(m_fontFamily);
    if(svgFont.isInternalFont()) {
        int a_upper_index = ldTextSvgHelper::instance()->indexForSvgValidChars(ch, m_fontFamily);
        // should not happen
        if (a_upper_index < 0 || a_upper_index >= (int) m_allSvgLetters.size())
            return res;
        //
        ldBezierCurveObject a_upper_svg = m_allSvgLetters[a_upper_index].data();

        res = a_upper_svg.dim().size();
    } else {
        res = generatePointObject(ch, svgFont.title(), m_fontSize).dim().size()/2.f;
    }

    if(cmpf(res.x, 0))
        res.x = 0.1f;
    if(cmpf(res.y, 0))
        res.y = 0.1f;

    return res;
}

// getInterLetterWidth
float ldAbstractText::getInterLetterWidth()
{
    float interLetterSuffix = ldCore::instance()->svgFontManager()->font(m_fontFamily).interLetterSuffix();
    float interLetter = interLetterSuffix*getLetterSize('A').x;

    float letterSpace = m_letterSpaceScale;
    if(m_isVertical)
        letterSpace = ldMaths::adjustToRange(letterSpace, 0.5, 1, 5, 1., 2, 3); // magic numbers again ✨, see ldFontControl. 2.76 = 0

    return interLetter * letterSpace;
}

// getSpaceWidth
float ldAbstractText::getSpaceWidth()
{
    const float space = 0.8f*getLetterSize('A').x;
    //
    return space;
}

void ldAbstractText::reloadAllSvgLetters()
{
    ldSvgFont svgFont =  ldCore::instance()->svgFontManager()->font(m_fontFamily);
    if(svgFont.isInternalFont())
        m_allSvgLetters = ldTextSvgHelper::instance()->resizedSvgLetters(m_fontSize, m_fontFamily);

    setText(m_text);
}


