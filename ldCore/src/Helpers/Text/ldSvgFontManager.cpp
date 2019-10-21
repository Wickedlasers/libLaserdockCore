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

#include <ldCore/Helpers/Text/ldSvgFontManager.h>

#include <ldCore/Helpers/ldEnumHelper.h>

ldSvgFontManager::ldSvgFontManager(QObject *parent)
    : QObject(parent)
{
}

ldSvgFontManager::~ldSvgFontManager()
{
}

void ldSvgFontManager::addFont(const ldSvgFont &font)
{
    m_fonts.push_back(font);
}

const std::vector<ldSvgFont> &ldSvgFontManager::fonts() const
{
    return m_fonts;
}

const ldSvgFont &ldSvgFontManager::font(int index) const
{
    return m_fonts[index];
}

QStringList ldSvgFontManager::titles(bool excludeCounter)
{
    QStringList titles;

    for(const ldSvgFont &font : m_fonts) {
        if(font.isCounterOnly() && excludeCounter)
            continue;

        titles << font.title();
    }

    return titles;
}

int ldSvgFontManager::counterFontIndex() const
{
    auto it = std::find_if(m_fonts.begin(), m_fonts.end(), [](const ldSvgFont &font) {
       return font.isCounterOnly();
    });

    if(it != m_fonts.end())
        return it - m_fonts.begin();
    else
        return -1;
}


