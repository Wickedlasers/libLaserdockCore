//
//  ldSpiralFighterVisualizer.cpp
//  LaserdockVisualizer
//
//  Created by Glauco Pires 20/10/2017.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#include "ldSpiralFighterVisualizer.h"

#include <math.h>

#include <QtCore/QTime>
#include <QtCore/QDebug>
#include <QtCore/QString>

#include "ldCore/ldCore.h"
#include "ldCore/Helpers/Text/ldTextLabel.h"

// Constants.
const int LEVEL_AMOUNT = 3;

const float LEVEL_ANGULAR_SPEED = M_PIf * 0.1f;
const float LEVEL_ANGULAR_SPEED_MULTIPLIER_LEVEL = 0.1f;
const float LEVEL_ANGULAR_SPEED_MULTIPLIER_WORLD = 0.4f;

const float TIME_TO_SHRINK = 2.0f;
const float SHRINK_AMOUNT = 0.05f;

const float TIME_TO_SPAWN_STREAM = 2.0f;
const float INTERVAL_BETWEEN_ENEMIES = 0.8f;

const float DISTANCE_TO_ROTATE = 0.4f;

// ldSpiralFighterVisualizer
ldSpiralFighterVisualizer::ldSpiralFighterVisualizer() : ldAbstractGameVisualizer()
{
    // Register sounds.
    m_soundEffects.insert(SFX::EXPLOSION, ldCore::instance()->resourceDir() + "/sound/shexplosion.wav");
    m_soundEffects.insert(SFX::FIRE, ldCore::instance()->resourceDir() + "/sound/shoot.wav");
    m_soundEffects.insert(SFX::NEWLEVEL, ldCore::instance()->resourceDir() + "/sound/newlevel.wav");
    m_soundEffects.insert(SFX::POWERUP, ldCore::instance()->resourceDir() + "/sound/powerup.wav");

    // Game score label.
    m_scoreLabel.reset(new ldTextLabel());
    m_scoreLabel->setColor(0xFFFFFF);

    // Current game state label.
    m_stateLabel.reset(new ldTextLabel());
    m_stateLabel->setColor(0xFFFFFF);

    // Set the timer for the initial countdown.
    m_countdownTimer.setInterval(1000);
    connect(&m_countdownTimer, &QTimer::timeout, this, &ldSpiralFighterVisualizer::onTimerTimeout);

    init();

    connect(this, &ldSpiralFighterVisualizer::scoreChanged, this, &ldSpiralFighterVisualizer::updateScoreLabel);
}

ldSpiralFighterVisualizer::~ldSpiralFighterVisualizer() {
}

/*
 * Main game functions.
 */

// Start a new game.
void ldSpiralFighterVisualizer::resetMatch() {
    m_isReset = true;
    m_isPaused = false;

    setStateText("");

    startCountdownTimer();

    // Reset variables.
    m_isGameOver = false;
    m_gameTimer = 0;
    m_score = 0;
    emit scoreChanged(0);

    // Reset game variables.
    m_player.rotation = 0.0f;
    m_player.releaseKeys();
    m_player.onFire = [this](ldSpiralFighterPlayer *player) { return onPlayerFire(player); };
    m_player.onPowerup = [this]() { return onPlayerPowerup(); };
    m_player.visible = true;
    m_levelIndex = -1;
    m_worldIndex = 0;
    m_shrinkTimer = TIME_TO_SHRINK;
    m_streamSpawnTimer = TIME_TO_SPAWN_STREAM;
    m_currentStreamSpawnPointIndex = 0;
    m_enemiesPerStream = 1;

    // Clear lists.
    m_enemies.clear();
    m_missiles.clear();
    m_spawnPoints.clear();
    m_spawnPointOrigins.clear();

    loadNewLevel();
}

