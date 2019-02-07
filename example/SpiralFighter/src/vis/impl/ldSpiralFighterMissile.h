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

#ifndef ldSpiralFighterMissile_H
#define ldSpiralFighterMissile_H

#include <ldCore/Render/ldRendererOpenlase.h>
#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldCore/Helpers/Color/ldColorUtil.h"

#include "ldCore/Visualizations/Visualizers/Games/Core/ldGameObject.h"

class ldSpiralFighterMissile : public ldGameObject
{
public:
    // Constructor and destructor.
    ldSpiralFighterMissile(Vec2 direction);
    ~ldSpiralFighterMissile();

protected:
    virtual void init() override;
    virtual void updateGameObject(float deltaTime) override;
    virtual void drawGameObject(ldRendererOpenlase* p_renderer) override;

private:

};

#endif // ldSpiralFighterMissile_H
