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

// ldSvgFont.h
// Created by Eric Brugère on 4/sep/17
// Copyright (c) 2017 Wicked Lasers. All rights reserved.

#ifndef LDSVGFONT_H
#define LDSVGFONT_H

#include "ldCore/Helpers/Maths/ldMaths.h"

class LDCORESHARED_EXPORT ldSvgFont {

public:
    explicit ldSvgFont(const QString &title,
                    const QString &prefix = "",
                    bool isCounterOnly = false,
                    float interLetterSuffix = 0.36f
            );

    QString title() const;
    QString prefix() const;

    bool isCounterOnly() const;
    float interLetterSuffix() const;

    bool isInternalFont() const;

private:
    QString m_title;
    QString m_prefix;

    bool m_isCounterOnly = false;
    float m_interLetterSuffix = 0.36f;
};


#endif // LDSVGFONT_H


