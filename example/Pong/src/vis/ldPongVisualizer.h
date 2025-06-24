//
//  ldPongVisualizer.h
//  LaserdockVisualizer
//
//  Created by Sergey Gavrushkin on 12/09/16.
//  Copyright (c) 2016 Wicked Lasers. All rights reserved.
//

#ifndef LaserdockVisualizer__ldPongVisualizer__
#define LaserdockVisualizer__ldPongVisualizer__

#include <QtCore/QTimer>

#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldCore/Helpers/Sound/ldQSound.h"

#include "ldCore/Visualizations/Visualizers/Games/ldAbstractGameVisualizer.h"

class ldTextLabel;

class ldPongVisualizer : public ldAbstractGameVisualizer
{
    Q_OBJECT
public:
    explicit ldPongVisualizer();
    virtual ~ldPongVisualizer();

    // ldVisualizer
    
    virtual QString visualizerName() const override { return "Pong"; }

public slots:
    void move1Up(bool keyPress);
    void move1Down(bool keyPress);
    void move2Up(bool keyPress);
    void move2Down(bool keyPress);
    void moveBoard(int player, float step);

    // ldAbstractGameVisualizer
    void moveY(double y) override;

signals:
    void scoreChanged(const std::pair <int, int> &score);

protected:
    virtual void draw() override;

private slots:
    void onTimerTimeout();

private:
    enum SFX{
        BAT_A,
        BAT_B,
        WALL,
        LOSE,
        WIN
    };

    // ldAbstractGameVisualizer
    virtual void onGameReset() override final;
    virtual void onGamePlay() override final;
    virtual void onGamePause() override final;

    // get angle closer to X-axis so pong computer can catch it
    void adjustBallSpeedToXAxis();
    // change y direction depending on board position
    void adjustSpeed(float board_bottom);

    void centerTimerLabel();

    void drawBoard(int color, bool isFirstBoard);
    void drawWall(float p_color_stepper, bool isTopWall);
    void drawLostCircle(int color, bool isFirstBoard);

    void drawPointBeam(float x, float y, uint32_t c = C_WHITE, int hold = 16, float radius = 0.01) ;

    void resetGame();
    void resetMatch();

    void startTimer();

    void updateScoreLabel();
    void updateTimerLabel();

    int m_safeDrawing = 0;

    ldVec2 m_ballPos;
    ldVec2 m_ballPosLastLost;
    ldVec2 m_ballSpeedVector;

    float m_firstBoardBottomY;
    float m_secondBoardBottomY;
    float m_firstBoardEffectTimer;
    float m_secondBoardEffectTimer;
    float m_firstBoardLostTimer;
    float m_secondBoardLostTimer;

    // player2
    bool m_keyPressedUpBoard1 = false;
    bool m_keyPressedDownBoard1 = false;

    // player 1
    bool m_keyPressedUp = false;
    bool m_keyPressedDown = false;
    float m_axis = 0.f;


    std::pair<int, int> m_score;
    QScopedPointer<ldTextLabel> m_scoreLabel;

    QTimer m_timer;
    QScopedPointer<ldTextLabel> m_stateLabel;
};

#endif /*__LaserdockVisualizer__ldPongVisualizer__*/
