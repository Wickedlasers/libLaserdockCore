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
//  ldGameObject.cpp
//  ldCore
//
//  Created by Glauco Pires (grpires.com) 26/09/2017.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#include "ldCore/Visualizations/Visualizers/Games/Core/ldGameObject.h"
#include <QtCore/QDebug>

// Static values.
int ldGameObject::uuid = 10000;

// Constants.
const float GRAVITY = -2.40f;               // Acceleration used when useGravity is set to true.
const float COLLISION_TOLERANCE = 0.01f;    // Small tolerance used in the vector display to avoid unwanted overlaps.
const int COLLISION_ITERATIONS = 10;        // Amount of iterations when processing a collision.

ldGameObject::ldGameObject() {
    m_id = ldGameObject::uuid++;

    init();
}

ldGameObject::~ldGameObject()
{
}

void ldGameObject::init() {
    enabled = true;
    visible = true;
    expired = false;
    bounded = false;
    useGravity = false;
    gravityScale = 1.0f;

    position = Vec2::zero;
    velocity = Vec2::zero;
    acceleration = Vec2::zero;
    rotation = 0;
}

void ldGameObject::update(float deltaTime) {
    if (!enabled) return;

    // Apply acceleration update.
    velocity.x += (acceleration.x) * deltaTime;
    velocity.y += (acceleration.y + (useGravity ? GRAVITY * gravityScale : 0)) * deltaTime;

    // Apply velocity update.
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;

    // If lifeTimer is greater than 0, then this
    // game object is destroyed after some time.
    if (m_lifeTimer > 0.0f) {
        m_lifeTimer -= deltaTime;
        expired = (m_lifeTimer <= 0.0f);
    }

    // Count for how long this gameobject is alive.
    m_timeAlive += deltaTime;

    if (bounded) {
        // Keep the game object within horizontal bounds.
        if (position.x < -1.0f) {
            position.x = -1.0f;
        } else if (position.x + size.x > 1.0f) {
            position.x = 1.0f - size.x;
        }

        // Keep the game object within vertical bounds.
        if (position.y + size.y > 1.0f) {
            position.y = 1.0f - size.y;
        } else if (position.y < -1.0f) {
            position.y = -1.0f;
        }
    }

    updateGameObject(deltaTime);
}

void ldGameObject::draw(ldRendererOpenlase* p_renderer) {
    if (!visible) return;

    drawGameObject(p_renderer);
}

bool ldGameObject::overlaps(ldGameObject other) {
    if (position.x < other.position.x + other.size.x &&
            position.x + size.x > other.position.x &&
            position.y + COLLISION_TOLERANCE < other.position.y + other.size.y &&
            position.y + size.y - COLLISION_TOLERANCE > other.position.y) {
        return true;
    }

    return false;
}

bool ldGameObject::overlaps(Vec2 otherPosition) {
    if (position.x < otherPosition.x &&
            position.x + size.x > otherPosition.x &&
            position.y + COLLISION_TOLERANCE < otherPosition.y &&
            position.y + size.y - COLLISION_TOLERANCE > otherPosition.y) {
        return true;
    }

    return false;
}

