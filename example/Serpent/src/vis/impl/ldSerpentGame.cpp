#include "ldSerpentGame.h"
#include "ldSerpentMazeGenerator.h"
#include "ldRazer/ldQueries.h"
#include "ldRazer/ldLaserFont.h"

using namespace ldRazer;

ldSerpentGame::ldSerpentGame() = default;
ldSerpentGame::~ldSerpentGame() = default;

void ldSerpentGame::initialise() {
}

bool ldSerpentGame::startGame(int16_t lives) {
    if(m_state != gameState::GS_FINISHED) return false;
    m_viewport.setCorners(vec2s(0, 0), vec2s(VIEWPORT_SIZE, VIEWPORT_SIZE));
    playSoundEvent(SoundEvent::SE_GAME_START);

    m_level = 1;
    m_lives = lives;
    m_collected = 0;

    m_snake = std::unique_ptr<ldSnakeObj>(new ldSnakeObj(vec2s(3, 3)));
    generateLevel();
    m_pickups[0].reset(m_maze->generatePosition(), ldPickupType::PT_TARGET, 0.f);
    m_pickups[0].setPosition(m_maze->generatePosition(vec2s(1, 1)));
    m_state = gameState::GS_STARTED;
    m_currTime = 0.f;
    m_triggerTime = 1.f;

    return true;
}

void ldSerpentGame::generateLevel() {
    // Go from level 1 to 30
    m_maze = ldSerpentMazeGenerator::generate(VIEWPORT_SIZE, VIEWPORT_SIZE, m_level/float(totalLevels));
    m_maze->setSnake(m_snake.get());
}

void ldSerpentGame::randomGenerator() {
    if(m_pickups[1].isVisible()) return;

    // TODO: move to table calculated on level up
    static const auto extra_life_chance = 1.f/(50000.f*(std::max(m_level/5, 1)));
    static const auto slow_chance = 1.f/(18000.f*(std::max(m_level/5, 1))), slow_top = (extra_life_chance + slow_chance);
    static const auto reduce_chance = 1.f/(12000.f*(std::max(m_level/5, 1))), reduce_top = (slow_top + reduce_chance);
    const auto value = m_rnd.random1();

    ldPickupType type = ldPickupType::PT_TARGET;
    if(value < extra_life_chance) {
        type = ldPickupType::PT_EXTRA_LIFE;
    } else if(value >= extra_life_chance && value < slow_top) {
        type = ldPickupType::PT_SLOW;
    } else if(value >= slow_top && value < reduce_top) {
        type = ldPickupType::PT_SHRINK;
    }

    if(type != ldPickupType::PT_TARGET) {
        m_pickups[1].setSnakePosition(m_snake->getWorldTranslation());
        m_pickups[1].reset(m_maze->generatePosition(), type, m_rnd.random1(10.f, 20.f));
    }
}

void ldSerpentGame::stopGame() {
    m_state = gameState::GS_FINISHED;
}

void ldSerpentGame::update(float, float dt) {
    m_currTime += dt;

    switch(m_state) {
        case gameState::GS_STARTED:
        case gameState::GS_LEVEL_COMPLETED:
            if(m_currTime >= m_triggerTime) {
                m_state = gameState::GS_LEVEL_STARTING;
                m_triggerTime = 1.f;
                m_currTime = 0.f;
            }
            break;
        case gameState::GS_GROWING:
        case gameState::GS_SHRINKING:
        case gameState::GS_SLOWING:
        case gameState::GS_EXTRALIFE:
            break;
        case gameState::GS_PLAYING:
            // When switching to the playing state, show the screen for a short while before play
            if(m_currTime >= m_triggerTime) {
                randomGenerator();
                m_snake->update(dt);
                for(auto& pickup : m_pickups) {
                    if(!pickup.isVisible()) continue;
                    pickup.update(dt);
                    pickup.setSnakePosition(m_snake->getWorldTranslation());
                }
                checkCollisions();
            } else {
                // We need to prepare the mesh for rendering, so update with 0.f
                m_snake->update(0.f);
                for(auto& pickup : m_pickups) {
                    pickup.update(0.f);
                    pickup.setSnakePosition(m_snake->getWorldTranslation());
                }
            };
            break;
        case gameState::GS_PAUSED:
            break;
        case gameState::GS_DESTROYED:
            if(m_currTime >= m_triggerTime) {
                if(m_lives == 0) {
                    m_triggerTime = 1.f;
                    m_state = gameState::GS_ENDING;
                    m_currTime = 0.f;
                } else {
                    m_triggerTime = .5f;
                    m_currTime = 0.f;
                    m_state = gameState::GS_LEVEL_STARTING;
                }
            }
            break;
        case gameState::GS_ENDING:
            if(m_currTime >= m_triggerTime) {
                m_state = gameState::GS_FINISHED;
                m_currTime = 0.f;
                m_triggerTime = 0.f;
            }
            break;
        case gameState::GS_LEVEL_STARTING:
            if(m_currTime >= m_triggerTime) {
                m_state = gameState::GS_PLAYING;
                m_triggerTime = .5f;
                m_currTime = 0.f;
            }
            break;
        case gameState::GS_GAME_COMPLETED:
            if(m_currTime >= m_triggerTime) {
                m_state = gameState::GS_FINISHED;
                m_triggerTime = 0.f;
                m_currTime = 0.f;
            }
            break;
        case gameState::GS_FINISHED:
            m_triggerTime = 0.f;
            m_currTime = 0.f;
            break;
    }
}

