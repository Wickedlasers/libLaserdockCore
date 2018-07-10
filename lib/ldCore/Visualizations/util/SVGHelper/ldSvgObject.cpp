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
//  ldSvgObject.cpp
//  ldCore
//
//  Created by Eric Brugère on 12/05/16.
//  Copyright (c) 2016 Wicked Lasers. All rights reserved.
//

#include "ldSvgObject.h"

#include <QtCore/QTime>
#include <QtCore/QDebug>

#include <ldCore/Render/ldRendererOpenlase.h>
#include "ldCore/Visualizations/util/DrawHelper/ldBezierCurveDrawer.h"
#include "ldCore/Visualizations/util/SVGHelper/ldSvgReader.h"

const int FRAME_CHANGE_INDEX = 5;

// ldSvgObject
ldSvgObject::ldSvgObject(QString const &p_file)
    : _drawer(new ldBezierCurveDrawer)
{
    _svgData.push_back(ldBezierCurveObject(ldSvgReader::loadSvg(p_file, LD_SVG_READING_MAXIMIZE, 0.01f), false));

    init();
    updateCachedSvgData();
}

ldSvgObject::ldSvgObject(std::vector<QString> const &p_files)
    : _drawer(new ldBezierCurveDrawer)
{
    for(const QString &p_file : p_files) {
        _svgData.push_back(ldBezierCurveObject(ldSvgReader::loadSvg(p_file, LD_SVG_READING_MAXIMIZE, 0.01f), false));
    }

    init();
    updateCachedSvgData();
}

// ldSvgObject
ldSvgObject::~ldSvgObject()
{
}

// setScale
void ldSvgObject::setScale(float p_scale)
{
    _scale = p_scale;
    updateCachedSvgData();
}

// getScale
float ldSvgObject::getScale() const
{
    return _scale;
}

// setPosition
void ldSvgObject::setPosition(Vec2 p_position)
{
    _position = p_position;
    updateCachedSvgData();
}

// getPosition
Vec2 ldSvgObject::getPosition() const
{
    return _position;
}

// getSvgData
const ldBezierCurveObject &ldSvgObject::getSvgData(uint frame)
{
    if(frame >= _cachedSvgData.size()) {
        qWarning() << "No svg data for frame" << frame;
        return ldBezierCurveObject::stub;
    }

    return _cachedSvgData[frame];
}

// innerDraw
void ldSvgObject::innerDraw(ldRendererOpenlase* p_renderer)
{
    // draw
    _drawer->innerDraw(p_renderer, _cachedSvgData[m_currentFrame]);

    // switch to next frame each FRAME_CHANGE_INDEX draws
    m_drawFrame++;
    if(m_drawFrame > FRAME_CHANGE_INDEX) {
        m_drawFrame = 0;

        nextFrame();
    }
}

// init
void ldSvgObject::init()
{
    _drawer->setMaxPoints(10);
    _drawer->setBezierLengthCoeff(20);
}

// nextFrame
void ldSvgObject::nextFrame()
{
    m_currentFrame++;
    if(m_currentFrame >= _cachedSvgData.size()) {
        m_currentFrame = 0;
    }
}

// updateCachedSvgData
void ldSvgObject::updateCachedSvgData()
{
    _cachedSvgData = _svgData;

    for(uint i = 0; i < _svgData.size(); i++) {
//        if(i == 0) {
              _cachedSvgData[i].scale(_scale);
              _cachedSvgData[i].translate(_position);
//        } else {
//            _cachedSvgData.push_back(originalCurves);
//        }
    }
}
