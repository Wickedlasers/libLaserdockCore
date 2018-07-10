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
//  ldColorUtilMEO.hpp
//  ldCore
//
//  Created by MEO
//  Copyright © 2016 Wicked Lasers. All rights reserved.
//

#ifndef LDCOLORUTILMEO_H
#define LDCOLORUTILMEO_H

#include <QtCore/QtGlobal>

#include "ldCore/ldCore_global.h"

class LDCORESHARED_EXPORT ldColorUtilMEO {
public:
    void HSVtoRGB(quint16 h, quint8 s, quint8 v, quint8 wheelLine, quint8 color[3]);
    void HSVtoRGBInt(quint16 h, quint8 s, quint8 v, quint8 wheelLine, int color);
    void RGBtoHSV(quint8 r, quint8 g, quint8 b, quint16 hsv[3]);
    quint32 convertRGBtoColor(quint8 r, quint8 g, quint8 b);
};

#endif /* LDCOLORUTILMEO_H */
