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

#include "ldCore/Visualizations/Visualizers/Games/Core/ldGameExplosion.h"

#include "ldCore/Visualizations/Visualizers/Games/Core/ldGameObject.h"


// Constants.
namespace  {
const float LIFETIME = 0.25f;
}

ldGameExplosion::ldGameExplosion(Vec2 pos, int col, float size) : ldGameObject() {
    init();

    position = pos;
    m_color = col;
    m_size = size;

    setLifetime(LIFETIME);
}

void ldGameExplosion::updateGameObject(float deltaTime) {
    ldGameObject::updateGameObject(deltaTime);
}

void ldGameExplosion::drawGameObject(ldRendererOpenlase* p_renderer) {
    ldGameObject::drawGameObject(p_renderer);

    p_renderer->begin(OL_LINESTRIP);
    p_renderer->drawCircle(position.x, position.y, m_size * getLifetimePercentage(), m_color);
    p_renderer->end();
}
