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

// ld3dTextLabelOne.h
// Created by Eric Brugère on 12/jan/17.
// Copyright (c) 2017 Wicked Lasers. All rights reserved.

#ifndef ld3dTextLabelOne_H
#define ld3dTextLabelOne_H

#include <QtCore/QStringList>

#include <ldCore/Render/ldRendererOpenlase.h>
#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldCore/Helpers/Draw/ld3dBezierCurveDrawer.h"

#include "ldAbstractText.h"

class LDCORESHARED_EXPORT ld3dTextLabelOne : public ldAbstractText
{
public:
    explicit ld3dTextLabelOne(const QString &p_string = "", float p_fontSize = 1.0f/16);
    ~ld3dTextLabelOne();

    virtual void setText(const QString& p_string) override;

    void innerDraw(ldRendererOpenlase* p_renderer);
    //void setFontSize(float fontSize);

    void setSpeedCoeff(float speedCoeff);

    void setModeRotate(bool mode_rotate);

private:
    void updateCurrentString();

    QStringList _words;
    int _currentIndice = 0;

    bool m_mode_rotate = false;
    int m_rotate_step = 0;

    std::unique_ptr<ld3dBezierCurveDrawer> _drawer;
};

#endif // ld3dTextLabelOne_H


