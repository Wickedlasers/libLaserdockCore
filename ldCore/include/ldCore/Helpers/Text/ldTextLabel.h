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
    ldTextLabel(const QString &text = "", float fontSize = 1.0f/16, const ldVec2 &p_position = ldVec2());
    ~ldTextLabel();

    void setColor(uint32_t p_color);
    uint32_t getColor() const;

    virtual void setText(const QString &text) override;
    void clear();

    /**
     * @brief innerDraw -draw
     * @param p_renderer
     */
    void innerDraw(ldRendererOpenlase* p_renderer);

    ldBezierCurveDrawer *drawer() const { return m_drawer.get(); }

    std::vector<std::vector<OLPoint>> getDrawingData() const;

protected:
    virtual void initTextFrame(const QString &word) override;

private:
    uint32_t m_color = 0xFFFFFF;

    std::unique_ptr<ldBezierCurveDrawer> m_drawer;
};

#endif // ldTextLabel_H


