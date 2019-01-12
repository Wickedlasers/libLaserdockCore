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

#include "ldCore/Visualizations/Visualizers/Games/Core/ldGameSmoke.h"

#include "ldCore/Visualizations/Visualizers/Games/Core/ldGameObject.h"


// Constants.
namespace  {
const float LIFETIME = 0.125f;
const float RADIUS = 0.05f;
const int COLOR = 0xffffff;
}

ldGameSmoke::ldGameSmoke(Vec2 pos) : ldGameObject() {
    init();

    position = pos;

    setLifetime(LIFETIME);
}

void ldGameSmoke::updateGameObject(float deltaTime) {
    ldGameObject::updateGameObject(deltaTime);
}

void ldGameSmoke::drawGameObject(ldRendererOpenlase* p_renderer) {
    ldGameObject::drawGameObject(p_renderer);

    p_renderer->begin(OL_LINESTRIP);
    p_renderer->drawCircle(position.x, position.y, RADIUS * getLifetimePercentage(), COLOR);
    p_renderer->end();
}
