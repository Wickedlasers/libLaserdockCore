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
//  ldOLDrawer.h
//
//  Created by Sergey Gavrushkin on 02/02/17.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#ifndef LDOLDRAWER_H
#define LDOLDRAWER_H

#include <memory>

#include <ldCore/Helpers/Maths/ldMaths.h>
#include <ldCore/Helpers/Openlase/ldOLPointObject.h>

class ldAbstractColorEffect;
class ldRendererOpenlase;
struct OLPoint;

class LDCORESHARED_EXPORT ldOLDrawer
{
public:
    enum class ColorEffect {
        No,
        One,
        Two,
        Three,
        First = No,
        Last = Three
    };
    const static QHash<int, QString> COLOR_EFFECT_STRING_HASH;

    explicit ldOLDrawer();
    ~ldOLDrawer();

    ColorEffect colorEffect() const;
    void setColorEffect(const ColorEffect &mode);
    void setColorEffectBaseColorDecay(int baseColorDecay);
    void setManualColorUpdate(int val_ms);

    void innerDraw(ldRendererOpenlase* renderer, const std::vector<PointVector> &dataVect);
    void innerDraw(ldRendererOpenlase* renderer, const ldOLPointObject &dataVect);

private:
    void draw(ldRendererOpenlase* renderer, const ldOLPointObject &drawingData);
    std::vector<std::vector<OLPoint> > makeSafeDrawing(const std::vector<std::vector<OLPoint> > &data) const;

    ColorEffect m_colorEffectValue = ColorEffect::No;
    std::unique_ptr<ldAbstractColorEffect> m_colorEffect;
    int m_baseColorDecay = 223;
};

#endif // LDBEZIERCURVEDRAWER_H

