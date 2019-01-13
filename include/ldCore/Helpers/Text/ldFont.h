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

// ldFont.h
// Created by Eric Brugère on 4/sep/17
// Copyright (c) 2017 Wicked Lasers. All rights reserved.

#ifndef LDFONT_H
#define LDFONT_H

#include "ldCore/Helpers/Maths/ldMaths.h"

class LDCORESHARED_EXPORT ldFont {

public:
    enum class Family {
        Roboto,
        Ubuntu,
        Pacifico,
        Doris,
        Elixia,
        Reeniebeanie,
        Brave,
        First = Roboto,
        Last = Brave
    };

    static QStringList titles();

    explicit ldFont(const Family &family = Family::Roboto);

    Family family() const;
    QString title() const;
    QString prefix() const;

private:
    Family m_family;
};


#endif // LDFONT_H


