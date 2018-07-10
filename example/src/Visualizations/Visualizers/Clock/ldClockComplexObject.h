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
//  ldClockComplexObject.h
//  ldCore
//
//  Created by Eric Brugère on 12/05/16.
//  Copyright (c) 2016 Wicked Lasers. All rights reserved.
//

#ifndef __ldCore__ldClockComplexObject__
#define __ldCore__ldClockComplexObject__

#include <QtCore/QObject>
#include <QtCore/QTime>

#include <ldCore/Render/ldRendererOpenlase.h>
#include "ldCore/Visualizations/util/MathsHelper/ldMaths.h"

class ldClockComplexObject : public QObject
{
    Q_OBJECT
public:
    ldClockComplexObject();
    ~ldClockComplexObject();

    // call innerDraw with your current m_renderer
    void innerDraw(ldRendererOpenlase* p_renderer, const QTime &time = QTime::currentTime());
    void showAll(bool b_all_visible);

private:
    bool _all_visible;
    svgBezierCurves _svgHour, _svgMin;
    void drawDataBezierAsLinestrip(ldRendererOpenlase* p_renderer, svgBezierCurves &dataVect, float rotation, int color);
};

#endif /*__ldCore__ldClockComplexObject__*/

