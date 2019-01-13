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

#include "ldCore/Visualizations/Visualizers/Games/ldAbstractGameVisualizer.h"

#include "ldCore/Helpers/Text/ldTextLabel.h"



//     default reset time in debug mode is 1 sec for quick testing
#ifdef LD_BUILD_RELEASE_VERSION
const int ldAbstractGameVisualizer::GAME_DEFAULT_RESET_TIME = 3;
#else
const int ldAbstractGameVisualizer::GAME_DEFAULT_RESET_TIME = 1;
#endif

// convert line list of vertex bricks to plain closed path
QList<QList<Vec2> > ldAbstractGameVisualizer::lineListToVertexShapes(const QList<Vec2> &lineListBricksInput, float precise)
{
    QList<Vec2> lineListBricks = lineListBricksInput;
    int i = 0;
    QList<QList<Vec2>> shapes;
    QList<Vec2> currentShape;

    // iterate over 'bricks' and find plain vertex path
    while(!lineListBricks.empty()) {
        // if we reach end of 'bricks' and didn't find next point create new shape
        if(i == lineListBricks.size()) {
            // add new shape
            shapes.push_back(currentShape);
            // start new one
            currentShape.clear();
            // start search from begin
            i = 0;
            continue;
        }

        const Vec2 &brickStart = lineListBricks[i];

        // add first 2 points in a raw
        if(currentShape.length() < 2) {
            // add point
            currentShape.append(brickStart);
            // remove 'brick' point
            lineListBricks.removeAt(i);
            // start search from begin
            i = 0;
            continue;
        } else {
            const Vec2 &lastPoint =  currentShape[currentShape.length() - 1];
            const Vec2 &brickEnd = lineListBricks[i+1];

            // if we find next 'brick' remember it's path and go to next iteration
            bool isEqualsBrickStart = cmpf(lastPoint.x, brickStart.x, precise) && cmpf(lastPoint.y, brickStart.y, precise);
            bool isEqualsBrickEnd = cmpf(lastPoint.x, brickEnd.x, precise) && cmpf(lastPoint.y, brickEnd.y, precise);

            if(isEqualsBrickStart
                    || isEqualsBrickEnd) {
                // check what we should remember
                if(isEqualsBrickStart) {
                    currentShape.append(brickEnd);
                } else {
                    currentShape.append(brickStart);
                }

                // remove 'brick'
                lineListBricks.removeAt(i);
                lineListBricks.removeAt(i);

                // start search from begin
                i = 0;
                continue;
            }
        }

        // go to next brick
        i += 2;
    }

    // add last part
    if(!currentShape.empty())
        shapes.push_back(currentShape);

    return shapes;
}

QList<QList<Vec2> > ldAbstractGameVisualizer::optimizeShapesToLaser(const QList<QList<Vec2>> &linePathParts, int repeat)
{
    QList<QList<Vec2>> laserLinePathParts;

    //  current path direction - x or y
    bool isCurrentYAxis = false;

    int repeatFirstLast = (repeat+1) / 2;

    for(const QList<Vec2> &line : linePathParts) {
        QList<Vec2> laserLinePath;

        for(const Vec2 &point : line) {

            // add first 2 points in plain order
            if(laserLinePath.size() < 2) {
                // add first point twice
                if(laserLinePath.size() == 1) {
                    for(int i = 0; i < repeatFirstLast; i++) {
                        laserLinePath.push_back(laserLinePath.first());
                    }
                }

                // add point
                laserLinePath.push_back(point);

                // current path direction
                isCurrentYAxis = cmpf(laserLinePath.first().y, point.y);
                continue;
            }

            // check direction of new point
            const Vec2 &lastBrick = laserLinePath.last();
            bool isNextYAxis = cmpf(lastBrick.y, point.y);

            // if we change direction add extra point to this angle
            if(isNextYAxis != isCurrentYAxis) {
                for(int i = 0; i < repeat; i ++) {
                    laserLinePath.push_back(lastBrick);
                }
            }

            // add point
            laserLinePath.push_back(point);

            // and save current path direction
            isCurrentYAxis = isNextYAxis;
        }

        // add last point twice
        for(int i = 0; i < repeatFirstLast; i++) {
            laserLinePath.push_back(laserLinePath.last());
        }

        // remember in parts
        laserLinePathParts.push_back(laserLinePath);
    }

    return laserLinePathParts;
}


// ldAbstractGameVisualizer
ldAbstractGameVisualizer::ldAbstractGameVisualizer()
    : ldVisualizer()
    , m_mutex(QMutex::Recursive)
{
    //    m_rate = 20000;

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
    QMutexLocker locker(&m_mutex);
    m_soundEffects.setSoundEnabled(enabled);
}

void ldAbstractGameVisualizer::setSoundLevel(int soundLevel)
{
    QMutexLocker locker(&m_mutex);
    m_soundEffects.setSoundLevel(soundLevel);
}

float ldAbstractGameVisualizer::complexity() const
{
    return m_complexity;
}

int ldAbstractGameVisualizer::levelIndex() const
{
    return 0;
}

QStringList ldAbstractGameVisualizer::levelList() const
{
    return QStringList();
}

void ldAbstractGameVisualizer::setComplexity(float complexity)
{
    QMutexLocker locker(&m_mutex);

    m_complexity = complexity;
}


void ldAbstractGameVisualizer::setLevelIndex(int /*index*/)
{
}

void ldAbstractGameVisualizer::nextLevel()
{
    QMutexLocker lock(&m_mutex);

    int nextLevelIndex = levelIndex() + 1;
    int levelCount = levelList().size();
    if(nextLevelIndex >= levelCount)
        nextLevelIndex = 0;

    setLevelIndex(nextLevelIndex);
}

void ldAbstractGameVisualizer::previousLevel()
{
    QMutexLocker lock(&m_mutex);

    int levelCount = levelList().size();

    int nextLevelIndex = levelIndex() - 1;
    if(nextLevelIndex < 0)
        nextLevelIndex = (levelCount > 0) ? (levelCount - 1) : 0;

    setLevelIndex(nextLevelIndex);
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
void ldAbstractGameVisualizer::showMessage(const QString &text, float duration) {
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