void ldSpiralFighterVisualizer::loadNewLevel() {
    if (m_levelIndex == LEVEL_AMOUNT - 1) {
        m_worldIndex++;
        m_player.givePowerup();
    }

    m_levelIndex = (m_levelIndex + 1) % LEVEL_AMOUNT;
    m_levelRotationDirection = rand() % 2 == 0 ? 1.0f : -1.0f;

    m_enemiesToDestroy = 0;
    m_enemiesPerStream = 0;
    m_currentStreamSpawnPointIndex = 0;

    m_enemies.clear();
    m_missiles.clear();
    m_spawnPoints.clear();
    m_spawnPointOrigins.clear();

    // Create the level.
    if (m_levelIndex == 0) {
        m_enemiesToDestroy = 15;
        m_enemiesPerStream = 2;

        m_spawnPoints = QList<Vec2>();
        m_spawnPoints.append(Vec2(-0.8f, 0.2f));
        m_spawnPoints.append(Vec2(0.8f, 0.7f));
        m_spawnPoints.append(Vec2(-0.2f, -0.8f));
    } else if (m_levelIndex == 1) {
        m_enemiesToDestroy = 25;
        m_enemiesPerStream = 3;

        m_spawnPoints = QList<Vec2>();
        m_spawnPoints.append(Vec2(0.8f, 0.0f));
        m_spawnPoints.append(Vec2(0.0f, 0.8f));
        m_spawnPoints.append(Vec2(-0.8f, 0.0f));
        m_spawnPoints.append(Vec2(0.0f, -0.8f));
    } else if (m_levelIndex == 2) {
        m_enemiesToDestroy = 40;
        m_enemiesPerStream = 4;

        m_spawnPoints = QList<Vec2>();
        m_spawnPoints.append(Vec2(-0.8f, 0.2f));
        m_spawnPoints.append(Vec2(0.0f, 0.8f));
        m_spawnPoints.append(Vec2(0.8f, 0.2f));
        m_spawnPoints.append(Vec2(0.2f, -0.7f));
        m_spawnPoints.append(Vec2(-0.2f, -0.7f));
    }

    // Difficulty increaser;
    m_enemiesToDestroy += (m_worldIndex * 10);
    m_enemiesPerStream += m_worldIndex;

    // Save a copy of the current level so
    // we know where each point started.
    m_spawnPointOrigins = QList<Vec2>();
    for (Vec2 &levelPoint : m_spawnPoints) {
        m_spawnPointOrigins.append(Vec2(levelPoint.x, levelPoint.y));
    }
}

void ldSpiralFighterVisualizer::onPlayerFire(ldSpiralFighterPlayer *player) {
    m_soundEffects.play(SFX::FIRE);

    ldSpiralFighterMissile missile = ldSpiralFighterMissile(Vec2(cosf(player->rotation), sinf(player->rotation)).normalize());
    missile.position = Vec2(player->getMissileSpawnPosition().x - missile.size.x * 0.5f, player->getMissileSpawnPosition().y - missile.size.y * 0.5f);
    m_missiles.append(missile);
}

void ldSpiralFighterVisualizer::onPlayerPowerup() {
    m_soundEffects.play(SFX::POWERUP);

    for (int i = 0; i < m_enemies.length(); i++) {
        ldSpiralFighterEnemy &enemy = m_enemies[i];

        addFireworks(enemy.position);

        m_bonusTimer = 0.5f;

        addScore(100);

        m_enemies.erase(m_enemies.begin() + i);
        i--;

        m_enemiesToDestroy--;
    }
}

// Draw all game graphics.
void ldSpiralFighterVisualizer::draw() {
    QMutexLocker lock(&m_mutex);

    ldAbstractGameVisualizer::draw();

    // Draw score label (only after game is over).
    if (m_isGameOver) {
        m_scoreLabel->innerDraw(m_renderer);
    }

    // Process game timer.
    if(m_isPlaying || (!m_isPlaying && !m_isReset) || m_isGameOver || m_isPaused) {
        m_stateLabel->innerDraw(m_renderer);
    }

    // Execute game functions.
    updateGame(m_renderer->getLastFrameDeltaSeconds());
    drawGame(m_renderer);
}

