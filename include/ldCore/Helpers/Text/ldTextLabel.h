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

// ldTextLabel.h
// Created by Eric Brugère on 10/dec/16.
// Copyright (c) 2016 Wicked Lasers. All rights reserved.

#ifndef ldTextLabel_H
#define ldTextLabel_H

#include <memory>

#include <ldCore/Render/ldRendererOpenlase.h>

#include "ldCore/Helpers/Maths/ldMaths.h"

#include "ldAbstractText.h"

class ldBezierCurveDrawer;

class LDCORESHARED_EXPORT ldTextLabel : public ldAbstractText
{
public:
    /** ldTextLabel params
    * p_string is the string to write
    * p_fontSize is 1.0 to show a upper A on the whole laser projection. 1/2.0 would mean half the projection and so on
    */
    ldTextLabel(const QString &p_string = "", float p_fontSize = 1.0f/16, const Vec2 &p_position = Vec2());
    ~ldTextLabel();

    void setPosition(const Vec2 &p_p);
    Vec2 getPosition() const;

    void setColor(uint32_t p_color);
    uint32_t getColor() const;

    virtual void setText(const QString& p_string) override;
    QString getText() const;
    void clear();

    /**
     * @brief setIncrementXPositionOrLoop - banner text feature. call it each time before drow
     * @param delta - how fast text should move
     * @return true if move was done or false if it was reset to initial pos
     */
    bool setIncrementXPositionOrLoop(float delta);

    /**
     * @brief innerDraw -draw
     * @param p_renderer
     */
    void innerDraw(ldRendererOpenlase* p_renderer);

    ldBezierCurveDrawer *drawer() const { return _drawer.get(); }

    std::vector<std::vector<OLPoint>> getDrawingData() const;

private:
    uint32_t _color;
    Vec2 _position; // working in [0,1]x[0,1]

    std::unique_ptr<ldBezierCurveDrawer> _drawer;

};

#endif // ldTextLabel_H


