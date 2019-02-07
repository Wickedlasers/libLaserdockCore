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

#ifndef ldSpiralFighterEnemy_H
#define ldSpiralFighterEnemy_H

#include <ldCore/Render/ldRendererOpenlase.h>
#include "ldCore/Helpers/Maths/ldMaths.h"

#include "ldCore/Visualizations/Visualizers/Games/Core/ldGameObject.h"

class ldSpiralFighterEnemy : public ldGameObject
{
public:
    // Constructor and destructor.
    ldSpiralFighterEnemy(Vec2 pos);
    ~ldSpiralFighterEnemy();

    void rotate();
    bool hasRotated();

protected:
    virtual void init() override;
    virtual void updateGameObject(float deltaTime) override;
    virtual void drawGameObject(ldRendererOpenlase* p_renderer) override;

private:
    float m_rotatedAngle;
    float m_targetRotation;
    bool m_hasRotated;
    bool m_rotating;
    int m_type;
    float m_speed;
};

#endif // ldSpiralFighterEnemy_H
