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
//  ldAbstractGameVisualizer.cpp
//  ldCore
//
//  Created by Sergey Gavrushkin 26/05/2017
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#include "ldAbstractGameVisualizer.h"

#include "ldCore/Visualizations/util/TextHelper/ldTextLabel.h"



//     default reset time in debug mode is 1 sec for quick testing
#ifdef LD_BUILD_RELEASE_VERSION
    const int ldAbstractGameVisualizer::GAME_DEFAULT_RESET_TIME = 3;
#else
    const int ldAbstractGameVisualizer::GAME_DEFAULT_RESET_TIME = 1;
#endif

// ldAbstractGameVisualizer
ldAbstractGameVisualizer::ldAbstractGameVisualizer()
    : ldVisualizer()
    , m_mutex(QMutex::Recursive)
{
    m_rate = 20000;

    m_isMusicAware = false;
    // Current game state label.
    m_messageLabel.reset(new ldTextLabel());
    m_messageLabel->setColor(0xFFFFFF);
}

ldAbstractGameVisualizer::~ldAbstractGameVisualizer()
{
}

void ldAbstractGameVisualizer::setSoundEnabled(bool enabled)
{
    m_soundEffects.setSoundEnabled(enabled);
}

void ldAbstractGameVisualizer::setSoundLevel(int soundLevel)
{
    m_soundEffects.setSoundLevel(soundLevel);
}

void ldAbstractGameVisualizer::draw() {
    ldVisualizer::draw();

    float deltaTime = m_renderer->getLastFrameDeltaSeconds();

    // Update game timer.
    if (m_state == State::Playing) {
        m_gameTimer += deltaTime;
    }

    // Update explosions.
    for (int i = 0; i < m_explosions.length(); i++) {
        ldGameExplosion &explosion = m_explosions[i];

        explosion.update(deltaTime);
        explosion.draw(m_renderer);

        if (explosion.expired) {
            m_explosions.removeAt(i--);
        }
    }

    // Update fireworks.
    for (int i = 0; i < m_fireworks.length(); i++) {
        ldGameFirework &firework = m_fireworks[i];

        firework.update(deltaTime);
        firework.draw(m_renderer);

        if (firework.expired) {
            m_fireworks.removeAt(i--);
        }
    }

    // Update sparkles.
    for (int i = 0; i < m_sparkles.length(); i++) {
        ldGameSparkle &sparkle = m_sparkles[i];

        sparkle.update(deltaTime);
        sparkle.draw(m_renderer);

        if (sparkle.expired) {
            m_sparkles.removeAt(i--);
        }
    }

    // Update smokes.
    for (int i = 0; i < m_smokes.length(); i++) {
        ldGameSmoke &smoke = m_smokes[i];

        smoke.update(deltaTime);
        smoke.draw(m_renderer);

        if (smoke.expired) {
            m_smokes.removeAt(i--);
        }
    }

    // Update message label.
    m_messageLabel->innerDraw(m_renderer);
    if (m_messageLabelTimer > 0) {
        m_messageLabelTimer -= deltaTime;
        if (m_messageLabelTimer <= 0) {
            clearMessage();
        }
    }
}

void ldAbstractGameVisualizer::addExplosion(Vec2 position, int color, float size) {
    m_explosions.append(ldGameExplosion(position, color, size));
}

void ldAbstractGameVisualizer::addFireworks(Vec2 position, int amount) {
    for (int i = 0; i < amount; i++) {
        float step = (float) i / (amount - 1);
        float angle = 2 * M_PI * step;
        float noise = M_PIf * 0.2f;
        float finalAngle = angle + (-noise + 2 * noise * ((rand() % 100) / 100.0f));

        m_fireworks.append(ldGameFirework(position, finalAngle));
    }
}

void ldAbstractGameVisualizer::addSparkle(Vec2 position) {
    m_sparkles.append(ldGameSparkle(position));
}

void ldAbstractGameVisualizer::addSmoke(Vec2 position) {
    m_smokes.append(ldGameSmoke(position));
}

/*
 * Label functions.
 */

// Set the label's text and horizontally align it.
void ldAbstractGameVisualizer::showMessage(string text, float duration) {
    m_messageLabel->setText(text);
    float labelWidth = m_messageLabel->getWidth();
    m_messageLabel->setPosition(Vec2(0.5f - (labelWidth/2.0f), 0.6f));

    m_messageLabelTimer = duration;
}

// Clears the displayed message and its timer.
void ldAbstractGameVisualizer::clearMessage() {
    m_messageLabel->setText("");
    m_messageLabelTimer = 0.0f;
}

/*
 * Draw utils
 */
void ldAbstractGameVisualizer::drawVertexRainbow(ldRendererOpenlase* p_renderer, QList<Vec2> vertices, QList<int> colors, int segmentsPerLine, int repeat) {
    // Remember to call p_renderer->begin(OL_LINESTRIP);

    int segmentsAmount = segmentsPerLine * (vertices.length() - 1);

    for (int i = 1; i < vertices.length(); i++) {
        for (int j = 0; j < segmentsPerLine; j++) {
            float step = (float) j / (segmentsPerLine - 1);
            float colorStep = (float) (j + ((i - 1) * segmentsPerLine)) / segmentsAmount;

            Vec2 target = vertices[i];
            Vec2 origin = vertices[i - 1];

            int startColorIndex = fmin(floorf(colorStep * colors.length()), colors.length() - 2);

            int startColor = colors[max(startColorIndex, 0)];
            int finalColor = colors[min(startColorIndex + 1, colors.length() - 1)];

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
