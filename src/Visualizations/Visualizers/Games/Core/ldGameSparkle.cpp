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

#include "ldCore/Visualizations/Visualizers/Games/Core/ldGameSparkle.h"

#include "ldCore/Visualizations/Visualizers/Games/Core/ldGameObject.h"


// Constants.
namespace  {
const float LIFETIME = 0.25f;
const float WIDTH_MIN = 0.05f;
const float WIDTH_MAX = 0.20f;
const int COLOR = 0xffff00;
}

ldGameSparkle::ldGameSparkle(Vec2 pos) : ldGameObject() {
    init();

    position = pos;
    rotation = ((rand() % 100) / 99.0f) * 2 * M_PI;
    size.x = WIDTH_MIN;

    setLifetime(LIFETIME);
}

void ldGameSparkle::updateGameObject(float deltaTime) {
    ldGameObject::updateGameObject(deltaTime);

    size.x = WIDTH_MAX - (WIDTH_MAX - WIDTH_MIN) * getLifetimePercentage();
}

void ldGameSparkle::drawGameObject(ldRendererOpenlase* p_renderer) {
    ldGameObject::drawGameObject(p_renderer);

    p_renderer->begin(OL_LINESTRIP);
    p_renderer->vertex(position.x, position.y, COLOR);
    p_renderer->vertex(position.x + cosf(rotation) * size.x, position.y + sinf(rotation) * size.x, COLOR);
    p_renderer->end();
}