void ldGameObject::collide(ldGameObject other) {
    if (!overlaps(other)) return;

    Vec2 currentPosition = position;
    Vec2 previousPosition = position - velocity;
    Vec2 movementDirection = (position - previousPosition).normalize();
    float distanceTravelled = (position - previousPosition).magnitude();

    int steps = COLLISION_ITERATIONS;

    for (int i = 0; i < steps; i++) {
        float currentStep = (float) i / (steps - 1);

        Vec2 checkPosition = previousPosition + Vec2(movementDirection.x * distanceTravelled * currentStep, movementDirection.y * distanceTravelled * currentStep);

        // Create a dummy GameObject to process collision.
        ldGameObject checker;
        checker.position = checkPosition;
        checker.size = size;

        if (checker.overlaps(other)) {
            // Colliding position. Verify the side and adjust speed.
            float xContact = 0;
            if (position.x > other.position.x && position.x + size.x < other.position.x + other.size.x) {
                xContact = size.x;
            } else if (position.x < other.position.x && position.x + size.x > other.position.x) {
                xContact = position.x + size.x - other.position.x;
            } else if (position.x + size.x > other.position.x + other.size.x && position.x < other.position.x + other.size.x) {
                xContact = other.position.x + other.size.x - (position.x - size.x);
            }

            // Colliding position. Verify the side and adjust speed.
            float yContact = 0;
            if (position.y > other.position.y && position.y + size.y < other.position.y + other.size.y) {
                yContact = size.y;
            } else if (position.y < other.position.y + other.size.y && position.y > other.position.y) {
                yContact = other.position.y + other.size.y - position.y;
            } else if (position.y + size.y > other.position.y && position.y + size.y < other.position.y + other.size.y) {
                yContact = position.y + size.y - other.position.y;
            }

            if (xContact > yContact) {
                // Collision was vertical.
                velocity.y = 0;

                // The horizontal position can be the same one as it was supposed to be.
                position.x = currentPosition.x;

                if (center().y > other.center().y) {
                    // Collision on this object's bottom side.
                    position.y = other.position.y + other.size.y - COLLISION_TOLERANCE;
                } else {
                    // Collision on this object's top side.
                    position.y = other.position.y - size.y + COLLISION_TOLERANCE;
                }
            } else {
                // Collision was horizontal.
                velocity.x = 0;

                // The vertical position can be the same one as it was supposed to be.
                position.y = currentPosition.y;

                if (center().x > other.center().x) {
                    // Collision on this object's left side.
                    position.x = other.position.x + other.size.x + COLLISION_TOLERANCE;
                } else {
                    // Collision on this object's right side.
                    position.x = other.position.x - size.x - COLLISION_TOLERANCE;
                }
            }

            return;
        }
    }
}

void ldGameObject::setLifetime(float duration) {
    m_lifeTimer = duration;
    m_totalLifetime = duration;
}

float ldGameObject::getLifetimePercentage() {
    return std::max(m_lifeTimer, 0.0f) / m_totalLifetime;
}

float ldGameObject::getTimeAlive() {
    return m_timeAlive;
}

int ldGameObject::getId() const {
    return m_id;
}

Vec2 ldGameObject::center() {
    return Vec2(position.x + size.x * 0.5f, position.y + size.y * 0.5f);
}

void ldGameObject::updateGameObject(float /*deltaTime*/) {}
void ldGameObject::drawGameObject(ldRendererOpenlase* /*p_renderer*/) {}

/*
 * Draw utils
 */
void ldGameObject::drawVertexRainbow(ldRendererOpenlase* p_renderer, QList<Vec2> vertices, QList<int> colors, int segmentsPerLine, int repeat) {
    // Remember to call p_renderer->begin(OL_LINESTRIP);

    int segmentsAmount = segmentsPerLine * (vertices.length() - 1);

    for (int i = 1; i < vertices.length(); i++) {
        for (int j = 0; j < segmentsPerLine; j++) {
            float step = (float) j / (segmentsPerLine - 1);
            float colorStep = (float) (j + ((i - 1) * segmentsPerLine)) / segmentsAmount;

            Vec2 target = vertices[i];
            Vec2 origin = vertices[i - 1];

            int startColorIndex = fmin(floorf(colorStep * colors.length()), colors.length() - 2);

            int startColor = colors[startColorIndex];
            int finalColor = colors[startColorIndex + 1];

            float x = origin.x * (1 - step) + target.x * (step);
            float y = origin.y * (1 - step) + target.y * (step);
            uint32_t color = ldColorUtil::lerpInt(startColor, finalColor, colorStep);

            int localRepeat = 1;
            if(repeat > 1) {
                bool isFirstSegment = (j == 0);
                bool isLastSegment = (j == segmentsPerLine - 1) ;
                if(isFirstSegment || isLastSegment) {
                    bool isFirstVertex = (i == 1);
                    bool isLastVertex = (i == vertices.length() - 1);

                    if(isFirstVertex && isFirstSegment) {
                        localRepeat = repeat / 2;
                    } else if(isLastSegment && isLastVertex) {
                        localRepeat = repeat / 2;
                    } else {
                        localRepeat = repeat;
                    }
                }
            }

            p_renderer->vertex(x, y, color, localRepeat);
        }
    }

    // Remember to call p_renderer->end();
}
