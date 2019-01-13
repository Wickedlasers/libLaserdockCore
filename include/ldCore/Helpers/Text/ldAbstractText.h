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
//  ldAbstractText.h
//
//  Created by Eric Brugère on 09/20/17.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#ifndef LDABSTRACTTEXT_H
#define LDABSTRACTTEXT_H

#include <string>

#include <QtCore/QObject>

#include "ldCore/Helpers/BezierCurve/ldBezierCurveFrame.h"
#include "ldFont.h"
#include "ldSvgLetter.h"

// ldAbstractText
class LDCORESHARED_EXPORT ldAbstractText : public QObject
{
    Q_OBJECT

public:
    ldAbstractText();
    ~ldAbstractText();

    virtual void setText(const QString &p_string);

    void setFontSize(float fontSize);
    float getFontSize() const;

    void setFont(const ldFont::Family &family);
    ldFont::Family font() const;

    double letterSpace() const;
    void setLetterSpace(double letterSpace);

    /**
     * @brief getAsFrame - each letter is ldBezierCurveObject
     * @return
     */
    const ldBezierCurveFrame &getFrame() const;

    float getWidth();
    float getHeight();


protected:
    void initLabelLettersFrame(const QString &word);

    float getLetterAWidth();
    float getInterLetterWidth();
    float getSpaceWidth();

    void reloadAllSvgLetters();

    float m_fontSize = 1.0f/16.f;
    double m_letterSpace = 1.;
    QString m_string;
    ldFont::Family m_fontFamily = ldFont::Family::Roboto;
    std::vector<ldSvgLetter> m_allSvgLetters;

    ldBezierCurveFrame _labelLettersFrame;
};

#endif // LDABSTRACTTEXT_H