void ldSerpentGame::draw() {
    preDraw();

    m_mesh.vertices.clear();
    m_mesh.indices.clear();

    switch(m_state) {
        case gameState::GS_STARTED:
        case gameState::GS_FINISHED:
            drawTitle();
            break;
        case gameState::GS_PLAYING:
            m_maze->draw(m_mesh);
            m_snake->draw(m_mesh);
            m_pickups[0].draw(m_mesh);
            if(m_pickups[1].isVisible()) m_pickups[1].draw(m_mesh);
            break;
        case gameState::GS_PAUSED:
            drawPaused();
            break;
        case gameState::GS_DESTROYED:
            m_lives == 0 ? drawGameOver() : drawLivesLeft();
            break;
        case gameState::GS_GROWING:
            break;
        case gameState::GS_SHRINKING:
            break;
        case gameState::GS_SLOWING:
            break;
        case gameState::GS_EXTRALIFE:
            break;
        case gameState::GS_ENDING:
            drawGameOver();
            break;
        case gameState::GS_LEVEL_STARTING:
            drawGetReady();
            break;
        case gameState::GS_LEVEL_COMPLETED:
            drawLevelCompleted();
            break;
        case gameState::GS_GAME_COMPLETED:
            drawWinner();
            break;
    }

    postDraw();
}

void ldSerpentGame::preDraw() {
    m_mesh.indices.clear();
    m_mesh.vertices.clear();

#if defined(FREESTANDING)
    m_batch->bind();
#endif
}

void ldSerpentGame::drawTitle() {
    // Draw a string
    m_font.setScale(.1f);
    auto bound = m_font.getStringAABB("Serpent");
    auto mesh = m_font.buildString(-bound.halfExtent(), "Serpent",
                                   NO_RED_LASER ? makeARGB(0, 0, 255) : makeARGB(255, 0, 0), true);
    mesh = fadeOut(mesh, makeARGB(0, 0, 20), eq(m_triggerTime, 0.f) ? 0.f : m_currTime/m_triggerTime);
    m_mesh += mesh;
}

void ldSerpentGame::drawGetReady() {
    m_font.setScale(.1f);
    auto bound = m_font.getStringAABB("Get Ready");
    auto mesh = m_font.buildString(-bound.halfExtent(), "Get Ready",
                       NO_RED_LASER ? makeARGB(0, 255, 255) : makeARGB(255, 0, 0), true);
    mesh = fadeOut(mesh, makeARGB(0, 0, 20), eq(m_triggerTime, 0.f) ? 0.f : m_currTime/m_triggerTime);
    m_mesh += mesh;
}

void ldSerpentGame::drawLivesLeft() {
    m_font.setScale(.1f);
    const auto str = (m_lives == 1 ? "1 Life" : lexical_cast(m_lives) + " Lives");
    auto bound = m_font.getStringAABB(str);
    auto mesh = m_font.buildString(-bound.halfExtent(), str,
                       NO_RED_LASER ? makeARGB(0, 255, 255) : makeARGB(255, 255, 0), true);
    mesh = fadeOut(mesh, makeARGB(0, 0, 20), eq(m_triggerTime, 0.f) ? 0.f : m_currTime/m_triggerTime);
    m_mesh += mesh;
}

void ldSerpentGame::drawLevelCompleted() {
    m_font.setScale(.05f);
    const auto str = "Level " + lexical_cast(m_level-1) + " Complete";
    auto bound = m_font.getStringAABB(str);
    m_font.buildString(m_mesh, -bound.halfExtent(), str,
                       NO_RED_LASER ? makeARGB(0, 255, 255) : makeARGB(255, 255, 0), true);
}

