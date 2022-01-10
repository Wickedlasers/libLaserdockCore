//
// Laser AngryLasers
// By Vander R. N. Dias a.k.a. "imerso" @ imersiva.com
// Copyright(c) 2019 Wicked Lasers
//

#ifndef __LaserdockVisualizer__ldAngryLasersVisualizer__
#define __LaserdockVisualizer__ldAngryLasersVisualizer__

#include <QtCore/QTimer>

#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldCore/Helpers/Color/ldColorUtil.h"
#include "ldCore/Helpers/Sound/ldQSound.h"
#include "ldCore/Visualizations/Visualizers/Games/ldAbstractGameVisualizer.h"


#include "impl/globals.h"
#include "impl/ldAngryLasersPlayer.h"
#include "impl/ldAngryLasersBox.h"
#include "impl/ldAngryLasersEnemy.h"
#include "impl/spline.h"

class ldTextLabel;

class ldAngryLasersVisualizer : public ldAbstractGameVisualizer, b2ContactListener
{
    Q_OBJECT

public:

    explicit ldAngryLasersVisualizer();
    virtual ~ldAngryLasersVisualizer() override;

    // ldVisualizer
    virtual QString visualizerName() const override { return "Angry Lasers"; }

    virtual int targetFPS() const override { return 20; }

    void initSplines();

public slots:
    // ldAbstractGameVisualizer
    virtual void moveX(double x) override;
    virtual void moveY(double y) override;

    // Input functions
    void onPressedLeft(bool pressed);
    void onPressedRight(bool pressed);
    void onPressedUp(bool pressed);
    void onPressedDown(bool pressed);
    void onPressedShoot(bool pressed);

signals:

    void scoreChanged(const int &score);

protected:

    virtual void draw() override;
    virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

private:

    enum SFX {
        EXPLOSION,
        POWERUP,
        NEWLEVEL,
        WOOD_IMPACT,
    };

    // ldAbstractGameVisualizer
    virtual void onGameReset() override final;
    virtual void onGamePause() override final;
    virtual void onGamePlay() override final;

    void updateGame(float deltaTime);

    void resetMatch();
    void addScore(int value);
    void endGame(bool won);

    // Labels.
    void updateScoreLabel();
    void setStateText(const QString &text);

    void addBox(float px, float py, float sx, float sy, ldAngryLasersBox::eAB_BoxType type);
    void addEnemy(float px, float py, ldAngryLasersEnemy::eAB_EnemyType type);

    /*
     * Specific game functions.
     */

    void parseLevel(std::vector<std::string> level);
    void loadNewLevel();

    /*
     * Event callbacks.
     */

    void onPlayerStopped(ldAngryLasersPlayer *player);

    // Label.
    QScopedPointer<ldTextLabel> m_scoreLabel;
    QScopedPointer<ldTextLabel> m_stateLabel;

    /*
     * Specific game variables
     */

    std::list<std::unique_ptr<ldAngryLasersBox>> boxes;
    std::list<std::unique_ptr<ldAngryLasersEnemy>> enemies;

    const int max_balls = 3;
    int m_balls;

    int m_score = 0;
    int m_level = 1;
    float m_levelWinTimer = 0;
    float m_levelLoseTimer = 0;
    ldAngryLasersPlayer m_player;

    float ground_line_y;
    std::shared_ptr<b2World> b2_world;
    b2Body* b2_ground_body;
    b2BodyDef b2_ground_def;
    b2PolygonShape b2_ground;
    std::unique_ptr<ldGradientObject> m_ground;
};

#endif /*__LaserdockVisualizer__ldAngryLasersVisualizer__*/