// Update game elements.
void ldSpiralFighterVisualizer::updateGame(float deltaTime) {
    if (m_isGameOver || m_countdownTimer.isActive() || m_isPaused) return;

    /*
     * Timers.
     */

    // Update timers.
    m_gameTimer += deltaTime;
    m_bonusTimer -= deltaTime;

    /*
     * Pre-update calls.
     */

    float levelAngularSpeed = LEVEL_ANGULAR_SPEED * (1 + (LEVEL_ANGULAR_SPEED_MULTIPLIER_LEVEL * m_levelIndex)) * (1 + (LEVEL_ANGULAR_SPEED_MULTIPLIER_WORLD * m_worldIndex));

    // Rotate enemies.
    for (ldSpiralFighterEnemy &enemy : m_enemies) {
        float distanceToCenter = enemy.position.magnitude();

        float currentAngle = enemy.position.toRadians();
        float newAngle = currentAngle + levelAngularSpeed * m_levelRotationDirection * deltaTime;

        enemy.position = Vec2(cosf(newAngle) * distanceToCenter, sinf(newAngle) * distanceToCenter);
    }

    /*
     * Update calls.
     */

    // Update player.
    m_player.update(deltaTime);

    // Update missiles.
    for (ldSpiralFighterMissile &missile : m_missiles) {
        missile.update(deltaTime);
    }

    // Update enemies.
    for (ldSpiralFighterEnemy &enemy : m_enemies) {
        enemy.update(deltaTime);
    }

    /*
     * Clear expired assets.
     */

    // Clear expired missiles.
    for (int i = 0; i < m_missiles.length(); i++) {
        if (m_missiles[i].expired) {
            m_missiles.erase(m_missiles.begin() + i);
            i--;
        }
    }

    /*
     * Game logic.
     */

    // Spawn enemies.
    m_streamSpawnTimer -= deltaTime;
    if (m_streamSpawnTimer <= 0.0f) {
        m_streamSpawnTimer = TIME_TO_SPAWN_STREAM;

        m_streamTimer = INTERVAL_BETWEEN_ENEMIES;
        m_remainingStreamEnemies = m_enemiesPerStream;

        m_currentStreamSpawnPointIndex = rand() % m_spawnPoints.length();
    }

    if (m_remainingStreamEnemies > 0) {
        m_streamTimer -= deltaTime;
        if (m_streamTimer <= 0.0f) {
            m_streamTimer = INTERVAL_BETWEEN_ENEMIES;
            m_remainingStreamEnemies--;

            ldSpiralFighterEnemy enemy = ldSpiralFighterEnemy(m_spawnPoints[m_currentStreamSpawnPointIndex]);
            m_enemies.append(enemy);
        }
    }

    // Rotate spawn points.
    for (int i = 0; i < m_spawnPoints.length(); i++) {
        float distanceToCenter = m_spawnPoints[i].magnitude();

        float currentAngle = m_spawnPoints[i].toRadians();
        float newAngle = currentAngle + levelAngularSpeed * m_levelRotationDirection * deltaTime;

        m_spawnPoints[i] = Vec2(cosf(newAngle) * distanceToCenter, sinf(newAngle) * distanceToCenter);
    }

    // Shrink the game.
    m_shrinkTimer -= deltaTime;
    if (m_shrinkTimer <= 0.0f) {
        m_shrinkTimer = TIME_TO_SHRINK;

        int randomLevelIndex = rand() % m_spawnPoints.length();

        Vec2 randomLevelPoint = m_spawnPoints[randomLevelIndex];
        Vec2 normalized = randomLevelPoint.normalize();

        float newDistance = fmax(randomLevelPoint.magnitude() - SHRINK_AMOUNT, 0.0f);

        randomLevelPoint = Vec2(normalized.x * newDistance, normalized.y * newDistance);
        m_spawnPoints[randomLevelIndex] = randomLevelPoint;
    }

    // Rotate enemies.
    for (ldSpiralFighterEnemy &enemy : m_enemies) {
        if (enemy.position.magnitude() <= DISTANCE_TO_ROTATE && !enemy.hasRotated()) {
            enemy.rotate();
        }
    }

    /*
     * Collisions.
     */

    // Check for collisions between player and enemies.
    for (int i = 0; i < m_enemies.length(); i++) {
        ldSpiralFighterEnemy &enemy = m_enemies[i];
        if (m_player.overlaps(enemy)) {
            addFireworks(m_player.position);

            m_soundEffects.play(SFX::EXPLOSION);

            m_player.visible = false;

            endGame(false);
        }
    }

    // Check for collisions between player missiles and enemies.
    for (int i = 0; i < m_missiles.length(); i++) {
        ldSpiralFighterMissile &missile = m_missiles[i];
        for (int j = 0; j < m_enemies.length(); j++) {
            ldSpiralFighterEnemy &enemy = m_enemies[j];
            if (missile.overlaps(enemy)) {
                addFireworks(enemy.position);

                m_soundEffects.play(SFX::EXPLOSION);

                m_bonusTimer = 0.5f;

                addScore(100);

                // Erase missile and restart checking. This is
                // necessary to avoid null pointer errors when
                // a missile collide with multiple targets.
                m_missiles.erase(m_missiles.begin() + i);
                i--;

                m_enemies.erase(m_enemies.begin() + j);
                j = m_enemies.length();

                m_enemiesToDestroy--;
            }
        }
    }

    // Check for collisions between player missiles and spawn points.
    for (int i = 0; i < m_missiles.length(); i++) {
        ldSpiralFighterMissile &missile = m_missiles[i];
        for (int j = 0; j < m_spawnPoints.length(); j++) {
            Vec2 &point = m_spawnPoints[j];
            Vec2 origin = m_spawnPointOrigins[j];

            if (missile.overlaps(point)) {
                if (origin.magnitude() > point.magnitude() && (point - origin).magnitude() > 0.01f) {
                    // If the point is not in its origin, then un-shrink it.
                    addExplosion(point);

                    m_soundEffects.play(SFX::EXPLOSION);

                    m_bonusTimer = 0.5f;

                    addScore(25);

                    m_missiles.erase(m_missiles.begin() + i);
                    i--;

                    Vec2 normalized = point.normalize();
                    float newDistance = fmin(point.magnitude() + SHRINK_AMOUNT, origin.magnitude());
                    Vec2 newPosition = Vec2(normalized.x * newDistance, normalized.y * newDistance);

                    m_spawnPoints[j] = newPosition;
                }
            }
        }
    }

    // Check for new level.
    if (m_enemiesToDestroy <= 0) {
        m_soundEffects.play(SFX::NEWLEVEL);

        loadNewLevel();
    }
}

