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

// ldFont.cpp
// Created by Eric Brugère on 4/sep/17
// Copyright (c) 2017 Wicked Lasers. All rights reserved.

#include "ldCore/Helpers/Text/ldFont.h"

#include <ldCore/Helpers/ldEnumHelper.h>

QStringList ldFont::titles()
{
    QStringList titlest;

    for(const auto &fontFamily : ldEnumHelper::Enum<ldFont::Family>()) {
        titlest << ldFont(fontFamily).title();
    }

    return titlest;
}

ldFont::ldFont(const ldFont::Family &family)
    : m_family(family)
{

}

ldFont::Family ldFont::family() const
{
    return m_family;
}

QString ldFont::title() const
{
    switch (m_family) {
    case Family::Roboto: return "Roboto";
    case Family::Ubuntu: return "Ubuntu";
    case Family::Pacifico: return "Pacifico";
    case Family::Doris: return "Doris";
    case Family::Elixia: return "Elixia";
    case Family::Reeniebeanie: return "Reeniebeanie";
    case Family::Brave: return "Brave";
    }

    return "";
}

QString ldFont::prefix() const
{
    switch (m_family) {
    case Family::Roboto: return "roboto";
    case Family::Ubuntu: return "ubuntu";
    case Family::Pacifico: return "pacifico";
    case Family::Doris: return "doris";
    case Family::Elixia: return "elixia";
    case Family::Reeniebeanie: return "reeniebeanie";
    case Family::Brave: return "brave";
    }

    return "";
}

