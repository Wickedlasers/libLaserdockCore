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
//  ldSvgObject.h
//  ldCore
//
//  Created by Eric Brugère on 12/05/16.
//  Copyright (c) 2016 Wicked Lasers. All rights reserved.
//

#ifndef __ldCore__ldSvgObject__
#define __ldCore__ldSvgObject__

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include "ldCore/Helpers/BezierCurve/ldBezierCurveFrame.h"
#include "ldCore/Helpers/Draw/ldBezierCurveDrawer.h"
#include "ldCore/Helpers/Maths/ldMaths.h"

class ldRendererOpenlase;

class LDCORESHARED_EXPORT ldSvgObject
{
public:
    explicit ldSvgObject(const QString &p_file);
    explicit ldSvgObject(const QStringList &p_file);
    ~ldSvgObject();

    void setScale(float p_scale);
    float getScale() const;

    void setPosition(Vec2 p_position);
    Vec2 getPosition() const;

    const ldBezierCurveObject &getSvgData(uint frame = 0);

    void innerDraw(ldRendererOpenlase* p_renderer);

private:
    void init();
    void nextFrame();
    void updateCachedSvgData();


    std::unique_ptr<ldBezierCurveDrawer> _drawer;

    QStringList m_files;

    std::vector<ldBezierCurveObject> _svgData;
    bool m_isValidCache = false;
    std::vector<ldBezierCurveObject> _cachedSvgData;
    Vec2 _position;
    float _scale = 1.0f;

    uint m_currentFrame = 0;
    uint m_drawFrame = 0;
};

#endif /*__ldCore__ldSvgObject__*/
