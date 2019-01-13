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
//  ldGameObject.h
//  ldCore
//
//  Created by Glauco Pires 26/09/2017.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#ifndef ldGameObject_H
#define ldGameObject_H

#include <ldCore/Render/ldRendererOpenlase.h>

#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldCore/Helpers/Color/ldColorUtil.h"

class LDCORESHARED_EXPORT ldGameObject
{
public:
    static int uuid;

    static void drawVertexRainbow(ldRendererOpenlase* p_renderer, QList<Vec2> vertices, QList<int> colors, int segmentsPerLine = 4, int repeat = 1);

    bool operator == (const ldGameObject& s) const { return m_id == s.getId(); }
    bool operator != (const ldGameObject& s) const { return !operator==(s); }

    ldGameObject();
    virtual ~ldGameObject();

    // Functions.
    void update(float deltaTime);
    void draw(ldRendererOpenlase* p_renderer);

    // Collision functions.
    bool overlaps(ldGameObject other);
    bool overlaps(Vec2 otherPosition);
    void collide(ldGameObject other);

    // Getters and setters.
    void setLifetime(float duration);
    float getLifetimePercentage();
    int getId() const;
    float getTimeAlive();
    Vec2 center();

    // Fields.
    Vec2 position;
    Vec2 size;
    Vec2 velocity;
    Vec2 acceleration;

    float rotation;

    bool enabled;
    bool visible;
    bool expired;
    bool bounded;
    bool useGravity;
    float gravityScale = 1.f;

protected:
    // Functions.
    virtual void init();
    virtual void updateGameObject(float deltaTime);
    virtual void drawGameObject(ldRendererOpenlase* p_renderer);


private:
    int m_id;

    // Fields.
    float m_lifeTimer = 0.0f;
    float m_totalLifetime = 0.0f;
    float m_timeAlive = 0.0f;
};

#endif // ldGameObject_H
