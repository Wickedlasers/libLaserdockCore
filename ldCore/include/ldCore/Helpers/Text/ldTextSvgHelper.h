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

// ldTextSvgHelper.h
// Created by Eric Brugère on 8/dec/16.
// Copyright (c) 2016 Wicked Lasers. All rights reserved.

#ifndef ldTextSvgHelper_H
#define ldTextSvgHelper_H

#include "ldCore/Helpers/Maths/ldMaths.h"

#include "ldFont.h"
#include "ldSvgLetter.h"

class LDCORESHARED_EXPORT ldTextSvgHelper
{
public:
    static ldTextSvgHelper* instance();

    int indexForSvgValidChars(const QChar& p_str);
    QChar replaceAccentAndSome(const QChar& p_str);
    QString svgPathForChar(const QChar &p_str, const ldFont::Family &font);

    std::vector<ldSvgLetter> resizedSvgLetters(float p_scale, const ldFont::Family &font = ldFont::Family::Roboto);

private:
    explicit ldTextSvgHelper();

    void initCharMap();
    void initSvgValidChars();
    void initSvgLetters();

    bool isSvgChar(const QChar& p_str);

    QMap<ldFont::Family, std::vector<ldSvgLetter>> m_laserdockSvgLetters;

    QList<QChar> m_svgValidChars;

    QHash<QChar, QString> m_charMap;
};

#endif // ldTextSvgHelper_H