void ldSerpentGame::drawPaused() {
    m_font.setScale(.1f);
    auto bound = m_font.getStringAABB("Paused");
    m_font.buildString(m_mesh, -bound.halfExtent(), "Paused",
                       NO_RED_LASER ? makeARGB(0, 255, 255) : makeARGB(255, 0, 0), true);
}

void ldSerpentGame::drawWinner() {
    m_font.setScale(.1f);
    auto bound = m_font.getStringAABB("You Win!");
    m_font.buildString(m_mesh, -bound.halfExtent(), "You Win!",
                       NO_RED_LASER ? makeARGB(0, 255, 255) : makeARGB(255, 0, 0), true);
}

void ldSerpentGame::drawGameOver() {
    m_font.setScale(.1f);
    auto bound = m_font.getStringAABB("Game Over");
    m_font.buildString(m_mesh, -bound.halfExtent(), "Game Over",
                       NO_RED_LASER ? makeARGB(0, 255, 255) : makeARGB(255, 0, 0), true);
}

void ldSerpentGame::postDraw() {
}

void ldSerpentGame::moveLeft() {
    if(m_snake) m_snake->move(vec2s(-1, 0));
}

void ldSerpentGame::moveRight() {
    if(m_snake) m_snake->move(vec2s(1, 0));
}

void ldSerpentGame::moveUp() {
    if(m_snake) m_snake->move(vec2s(0, 1));
}

void ldSerpentGame::moveDown() {
    if(m_snake) m_snake->move(vec2s(0, -1));
}

vec2f ldSerpentGame::worldTranslation() const {
    return m_snake && m_state == gameState::GS_PLAYING ? m_snake->getWorldTranslation() : vec2f(0.f, 0.f);
}

void ldSerpentGame::playSoundEvent(SoundEvent event) {
    if(m_playSoundFnc) m_playSoundFnc(event);
}

void ldSerpentGame::snakeDestroyed() {
    m_state = gameState::GS_DESTROYED;
    m_lives = std::max<int16_t>(m_lives-1, 0);
    if(m_lives > 0) playSoundEvent(SoundEvent::SE_SNAKE_DIE);
    m_snake->reset(m_maze->generatePosition(vec2s{3, 3}));
    m_collected = 0;
    m_triggerTime = 1.f;
    m_currTime = 0.f;
}

void ldSerpentGame::levelCompleted() {
    m_level++;
    if(m_level == totalLevels) gameCompleted();
    else {
        m_state = gameState::GS_LEVEL_COMPLETED;
        playSoundEvent(SoundEvent::SE_LEVEL_COMPLETED);
        generateLevel();
        m_maze->setSnake(m_snake.get());
        m_snake->reset(m_maze->generatePosition(vec2s{3, 3}));
        m_pickups[0].setPosition(m_maze->generatePosition());
        m_pickups[1].setVisible(false);
        m_collected = 0;
        m_triggerTime = 1.f;
        m_currTime = 0.f;
    }
}

void ldSerpentGame::pickupCollected() {
    m_collected++;
    if(m_collected == totalPickupsPerLevel) {
        levelCompleted();
    } else {
        playSoundEvent(SoundEvent::SE_PICKUP_TARGET);
        m_snake->increaseLength();
        m_snake->increaseSpeed();
        m_pickups[0].setPosition(m_maze->generatePosition());
    }
}

void ldSerpentGame::powerupCollected() {
    switch(m_pickups[1].getType()) {
        case ldPickupType::PT_SHRINK:
            m_snake->decreaseLength(std::max(1, m_snake->getLength()/2));
            playSoundEvent(SoundEvent::SE_PICKUP_SHRINK);
            break;
        case ldPickupType::PT_SLOW:
            m_snake->setSpeed(std::max(1, m_snake->getSpeed()/2));
            playSoundEvent(SoundEvent::SE_PICKUP_SLOW);
            break;
        case ldPickupType::PT_EXTRA_LIFE:
            playSoundEvent(SoundEvent::SE_PICKUP_EXTRALIFE);
            m_lives++;
            break;
        case ldPickupType::PT_TARGET:
            break;
    }
    m_pickups[1].setVisible(false);
}

void ldSerpentGame::gameCompleted() {
    m_state = gameState::GS_GAME_COMPLETED;
    m_triggerTime = 4.f;
    m_currTime = 0.f;
}

void ldSerpentGame::checkCollisions() {
    const auto snakeHead = m_snake->getHead();
    if(m_snake->isSelfIntersection() || m_maze->intersection(snakeHead)) snakeDestroyed();
    if(m_pickups[0].intersection(snakeHead)) pickupCollected();
    if(m_pickups[1].isVisible() && m_pickups[1].intersection(snakeHead)) powerupCollected();
}

