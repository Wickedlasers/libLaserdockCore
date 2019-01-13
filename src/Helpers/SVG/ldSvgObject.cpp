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

#include "ldCore/Helpers/SVG/ldSvgObject.h"

#include <QtCore/QTime>
#include <QtCore/QDebug>

#include <ldCore/Render/ldRendererOpenlase.h>
#include "ldCore/Helpers/Draw/ldBezierCurveDrawer.h"
#include "ldCore/Helpers/SVG/ldSvgReader.h"

const int FRAME_CHANGE_INDEX = 5;

// ldSvgObject
ldSvgObject::ldSvgObject(const QString &p_file)
    : _drawer(new ldBezierCurveDrawer)
    , m_files(QStringList(p_file))
{
    init();
}

ldSvgObject::ldSvgObject(const QStringList &p_files)
    : _drawer(new ldBezierCurveDrawer)
    , m_files(p_files)
{

    init();
}

// ldSvgObject
ldSvgObject::~ldSvgObject()
{
}

// setScale
void ldSvgObject::setScale(float p_scale)
{
    _scale = p_scale;
    m_isValidCache = false;
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
    m_isValidCache = false;
}

// getPosition
Vec2 ldSvgObject::getPosition() const
{
    return _position;
}

// getSvgData
const ldBezierCurveObject &ldSvgObject::getSvgData(uint frame)
{
    if(!m_isValidCache) updateCachedSvgData();

    if(frame >= _cachedSvgData.size()) {
        qWarning() << "No svg data for frame" << frame;
        return ldBezierCurveObject::stub;
    }

    return _cachedSvgData[frame];
}

// innerDraw
void ldSvgObject::innerDraw(ldRendererOpenlase* p_renderer)
{
    if(!m_isValidCache) updateCachedSvgData();
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
    if(!m_isValidCache) updateCachedSvgData();

    m_currentFrame++;
    if(m_currentFrame >= _cachedSvgData.size()) {
        m_currentFrame = 0;
    }
}

// updateCachedSvgData
void ldSvgObject::updateCachedSvgData()
{
    if(_svgData.empty()) {
        for(const QString &p_file : m_files) {
            _svgData.push_back(ldBezierCurveObject(ldSvgReader::loadSvg(p_file, ldSvgReader::Type::Maximize, 0.01f), false));
        }
    }

    _cachedSvgData = _svgData;

    for(uint i = 0; i < _svgData.size(); i++) {
        _cachedSvgData[i].scale(_scale);
        _cachedSvgData[i].translate(_position);
    }

    m_isValidCache = true;
}
