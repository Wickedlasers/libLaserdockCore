#ifndef LASERDOCK_LDSERPENTGAME_H__
#define LASERDOCK_LDSERPENTGAME_H__

#include "ldSnakeObj.h"
#include "ldSerpentMaze.h"
#include "ldSerpentPickup.h"
#include "ldSerpentCommon.h"
#include "ldRazer/ldMesh.h"
#include "ldRazer/ldLaserFont.h"

enum class gameState {
    GS_STARTED,
    GS_PLAYING,
    GS_GROWING,         // Substate of playing
    GS_SHRINKING,       // Ditto
    GS_SLOWING,         // Ditto
    GS_EXTRALIFE,       // Ditto
    GS_DESTROYED,
    GS_PAUSED,
    GS_ENDING,
    GS_LEVEL_COMPLETED,
    GS_LEVEL_STARTING,
    GS_GAME_COMPLETED,
    GS_FINISHED
};

class ldSerpentGame {
public:
    ldSerpentGame();
    ~ldSerpentGame();

    void initialise();

    bool startGame(int16_t lives=3);
    void stopGame();

    void update(float t, float dt);
    void draw();

    bool isPlaying() const { return m_state == gameState::GS_PLAYING ||
                                    m_state == gameState::GS_GROWING ||
                                    m_state == gameState::GS_SHRINKING ||
                                    m_state == gameState::GS_SLOWING ||
                                    m_state == gameState::GS_EXTRALIFE; }
    bool isFinished() const { return m_state == gameState::GS_FINISHED; }
    bool isPaused() const { return m_state == gameState::GS_PAUSED; }

    gameState getState() const { return m_state; }
    void pause() { m_state = gameState::GS_PAUSED; }
    void resume() { m_state = gameState::GS_PLAYING; }
    void reset() { m_state = gameState::GS_FINISHED; }

    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();

    ldRazer::vec2f worldTranslation() const;

    const ldRazer::ldMesh2f_t& getRenderBatch() const { return m_mesh; }

    void setPlaySoundFunction(playSoundFnc&& fnc) { m_playSoundFnc = fnc; }
    void playSoundEvent(SoundEvent event);

protected:
    void snakeDestroyed();
    void pickupCollected();
    void powerupCollected();
    void levelCompleted();
    void gameCompleted();

    void generateLevel();
    void randomGenerator();

    void preDraw();
    void drawTitle();
    void drawGetReady();
    void drawLivesLeft();
    void drawLevelCompleted();
    void drawPaused();
    void drawGameOver();
    void drawWinner();
    void postDraw();
    void checkCollisions();

#if defined(FREESTANDING)
    std::unique_ptr<zap::rndr_batch_t> m_batch;
    zap::rndr_batch_t::token m_token;

    bool zap_init();
    void zap_flush();
#endif

private:
    const int16_t totalLevels = 30;
    const int16_t totalPickupsPerLevel = 20;

    gameState m_state = gameState::GS_FINISHED;
    int16_t m_lives = 0;
    int16_t m_level = 0;
    int16_t m_collected = 0;
    std::unique_ptr<ldSnakeObj> m_snake;
    std::unique_ptr<ldSerpentMaze> m_maze;
    std::array<ldSerpentPickup, 2> m_pickups;   // Max 2 at a time
    ldRazer::ldLaserFont m_font;                // Vector Font for displaying text & info
    ldRazer::ldRandom<float> m_rnd;             // Random generator
    ldRazer::AABB2s_t m_viewport;               // The visible screen in world coordinates
//    ldRazer::AABB2s_t m_worldBound;             // The world bound
//    ldRazer::vec2f m_worldPos;                  // The world position

    playSoundFnc m_playSoundFnc;

    ldRazer::ldMesh2f_t m_mesh;
    float m_currTime, m_triggerTime;
};

#endif //__LASERDOCK_LDSERPENTGAME_H__