// Draw game elements.
void ldSpiralFighterVisualizer::drawGame(ldRendererOpenlase *p_renderer) {
    /*
     * Draw calls.
     */

    // Draw player.
    m_player.draw(p_renderer);

    // Draw missiles.
    for (ldSpiralFighterMissile &missile : m_missiles) {
        missile.draw(p_renderer);
    }

    // Draw enemies.
    for (ldSpiralFighterEnemy &enemy : m_enemies) {
        enemy.draw(p_renderer);
    }

    // Draw level.
    float baseColorStep = m_bonusTimer > 0 ? m_gameTimer * 3.0f : m_gameTimer / 2.0f;
    float colorStep = 0.1f;

    p_renderer->begin(OL_LINESTRIP);
    QList<Vec2> v = m_spawnPoints;
    v.append(m_spawnPoints[0]); // Close the level
    QList<int> c = QList<int>();
    c.append(ldColorUtil::colorForStep(baseColorStep));
    c.append(ldColorUtil::colorForStep(baseColorStep + colorStep));
    ldGameObject::drawVertexRainbow(m_renderer, v, c, 3);
    p_renderer->end();

    // Draw powerup.
    if (!m_player.hasUsedPowerup()) {
        int color = 0xffffff;
        p_renderer->begin(OL_LINESTRIP);
        p_renderer->vertex(-0.95f, -0.95f, color, 3);
        p_renderer->vertex(-0.95f, -0.85f, color, 5);
        p_renderer->vertex(-0.90f, -0.85f, color, 5);
        p_renderer->vertex(-0.90f, -0.90f, color, 5);
        p_renderer->vertex(-0.95f, -0.90f, color, 3);
        p_renderer->end();
    }
}

/*
 * Specific game functions.
 */

void ldSpiralFighterVisualizer::addScore(int value) {
    m_score += value;

    emit scoreChanged(0);
}

// Ends the game and sets the correct state text.
void ldSpiralFighterVisualizer::endGame(bool won) {
    m_isGameOver = true;
    if (won) {
        setStateText("You win!");
    } else {
        setStateText("Game over!");
    }

    emit finished();
}

