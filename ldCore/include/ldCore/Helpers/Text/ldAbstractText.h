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
#include "ldCore/Helpers/Openlase/ldOLPointObject.h"
#include "ldSvgFont.h"
#include "ldSvgLetter.h"

// ldAbstractText
class LDCORESHARED_EXPORT ldAbstractText : public QObject
{
    Q_OBJECT

public:
    ldAbstractText(const QString &text, float fontSize, const ldVec2 &p_position = ldVec2());
    ~ldAbstractText();

    virtual void setText(const QString &p_string);
    QString getText() const;

    void setFontSize(float fontSize);
    float getFontSize() const;

    void setFont(const QString &fontFamily);
    QString font() const;

    float letterSpace() const;
    void setLetterSpace(float letterSpace);

    void setVertical(bool isVertical);
    bool isVertical() const;

    // working in [0,1]x[0,1]
    virtual void setPosition(const ldVec2 &p_p);
    ldVec2 getPosition() const;

    /**
     * @brief getAsFrame - each letter is ldBezierCurveObject
     * @return
     */
    const ldBezierCurveFrame &getFrame() const;
    ldOLPointObject getOLObject() const;

    float getWidth() const;
    float getHeight() const;

    ldVec2 getFirstLetterSize();

    void cloneProperties(ldAbstractText *other) const;

protected:
    virtual void initTextFrame(const QString &word);

    ldBezierCurveFrame m_textFrame;
    ldOLPointObject m_textPointObject;

private:
    ldVec2 getLetterSize(const QChar &ch);
    float getInterLetterWidth();
    float getSpaceWidth();

    void reloadAllSvgLetters();

    QString m_text;
    float m_fontSize = 1.0f/16.f;
    QString m_fontFamily;
    float m_letterSpaceScale = 1.f;
    bool m_isVertical = false;


    ldVec2 m_position;

    std::vector<ldSvgLetter> m_allSvgLetters;
    ldOLPointObject generatePointObject(const QString &text, const QString &fontFamily, float fontSize, float letterSpace = 0.f, bool isVertical = false);
};

#endif // LDABSTRACTTEXT_H

