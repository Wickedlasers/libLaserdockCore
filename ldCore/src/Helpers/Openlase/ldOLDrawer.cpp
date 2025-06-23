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
//  ldOLDrawer.cpp
//
//  Created by Sergey Gavrushkin on 02/02/17.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#include <ldCore/Helpers/Openlase/ldOLDrawer.h>

#include <QtCore/QtDebug>
#include <QtCore/QTime>

#include <ldCore/Helpers/ldEnumHelper.h>
#include <ldCore/Helpers/Color/ldColorEffects.h>
#include <ldCore/Render/ldRendererOpenlase.h>

const QHash<int, QString> ldOLDrawer::COLOR_EFFECT_STRING_HASH = {
    {ldEnumHelper::as_integer(ldOLDrawer::ColorEffect::No), "No"},
    {ldEnumHelper::as_integer(ldOLDrawer::ColorEffect::One), "Gradient"},
    {ldEnumHelper::as_integer(ldOLDrawer::ColorEffect::Two), "Striped"},
    {ldEnumHelper::as_integer(ldOLDrawer::ColorEffect::Three), "Rainbow"}
};

ldOLDrawer::ldOLDrawer()
{
}

ldOLDrawer::~ldOLDrawer()
{
}

ldOLDrawer::ColorEffect ldOLDrawer::colorEffect() const
{
    return m_colorEffectValue;
}

void ldOLDrawer::setColorEffect(const ldOLDrawer::ColorEffect &mode)
{
    if(m_colorEffectValue == mode)
        return;

    m_colorEffectValue = mode;

    switch(mode) {
    case ColorEffect::No:
        m_colorEffect.reset();
        break;
    case ColorEffect::One:
        m_colorEffect.reset(new ldColorEffectOne);
        break;
    case ColorEffect::Two:
        m_colorEffect.reset(new ldColorEffectTwo);
        break;
    case ColorEffect::Three:
        m_colorEffect.reset(new ldColorEffectThree);
        break;
    }

    if(m_colorEffect) {
        m_colorEffect->setBaseColorDecay(m_baseColorDecay);
    }
}


void ldOLDrawer::setColorEffectBaseColorDecay(int baseColorDecay)
{
    if(m_baseColorDecay == baseColorDecay) {
        return;
    }

    m_baseColorDecay = baseColorDecay;

    if(m_colorEffect) m_colorEffect->setBaseColorDecay(m_baseColorDecay);
}

void ldOLDrawer::setManualColorUpdate(int val_ms)
{
    if(m_colorEffect) m_colorEffect->setManualColorIncValue(val_ms);
}

void ldOLDrawer::innerDraw(ldRendererOpenlase *renderer, const std::vector<PointVector> &dataVect)
{
    innerDraw(renderer, ldOLPointObject(dataVect));
}

void ldOLDrawer::innerDraw(ldRendererOpenlase *renderer, const ldOLPointObject &dataVect)
{
    if(m_colorEffect) m_colorEffect->updateColor();

    draw(renderer, dataVect);

}

void ldOLDrawer::draw(ldRendererOpenlase *renderer, const ldOLPointObject &drawingData)
{
    ldRect dimInUnited = drawingData.dim();

    for(const std::vector<OLPoint> &pointVector : drawingData.data()) {
        renderer->begin(OL_LINESTRIP);
        for(const OLPoint &p: pointVector) {
            uint32_t color = m_colorEffect ? m_colorEffect->getColor(ldVec2(p.x, p.y), dimInUnited) :  p.color;
            renderer->vertex(p.x, p.y, color, 1);
        }
        renderer->end();
   }
}

std::vector<std::vector<OLPoint> > ldOLDrawer::makeSafeDrawing(const std::vector<std::vector<OLPoint> > &data) const
{
    std::vector<std::vector<OLPoint>> res;
    const int MAX_SAFE_POINTS = 4000;
    int safePoints = 0;
    for(const std::vector<OLPoint> &points : data) {
        std::vector<OLPoint> resPoints;
        for(const OLPoint &point : points) {
            resPoints.push_back(point);
            safePoints++;
            if(safePoints > MAX_SAFE_POINTS) {
                break;
            }
        }
        res.push_back(resPoints);

        if(safePoints > MAX_SAFE_POINTS) {
            break;
        }
    }

    return res;
}