/*
 * Input functions.
 */


void ldSpiralFighterVisualizer::moveX(double value)
{
    if (m_isGameOver || m_countdownTimer.isActive()) return;

    m_player.rotate(value * -1.0);
}

void ldSpiralFighterVisualizer::onPressedLeft(bool pressed) {
    if (m_isGameOver || m_countdownTimer.isActive()) return;

    m_player.rotate(pressed ? 1.0 : 0);
}

void ldSpiralFighterVisualizer::onPressedRight(bool pressed) {
    if (m_isGameOver || m_countdownTimer.isActive()) return;

    m_player.rotate(pressed ? -1.0 : 0);
}

void ldSpiralFighterVisualizer::onPressedShoot(bool pressed) {
    if (m_isGameOver || m_countdownTimer.isActive()) return;

    m_player.onPressedShoot(pressed);
}

void ldSpiralFighterVisualizer::onPressedPowerup(bool pressed) {
    if (m_isGameOver || m_countdownTimer.isActive()) return;

    m_player.onPressedPowerup(pressed);
}

/*
 * Timer functions.
 */

// Starts the countdown timer.
void ldSpiralFighterVisualizer::startCountdownTimer() {
    m_countdownTimerValue = GAME_DEFAULT_RESET_TIME;

    QMetaObject::invokeMethod(&m_countdownTimer, "start", Qt::QueuedConnection);

    updateStateLabel();
}

// Logic for initial game countdown.
void ldSpiralFighterVisualizer::onTimerTimeout() {
    if (m_isPlaying) {
        m_countdownTimerValue--;

        updateStateLabel();

        if(m_countdownTimerValue == 0) {
            // If reached this point, it's time to start a new game.
            m_countdownTimer.stop();
        }
    }
}

/*
 * Label functions.
 */

// Updates the countdown label.
void ldSpiralFighterVisualizer::updateStateLabel() {
    if (!m_isGameOver) {
        QString timerString;
        if(m_countdownTimerValue > 0) {
            timerString = QString::number(m_countdownTimerValue);
        }
        setStateText(timerString);
    }
}

// Set the state label's text and horizontally align it.
void ldSpiralFighterVisualizer::setStateText(const QString &text) {
    m_stateLabel->setText(text);

    float labelWidth = m_stateLabel->getWidth();
    m_stateLabel->setPosition(Vec2(0.5f - (labelWidth/2.0f), 0.6f));
}

// Updates the score label.
void ldSpiralFighterVisualizer::updateScoreLabel() {
    QString scoreString = QString("%1").arg(QString::number(m_score));
    m_scoreLabel->setText(scoreString);

    float labelWidth = m_scoreLabel->getWidth();
    float labelHeight = m_scoreLabel->getHeight();
    m_scoreLabel->setPosition(Vec2(0.5f - (labelWidth/2.0f), 0.0f + (labelHeight / 2.0f)));
}

/*
 * Other functions.
 */

bool ldSpiralFighterVisualizer::init() {
    if (ldVisualizer::init()) {
        setPosition(ccp(1, 1));
        return true;
    }
    return false;
}

void ldSpiralFighterVisualizer::onShouldStart() {
    QMutexLocker lock(&m_mutex);
    m_renderer->setRenderParamsQuality();

    if(!m_isPaused) {
        resetMatch();
    }
}

void ldSpiralFighterVisualizer::onShouldStop()
{
    QMutexLocker lock(&m_mutex);
    if(m_isPlaying)
        togglePlay();
}

void ldSpiralFighterVisualizer::reset() {
    QMutexLocker lock(&m_mutex);

    resetMatch();
}

void ldSpiralFighterVisualizer::togglePlay() {
    QMutexLocker lock(&m_mutex);

    if(m_isGameOver) m_isPlaying = false;

    m_isPlaying = !m_isPlaying;

    if(m_isPlaying) {
        if(m_isPaused) {
            startCountdownTimer();
        } else {
            resetMatch();
        }

        m_isPaused = false;
    } else {
        m_isPaused = true;
        m_countdownTimer.stop();
        setStateText("PAUSE");
    }
}
