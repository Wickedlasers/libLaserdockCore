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

#include <QtCore/QtDebug>

#include <ldCore/Helpers/ldEnumHelper.h>
#include <ldCore/Helpers/Text/ldSvgFontManager.h>

ldSvgFontManager::ldSvgFontManager(QObject *parent)
    : QObject(parent)
{
}

ldSvgFontManager::~ldSvgFontManager()
{
}

void ldSvgFontManager::addFont(const ldSvgFont &font, bool isDefaultFont)
{
    if (isDefaultFont) m_defaultFont = font.title();
    m_fonts.push_back(font);
}

void ldSvgFontManager::insertFont(const ldSvgFont &font, int index)
{
    m_fonts.insert(m_fonts.begin() + index, font);
}

const std::vector<ldSvgFont> &ldSvgFontManager::fonts() const
{
    return m_fonts;
}

const ldSvgFont &ldSvgFontManager::font(int index) const
{
    return m_fonts[index];
}

ldSvgFont ldSvgFontManager::font(const QString &fontFamily) const
{
    auto it = std::find_if(m_fonts.begin(), m_fonts.end(), [fontFamily](const ldSvgFont &font) {
       return font.title() == fontFamily;
    });

    if(it != m_fonts.end()) {
        return *it;
    } else {
        qWarning() << __FUNCTION__ << fontFamily << "not found";
        return ldSvgFont("");
    }
}

ldSvgFont ldSvgFontManager::defaultFont() const
{
    return (m_fonts.size()==0) ? ldSvgFont("") : m_defaultFont.isEmpty() ? m_fonts[0] : font(m_defaultFont);
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

size_t ldSvgFontManager::internalFontsCount()
{
    return m_internalFontsCount;
}

void ldSvgFontManager::endInternalFonts()
{
    m_internalFontsCount = m_fonts.size();
}


