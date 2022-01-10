//
//  ldSerpentVisualizer.cpp
//  LaserdockVisualizer
//
//  Created by Darren Otgaar 16/08/2018
//  Copyright (c) 2018 Wicked Lasers. All rights reserved.


#include "ldSerpentVisualizer.h"

#include "ldCore/ldCore.h"

#include "impl/ldSerpentCommon.h"
#include "impl/ldSerpentGame.h"

using namespace ldRazer;

/* Note:  There is a timer issue in the game code.  The QTimer is not a high-resolution or reliable timer.  I've adapted
 * the code to use a high-resolution C++ timer (C++11).
 */

ldSerpentVisualizer::ldSerpentVisualizer() : ldAbstractGameVisualizer() {
//    setPosition(ccp(1, 1));
    m_soundEffects.insert(SoundEvent::SE_PICKUP_TARGET, ldCore::instance()->resourceDir() + "/sound/serpent/PICKUP_TARGET.wav");
    m_soundEffects.insert(SoundEvent::SE_PICKUP_SHRINK, ldCore::instance()->resourceDir() + "/sound/serpent/PICKUP_SHRINK.wav");
    m_soundEffects.insert(SoundEvent::SE_PICKUP_SLOW, ldCore::instance()->resourceDir() + "/sound/serpent/PICKUP_SLOW.wav");
    m_soundEffects.insert(SoundEvent::SE_PICKUP_EXTRALIFE, ldCore::instance()->resourceDir() + "/sound/serpent/PICKUP_EXTRALIFE.wav");
    m_soundEffects.insert(SoundEvent::SE_GAME_START, ldCore::instance()->resourceDir() + "/sound/serpent/GAME_START.wav");
    m_soundEffects.insert(SoundEvent::SE_SNAKE_DIE, ldCore::instance()->resourceDir() + "/sound/serpent/SNAKE_DIE.wav");
    m_soundEffects.insert(SoundEvent::SE_LEVEL_COMPLETED, ldCore::instance()->resourceDir() + "/sound/serpent/LEVEL_COMPLETED.wav");

    m_game = std::unique_ptr<ldSerpentGame>(new ldSerpentGame());
    m_game->setPlaySoundFunction([&](const SoundEvent& e) { m_soundEffects.play(e); });
    m_game->initialise();
}

ldSerpentVisualizer::~ldSerpentVisualizer() = default;

void ldSerpentVisualizer::moveLeft(bool /*keyPress*/) {
    QMutexLocker lock(&m_mutex);
    m_game->moveLeft();
}

void ldSerpentVisualizer::moveRight(bool /*keyPress*/) {
    QMutexLocker lock(&m_mutex);
    m_game->moveRight();
}

void ldSerpentVisualizer::moveUp(bool /*keyPress*/) {
    QMutexLocker lock(&m_mutex);
    m_game->moveUp();
}

void ldSerpentVisualizer::moveDown(bool /*keyPress*/) {
    QMutexLocker lock(&m_mutex);
    m_game->moveDown();
}

void ldSerpentVisualizer::moveX(double x)
{
    QMutexLocker lock(&m_mutex);
    if(x < -0.4)
        m_game->moveLeft();
    else if(x > 0.4)
        m_game->moveRight();
}

void ldSerpentVisualizer::moveY(double y)
{
    QMutexLocker lock(&m_mutex);
    if(y < -0.4)
        m_game->moveDown();
    else if(y > 0.4)
        m_game->moveUp();
}

// draw
void ldSerpentVisualizer::draw() {
    QMutexLocker lock(&m_mutex);

    if(!m_game->isFinished()) {
        m_game->update(m_gameTimer, m_renderer->getLastFrameDeltaSeconds());
        if(m_game->isFinished())
            setGameOver();
    }

    m_game->draw();

    const auto inv_scale = 2.f/VIEWPORT_SIZE;
    const auto translation = vec2f{1.f, 1.f};
    const auto wP = -m_game->worldTranslation();

    const auto& mesh = m_game->getRenderBatch();
    for(const auto& idx : mesh.indices) {
        m_renderer->begin(OL_LINESTRIP);
        for (uint32_t i = idx.start, end = idx.start + idx.count; i != end; ++i) {
            const auto lP = inv_scale * (mesh.vertices[i].position + wP) + translation;

            if(idx.strict) {    // Introduce a strict mode to insert extra points as per ldRendererOpenlase::rect
                if(i == idx.start) m_renderer->vertex(lP.x, lP.y, mesh.vertices[i].colour, 3);
                else if(i == idx.start+idx.count) m_renderer->vertex(lP.x, lP.y, mesh.vertices[i].colour, 4);
                else m_renderer->vertex(lP.x, lP.y, mesh.vertices[i].colour, 7);
            } else {
                // Smooth interpolation
                if(i == idx.start) m_renderer->vertex(lP.x, lP.y, mesh.vertices[i].colour, 2);
                else if(i == idx.start+idx.count) m_renderer->vertex(lP.x, lP.y, mesh.vertices[i].colour, 3);
                else m_renderer->vertex(lP.x, lP.y, mesh.vertices[i].colour, 1);
            }
        }
        m_renderer->end();
    }
}

void ldSerpentVisualizer::onGameReset()
{
    m_game->reset();
}

void ldSerpentVisualizer::onGamePlay()
{
    if(m_game->isFinished()) {
        m_game->startGame(3);
    } else {
        m_game->resume();
    }
}

void ldSerpentVisualizer::onGamePause()
{
    if(m_game->isPlaying()) m_game->pause();
}
