//
//  ldSpiralFighterVisualizer.h
//  LaserdockVisualizer
//
//  Created by Glauco Pires 20/10/2017.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#ifndef LaserdockVisualizer__ldSpiralFighterVisualizer__
#define LaserdockVisualizer__ldSpiralFighterVisualizer__

#include <QtCore/QTimer>

#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldCore/Helpers/Color/ldColorUtil.h"
#include "ldCore/Helpers/Sound/ldQSound.h"
#include "ldCore/Visualizations/Visualizers/Games/ldAbstractGameVisualizer.h"

#include "impl/ldSpiralFighterEnemy.h"
#include "impl/ldSpiralFighterMissile.h"
#include "impl/ldSpiralFighterPlayer.h"

class ldTextLabel;

class ldSpiralFighterVisualizer : public ldAbstractGameVisualizer
{
    Q_OBJECT
public:
    explicit ldSpiralFighterVisualizer();
    virtual ~ldSpiralFighterVisualizer();

    // ldVisualizer
    virtual QString visualizerName() const override { return "Spiral Fighter"; }

public slots:
    // ldAbstractGameVisualizer
    void moveX(double value) override;

    // Input functions
    void onPressedLeft(bool pressed);
    void onPressedRight(bool pressed);
    void onPressedShoot(bool pressed);
    void onPressedPowerup(bool pressed);

signals:
    void scoreChanged(const int &score);

protected:
    virtual void draw() override;
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

    // ldAbstractGameVisualizer
    virtual void onGameReset() override final;
    virtual void onGamePlay() override final;
    virtual void onGamePause() override final;

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
    void setStateText(const QString &text);

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

    bool m_hasWon = false;
    int m_score = 0;
    float m_gameTimer2 = 0.f;

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
	int m_playerHitPoints = 5;
    float m_levelRotationDirection;
    int m_remainingStreamEnemies;
    int m_currentStreamSpawnPointIndex;

    int m_enemiesToDestroy;
	int m_enemiesToSpawn;
    int m_enemiesPerStream;

    // Lists.
    QList<ldSpiralFighterEnemy> m_enemies;
    QList<ldSpiralFighterMissile> m_missiles;
    QList<ldVec2> m_spawnPoints;
    QList<ldVec2> m_spawnPointOrigins;

    // Timers.
    float m_bonusTimer;
    float m_shrinkTimer;
    float m_streamSpawnTimer;
    float m_streamTimer;
};

#endif /*__LaserdockVisualizer__ldSpiralFighterVisualizer__*/
