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
//  ldSpiralFighterVisualizer.h
//  ldCore
//
//  Created by Glauco Pires 20/10/2017.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#ifndef __ldCore__ldSpiralFighterVisualizer__
#define __ldCore__ldSpiralFighterVisualizer__

#include <QtCore/QTimer>

#include "ldCore/Visualizations/util/MathsHelper/ldMaths.h"
#include "ldCore/Visualizations/util/ColorHelper/ldColorUtilMEO.h"
#include "ldCore/Visualizations/util/SoundHelper/ldQSound.h"
#include "ldCore/Visualizations/Visualizers/Games/ldAbstractGameVisualizer.h"

#include "SpiralFighter/ldSpiralFighterEnemy.h"
#include "SpiralFighter/ldSpiralFighterMissile.h"
#include "SpiralFighter/ldSpiralFighterPlayer.h"

class ldTextLabel;

class ldSpiralFighterVisualizer : public ldAbstractGameVisualizer
{
    Q_OBJECT
public:
    explicit ldSpiralFighterVisualizer();
    virtual ~ldSpiralFighterVisualizer();

    bool init();

    // ldVisualizer
    virtual const char* getInternalName() override { return __FILE__; }
    virtual const char* visualizerName() override { return "Spiral Fighter"; }
    virtual void onShouldStart() override;
    virtual void onShouldStop() override;

public slots:
    // ldAbstractGameVisualizer
    virtual void reset() override;
    virtual void togglePlay()  override;

    void moveX(double value) override;

    // Input functions
    void onPressedLeft(bool pressed);
    void onPressedRight(bool pressed);
    void onPressedShoot(bool pressed);
    void onPressedPowerup(bool pressed);

signals:
    void scoreChanged(const int &score);

protected:
    virtual void draw(void) override;
    OLRenderParams m_params;

private slots:
    void onTimerTimeout();

private:
    enum SFX {
        EXPLOSION,
        FIRE,
        NEWLEVEL,
        POWERUP
    };

    /*
     * Main game functions.
     */

    void updateGame(float deltaTime);
    void drawGame(ldRendererOpenlase* p_renderer);

    void resetMatch();
    void addScore(int value);
    void endGame(bool won);

    // Timers.
    void startCountdownTimer();

    // Labels.
    void updateScoreLabel();
    void updateStateLabel();
    void setStateText(string text);

    /*
     * Specific game functions.
     */

    void loadNewLevel();

    /*
     * Event callbacks.
     */

    void onPlayerFire(ldSpiralFighterPlayer *player);
    void onPlayerPowerup();

    /*
     * Main game fields.
     */

    bool m_isGameOver = false;
    bool m_hasWon = false;
    int m_score = 0;
    float m_gameTimer = 0.f;

    // Label.
    QScopedPointer<ldTextLabel> m_scoreLabel;
    QScopedPointer<ldTextLabel> m_stateLabel;

    // Timer.
    QTimer m_countdownTimer;
    int m_countdownTimerValue = 0;

    /*
     * Specific game variables
     */

    int m_levelIndex;
    int m_worldIndex;
    ldSpiralFighterPlayer m_player;
    float m_levelRotationDirection;
    int m_remainingStreamEnemies;
    int m_currentStreamSpawnPointIndex;

    int m_enemiesToDestroy;
    int m_enemiesPerStream;

    // Lists.
    QList<ldSpiralFighterEnemy> m_enemies;
    QList<ldSpiralFighterMissile> m_missiles;
    QList<Vec2> m_spawnPoints;
    QList<Vec2> m_spawnPointOrigins;

    // Timers.
    float m_bonusTimer;
    float m_shrinkTimer;
    float m_streamSpawnTimer;
    float m_streamTimer;
};

#endif /*__ldCore__ldSpiralFighterVisualizer__*/
