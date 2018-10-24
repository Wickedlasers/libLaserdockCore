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

#include "ldSpiralFighterPlayer.h"

// Constants.
const float WIDTH = 0.08f;
const float HEIGHT = 0.06f;
const float SPEED_ROTATION = M_PIf * 1.5f;

const float COOLDOWN_DURATION = 0.25f;
const float ROTATION_SMOOTHNESS = 0.825f;

ldSpiralFighterPlayer::ldSpiralFighterPlayer() : ldGameObject() {
    init();
}

ldSpiralFighterPlayer::~ldSpiralFighterPlayer() {
}

void ldSpiralFighterPlayer::init() {
    ldGameObject::init();

    size.x = WIDTH;
    size.y = HEIGHT;
    m_targetRotation = 0.0f;
    m_cooldownTimer = COOLDOWN_DURATION;

    m_hasUsedPowerup = false;
}

void ldSpiralFighterPlayer::updateGameObject(float deltaTime) {
    ldGameObject::updateGameObject(deltaTime);

    // Rotate player with smoothness.
    m_targetRotation += m_rotationChange * SPEED_ROTATION * deltaTime;
    rotation = rotation * (ROTATION_SMOOTHNESS) + (m_targetRotation * (1 - ROTATION_SMOOTHNESS));

    // Fire missiles.
    m_cooldownTimer -= deltaTime;
    if (m_cooldownTimer <= 0.0f && m_pressingShoot) {
        m_cooldownTimer = COOLDOWN_DURATION;
        onFire(this);
    }

    // Use powerup.
    if (m_pressingPowerup && !hasUsedPowerup()) {
        m_hasUsedPowerup = true;
        onPowerup();
    }
}

void ldSpiralFighterPlayer::drawGameObject(ldRendererOpenlase* p_renderer) {
    ldGameObject::drawGameObject(p_renderer);

    // Draw body.
    p_renderer->begin(OL_LINESTRIP);

    int playerColor = 0x00ff00;
    int cannonColor = 0x00ffff;

    p_renderer->translate(position.x + size.x * 0.0f, position.y + size.y * 0.0f);
    p_renderer->rotate(-rotation);
    p_renderer->translate(-size.x * 0.3, -size.y * 0.5);

    p_renderer->vertex3(size.x * 0.0f, size.y * 0.0f, 0.0f, playerColor, 3);
    p_renderer->vertex3(size.x * 0.6f, size.y * 0.0f, 0.0f, playerColor, 5);
    p_renderer->vertex3(size.x * 0.6f, size.y * 0.3f, 0.0f, playerColor, 5);
    p_renderer->vertex3(size.x * 1.0f, size.y * 0.3f, 0.0f, cannonColor, 5);
    p_renderer->vertex3(size.x * 1.0f, size.y * 0.7f, 0.0f, cannonColor, 5);
    p_renderer->vertex3(size.x * 0.6f, size.y * 0.7f, 0.0f, cannonColor, 5);
    p_renderer->vertex3(size.x * 0.6f, size.y * 1.0f, 0.0f, playerColor, 5);
    p_renderer->vertex3(size.x * 0.0f, size.y * 1.0f, 0.0f, playerColor, 5);
    p_renderer->vertex3(size.x * 0.0f, size.y * 0.0f, 0.0f, playerColor, 2);

    p_renderer->loadIdentity();

    p_renderer->end();
}

Vec2 ldSpiralFighterPlayer::getMissileSpawnPosition() {
    return Vec2(position.x + size.x * 1.0f * cosf(rotation), position.y + size.y * 0.5f * sinf(rotation));
}

void ldSpiralFighterPlayer::givePowerup() {
    m_hasUsedPowerup = false;
}

bool ldSpiralFighterPlayer::hasUsedPowerup() {
    return m_hasUsedPowerup;
}

/*
 * Input functions.
 */

void ldSpiralFighterPlayer::rotate(double rotate) {
    m_rotationChange = rotate;
}

void ldSpiralFighterPlayer::onPressedShoot(bool pressed) {
    m_pressingShoot = pressed;
}

void ldSpiralFighterPlayer::onPressedPowerup(bool pressed) {
    m_pressingPowerup = pressed;
}

void ldSpiralFighterPlayer::releaseKeys() {
    m_rotationChange = false;
    m_pressingShoot = false;
    m_pressingPowerup = false;
}
