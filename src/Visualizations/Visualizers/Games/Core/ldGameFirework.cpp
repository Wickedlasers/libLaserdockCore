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

#include "ldCore/Visualizations/Visualizers/Games/Core/ldGameFirework.h"

#include "ldCore/Visualizations/Visualizers/Games/Core/ldGameObject.h"


// Constants.
namespace  {
const float LIFETIME_MIN = 0.30f;
const float LIFETIME_MAX = 0.50f;

const float SPEED_MIN = 0.5f;
const float SPEED_MAX = 1.0f;
}

ldGameFirework::ldGameFirework(Vec2 p_position, float p_rotation) : ldGameObject() {
    init();

    position = p_position;
    rotation = p_rotation;

    float randomLifetime = LIFETIME_MIN + (LIFETIME_MAX - LIFETIME_MIN) * ((rand() % 100) / 100.0f);
    float randomSpeed = SPEED_MIN + (SPEED_MAX - SPEED_MIN) * ((rand() % 100) / 100.0f);

    setLifetime(randomLifetime);
    velocity = Vec2(randomSpeed * cosf(rotation), randomSpeed * sinf(rotation));
    m_color = ldColorUtil::colorForStep(p_rotation / (2 * M_PI));
}

void ldGameFirework::updateGameObject(float deltaTime) {
    ldGameObject::updateGameObject(deltaTime);
}

void ldGameFirework::drawGameObject(ldRendererOpenlase* p_renderer) {
    ldGameObject::drawGameObject(p_renderer);

    p_renderer->begin(OL_LINESTRIP);
    p_renderer->drawCircle(position.x, position.y, 0.1f * getLifetimePercentage(), m_color);
    p_renderer->end();
}
