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

// ldSvgFont.cpp
// Created by Eric Brugère on 4/sep/17
// Copyright (c) 2017 Wicked Lasers. All rights reserved.

#include <ldCore/Helpers/Text/ldSvgFont.h>

ldSvgFont::ldSvgFont(const QString &title, const QString &prefix, bool isCounterOnly, float interLetterSuffix)
    : m_title(title)
    , m_prefix(prefix)
    , m_isCounterOnly(isCounterOnly)
    , m_interLetterSuffix(interLetterSuffix)
{

}

QString ldSvgFont::title() const
{
    return m_title;
}

QString ldSvgFont::prefix() const
{
    return m_prefix;
}

bool ldSvgFont::isCounterOnly() const
{
    return m_isCounterOnly;
}

float ldSvgFont::interLetterSuffix() const
{
    return m_interLetterSuffix;
}

