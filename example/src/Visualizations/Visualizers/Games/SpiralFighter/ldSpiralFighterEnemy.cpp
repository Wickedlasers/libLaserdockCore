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

#include "ldSpiralFighterEnemy.h"

// Constants.
const float WIDTH = 0.05f;
const float HEIGHT = 0.05f;
const float SPEED = 0.1f;
const float SPEED_DASHER = 0.15f;

const float ANGULAR_SPEED = M_PIf * 0.500f;

const float ROTATION_SMOOTHNESS = 0.825f;

ldSpiralFighterEnemy::ldSpiralFighterEnemy(Vec2 pos) : ldGameObject() {
    init();

    position = pos;
}

ldSpiralFighterEnemy::~ldSpiralFighterEnemy() {
}

void ldSpiralFighterEnemy::init() {
    ldGameObject::init();

    size.x = WIDTH;
    size.y = HEIGHT;

    m_targetRotation = 0.0f;
    m_rotatedAngle = 0.0f;

    m_hasRotated = false;
    m_rotating = false;

    m_type = rand() % 100 < 20 ? 1 : 0;
    m_speed = m_type == 0 ? SPEED : SPEED_DASHER;

    velocity = Vec2(cosf(rotation) * m_speed, sinf(rotation) * m_speed);
}

void ldSpiralFighterEnemy::updateGameObject(float deltaTime) {
    ldGameObject::updateGameObject(deltaTime);

    // Rotate object with smoothness.
    rotation = rotation * (ROTATION_SMOOTHNESS) + (m_targetRotation * (1 - ROTATION_SMOOTHNESS));

    // Make enemy move towards center.
    if (!m_rotating) {
        Vec2 direction = Vec2(-position.x, -position.y).normalize();
        velocity = Vec2(direction.x * m_speed, direction.y * m_speed);
    } else {
        velocity = Vec2::zero;

        float angleDisplacement = ANGULAR_SPEED * deltaTime;

        m_rotatedAngle += angleDisplacement;

        float distanceToCenter = position.magnitude();
        float currentAngle = position.toRadians();
        float newAngle = currentAngle + angleDisplacement;

        position = Vec2(cosf(newAngle) * distanceToCenter, sinf(newAngle) * distanceToCenter);

        if (m_rotatedAngle >= M_PI) {
            m_rotating = false;
        }
    }
}

void ldSpiralFighterEnemy::drawGameObject(ldRendererOpenlase* p_renderer) {
    ldGameObject::drawGameObject(p_renderer);

    // Draw body.
    p_renderer->begin(OL_LINESTRIP);

    int color = rand() % 2 == 0 ? 0xffff00 : 0xff3333;

    p_renderer->translate(position.x + size.x * 0.5f, position.y + size.y * 0.5f);
    p_renderer->rotate(-rotation);
    p_renderer->translate(-size.x * 0.5, -size.y * 0.5);

    if (m_type == 0) {
        p_renderer->vertex3(size.x * 0.0f, size.y * 0.0f, 0.0f, color, 3);
        p_renderer->vertex3(size.x * 1.0f, size.y * 0.0f, 0.0f, color, 3);
        p_renderer->vertex3(size.x * 0.5f, size.y * 1.0f, 0.0f, color, 3);
        p_renderer->vertex3(size.x * 0.0f, size.y * 0.0f, 0.0f, color, 3);
    } else {
        p_renderer->drawCircle(0, 0, size.x / 2, 0xff00ff);
    }

    p_renderer->loadIdentity();

    p_renderer->end();
}

void ldSpiralFighterEnemy::rotate() {
    if (m_type == 0) {
        m_hasRotated = true;
        m_rotating = true;
    }
}

bool ldSpiralFighterEnemy::hasRotated() {
    return m_hasRotated;
}
