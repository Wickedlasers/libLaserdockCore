#include "ldSpiralFighterMissile.h"

// Constants
const float WIDTH = 0.05f;
const float HEIGHT = 0.05f;
const float SPEED = 1.2f;
const float LIFETIME = 5.0f;

ldSpiralFighterMissile::ldSpiralFighterMissile(Vec2 direction) : ldGameObject() {
    init();

    size.x = WIDTH;
    size.y = HEIGHT;

    velocity = Vec2(direction.x * SPEED, direction.y * SPEED);
}

ldSpiralFighterMissile::~ldSpiralFighterMissile() {
}

void ldSpiralFighterMissile::init() {
    ldGameObject::init();

    setLifetime(LIFETIME);
}

void ldSpiralFighterMissile::updateGameObject(float deltaTime) {
    ldGameObject::updateGameObject(deltaTime);
}

void ldSpiralFighterMissile::drawGameObject(ldRendererOpenlase* p_renderer) {
    ldGameObject::drawGameObject(p_renderer);

    // Draw rect.
    p_renderer->begin(OL_LINESTRIP);

    p_renderer->translate(position.x, position.y);

    int color = ldColorUtil::colorForStep(getLifetimePercentage());

    p_renderer->vertex3(size.x * 0.0f, size.y * 0.0f, 0.0f, color);
    p_renderer->vertex3(size.x * 1.0f, size.y * 0.0f, 0.0f, color);
    p_renderer->vertex3(size.x * 1.0f, size.y * 1.0f, 0.0f, color);
    p_renderer->vertex3(size.x * 0.0f, size.y * 1.0f, 0.0f, color);
    p_renderer->vertex3(size.x * 0.0f, size.y * 0.0f, 0.0f, color);

    p_renderer->loadIdentity();

    p_renderer->end();
}
