//
//  ldPongVisualizer.cpp
//  LaserdockVisualizer
//
//  Created by Sergey Gavrushkin on 12/09/16.
//  Copyright (c) 2016 Wicked Lasers. All rights reserved.
//

#include "ldPongVisualizer.h"

#include <QtCore/QTime>

#include "ldCore/ldCore.h"
#include "ldCore/Helpers/Color/ldColorUtil.h"

#include "ldCore/Helpers/Text/ldTextLabel.h"
#include <QtCore/qmath.h>

const float BOARD_LENGTH  = 0.5f; // 0..1
const float BOARD_STEP = 0.05f; // speed of board 0..1
const float BOUNDS = 0.99f;

// ldPongVisualizer
ldPongVisualizer::ldPongVisualizer() : ldAbstractGameVisualizer() {

    m_soundEffects.insert(SFX::BAT_A, ldCore::instance()->resourceDir() + "/sound/pongbata.wav");
    m_soundEffects.insert(SFX::BAT_B, ldCore::instance()->resourceDir() + "/sound/pongbatb.wav");
    m_soundEffects.insert(SFX::WALL, ldCore::instance()->resourceDir() + "/sound/pongwall.wav");
    m_soundEffects.insert(SFX::LOSE, ldCore::instance()->resourceDir() + "/sound/ponglose.wav");
    m_soundEffects.insert(SFX::WIN, ldCore::instance()->resourceDir() + "/sound/pongwin.wav");

    m_firstBoardEffectTimer = 0;
    m_secondBoardEffectTimer = 0;
    m_firstBoardLostTimer = 0;
    m_secondBoardLostTimer = 0;
    m_ballPosLastLost = ldVec2();

    m_scoreLabel.reset(new ldTextLabel());
    m_scoreLabel->setColor(0xFFFFFF);

    m_timer.setInterval(1000); // 1 sec
    connect(&m_timer, &QTimer::timeout, this, &ldPongVisualizer::onTimerTimeout);

    m_stateLabel.reset(new ldTextLabel());
    m_stateLabel->setColor(0xFFFFFF);

    setPosition(ccp(1, 1));

    qRegisterMetaType<std::pair<int, int> >();
    connect(this, &ldPongVisualizer::scoreChanged, this, &ldPongVisualizer::updateScoreLabel);

    resetMatch();
}

ldPongVisualizer::~ldPongVisualizer() {
}

void ldPongVisualizer::move1Up(bool keyPress) {
    QMutexLocker lock(&m_mutex);
    m_keyPressedUpBoard1 = keyPress;
}

void ldPongVisualizer::move1Down(bool keyPress) {
    QMutexLocker lock(&m_mutex);
    m_keyPressedDownBoard1 = keyPress;
}

void ldPongVisualizer::move2Up(bool keyPress) {
    QMutexLocker lock(&m_mutex);
    m_keyPressedUp = keyPress;
}

void ldPongVisualizer::move2Down(bool keyPress) {
    QMutexLocker lock(&m_mutex);
    m_keyPressedDown = keyPress;
}

void ldPongVisualizer::moveBoard(int player, float step) {
    const float MAX_Y = BOUNDS - BOARD_LENGTH;
    const float MIN_Y = -1.0f * BOUNDS;
    if(player == 0) {
        if(m_firstBoardBottomY < MIN_Y) {
            return;
        }
        m_firstBoardBottomY += step;
        m_firstBoardBottomY = std::min(std::max(m_firstBoardBottomY, MIN_Y), MAX_Y);
    } else {
        if(m_secondBoardBottomY < MIN_Y) {
            return;
        }
        m_secondBoardBottomY += step;
        m_secondBoardBottomY = std::min(std::max(m_secondBoardBottomY, MIN_Y), MAX_Y);
    }
}

void ldPongVisualizer::moveY(double y)
{
    QMutexLocker lock(&m_mutex);
    m_axis = y;
}

// draw
void ldPongVisualizer::draw() {
    const float MAX_Y = BOUNDS - BOARD_LENGTH;
    const float MIN_Y = -1.0f * BOUNDS;
    QMutexLocker lock(&m_mutex);

    // init at each draw
    m_safeDrawing = 0;

    //Board: MEO jumpiness fix
    if(!m_timer.isActive()) {
        if (m_keyPressedUp) { //MEO: when added A.I., swapped player keys
            moveBoard(0, BOARD_STEP);
        }
        if (m_keyPressedDown) {
            moveBoard(0, -1*BOARD_STEP);
        }
        if(m_axis < 0.05f || m_axis > 0.05f) {
            moveBoard(0, BOARD_STEP * m_axis * 1.5f);
        }
    }
    if (m_keyPressedUpBoard1) { //MEO: when added A.I., swapped player keys
        //boardUp(1); //ToDo -reimplement when AI/2nd player choice
    }
    if (m_keyPressedDownBoard1) {
        //boardDown(1);
    }

    //A.I.
    m_secondBoardBottomY += 0.1f * (((1.0f + m_ballPos.y) - ((1.0f + m_secondBoardBottomY) + (BOARD_LENGTH / 2.0f))) * qPow((1.0f + m_ballPos.x) / 2.0f, 2.5f));
    m_secondBoardBottomY = std::max(m_secondBoardBottomY, MIN_Y);
    m_secondBoardBottomY = std::min(m_secondBoardBottomY, MAX_Y);
    if (m_secondBoardBottomY != m_secondBoardBottomY) m_secondBoardBottomY = MIN_Y; //check for infinity and correct

    /*
     * DRAW
     */

    // reduce effect timer if positive
    float decay_timer = 0.08f;
    m_firstBoardEffectTimer = (m_firstBoardEffectTimer > 0)?(m_firstBoardEffectTimer-decay_timer):0;
    m_secondBoardEffectTimer = (m_secondBoardEffectTimer > 0)?(m_secondBoardEffectTimer-decay_timer):0;
    // lost timer
    decay_timer = 0.17f;
    m_firstBoardLostTimer = (m_firstBoardLostTimer > 0)?(m_firstBoardLostTimer-decay_timer):0;
    m_secondBoardLostTimer = (m_secondBoardLostTimer > 0)?(m_secondBoardLostTimer-decay_timer):0;

    // color
    QTime time = QTime::currentTime();
    float second = time.second() + 0.001*time.msec();
    int colorOne = ldColorUtil::colorForStep(-1 * second/60.0);
    int colorTwo = ldColorUtil::colorForStep(0.5 - 1 * second/60.0);
    if (m_firstBoardEffectTimer > 0) {
        colorOne = ldColorUtil::lerpInt(colorOne,0xFFFFFF,m_firstBoardEffectTimer);
    } else if (m_firstBoardLostTimer > 0) {
        colorOne = ldColorUtil::lerpInt(colorOne,0xFFFFFF,m_firstBoardLostTimer);
    }
    if (m_secondBoardEffectTimer > 0) {
        colorTwo = ldColorUtil::lerpInt(colorTwo,0xFFFFFF,m_secondBoardEffectTimer);
    } else if (m_secondBoardLostTimer > 0) {
        colorTwo = ldColorUtil::lerpInt(colorTwo,0xFFFFFF,m_secondBoardLostTimer);
    }

    // draw top line
    drawWall(-1 * second/60.0, true);

    // score
    m_scoreLabel->innerDraw(m_renderer);

    // timer
    if(state() != ldGameState::Reset) {
        m_stateLabel->innerDraw(m_renderer);
    }

    // board left: first one is left
    drawBoard(colorOne, true);
    if (m_firstBoardLostTimer > 0) drawLostCircle(colorOne, true);

    // ball
    drawPointBeam(m_ballPos.x, m_ballPos.y);

    // board right: draw right board from top to bottom
    drawBoard(colorTwo, false);
    if (m_secondBoardLostTimer > 0) drawLostCircle(colorTwo, false);

    // bottom line
    drawWall(-1 * second/60.0, false);

	if (m_gameOverCountDown > 0)
	{
		m_stateLabel->setText("GAME OVER");
		centerTimerLabel();

		m_gameOverCountDown--;
		if (m_gameOverCountDown == 0)
		{
			resetMatch();
		}

		return;
	}
    /*
     * game logic
     */

    // check for up/down walls
    const float bounds_check = BOUNDS - 0.02f;
    if((m_ballPos.y >= bounds_check && m_ballSpeedVector.y > 0.0f)
            || (m_ballPos.y <= -1.0f * bounds_check && m_ballSpeedVector.y < 0.0f)) {
        m_ballSpeedVector.y *= -1.0f;
        m_soundEffects.play(SFX::BAT_B);
    }

    bool wasReturn = false;
    int playerReturning = -1;

    // check if board was at right
    if(m_ballPos.x >= bounds_check && m_ballSpeedVector.x > 0.0f
            && (m_ballPos.y >= m_secondBoardBottomY && m_ballPos.y <= m_secondBoardBottomY + BOARD_LENGTH)) {
        adjustSpeed(m_secondBoardBottomY);
        wasReturn = true;
        playerReturning = 2;
    }
    // check if board was at left
    if(m_ballPos.x <= -1.0f * bounds_check && m_ballSpeedVector.x < 0
            && (m_ballPos.y >= m_firstBoardBottomY && m_ballPos.y <= m_firstBoardBottomY + BOARD_LENGTH)) {
        adjustSpeed(m_firstBoardBottomY);
        wasReturn = true;
        playerReturning = 1;
    }

    if (wasReturn) {
        if (playerReturning==1) {
            m_firstBoardEffectTimer = 1.0;
            m_soundEffects.play(SFX::WALL);
        } else {
            m_secondBoardEffectTimer = 1.0;
            m_soundEffects.play(SFX::BAT_A);
        }
    }

    // check for goal
    if(!wasReturn &&
            (m_ballPos.x >= 1.0f || m_ballPos.x <= -1.0f)) {
        if(m_ballPos.x >= 1.0f) {
            m_score.first += 1;
            m_secondBoardLostTimer = 1.0;
            m_soundEffects.play(SFX::WIN);
        }
        if(m_ballPos.x <= -1.0f) {
            m_score.second += 1;
            m_firstBoardLostTimer = 1.0;
            m_soundEffects.play(SFX::LOSE);
        }
        emit scoreChanged(m_score);

		if (m_score.first == 5 || m_score.second == 5)
		{
			m_gameOverCountDown = 200;
			QMetaObject::invokeMethod(&m_timer, "start", Qt::QueuedConnection);
		}
		else
		{
			resetGame();
		}

        return;
    }

    // next ball pos
    if(state() == ldGameState::Playing && !m_timer.isActive()) {
        m_ballPos.x += m_ballSpeedVector.x;
        m_ballPos.y += m_ballSpeedVector.y;
    }
}

// drawWall
void ldPongVisualizer::drawWall(float p_color_stepper, bool isTopWall) {
    ldVec2 p = ldVec2();
    int pointsCount = 20;
    float sign = 1.0;;
    if (!isTopWall) sign = -1.0;
    m_renderer->begin(OL_LINESTRIP);
    for (int j = 0; j < pointsCount+1; j++) {
        float step = 1.0*j/pointsCount;
        if (!isTopWall) step = 1.0 - step;
        p.x = (2*step-1)*BOUNDS;
        p.y = isTopWall ? BOUNDS : -1.f*BOUNDS;
        int color = ldColorUtil::colorForStep(p_color_stepper + sign*step * 0.5);
        m_renderer->vertex(p.x, p.y, color);
    }
    m_renderer->end();
}

// drawBoard
void ldPongVisualizer::drawBoard(int color, bool isFirstBoard) {
    m_renderer->begin(OL_LINESTRIP);
    int pointsCount = 10;
    ldVec2 p = ldVec2();
    for (int j = 0; j < pointsCount+1; j++) {
        // advance angle to next point
        float step = BOARD_LENGTH*j/pointsCount;
        p.x = isFirstBoard ? -1.f*BOUNDS : BOUNDS;
        p.y = isFirstBoard ? step + m_firstBoardBottomY : step + m_secondBoardBottomY;
        m_renderer->vertex(p.x, p.y, color);
    }
    m_renderer->end();
}

// drawLostCircle
void ldPongVisualizer::drawLostCircle(int color, bool isFirstBoard) {
    if (m_firstBoardLostTimer == 0 && m_secondBoardLostTimer == 0) return;

    int pointsCount = 10;
    ldVec2 p = ldVec2();
    float scale = 1.0 - m_firstBoardLostTimer;
    float sign = 1.0;
    if (!isFirstBoard) {
        scale = 1.0 - m_secondBoardLostTimer;
        sign = -1.0;
    }
    m_renderer->begin(OL_LINESTRIP);
    for (int j = 0; j < pointsCount+1; j++) {
        // advance angle to next point
        float step = 1.0 - 1.0*j/pointsCount;
        float ray = BOARD_LENGTH*scale;
        p.x = m_ballPosLastLost.x;
        p.y = m_ballPosLastLost.y;
        p.x += sign*ray*cos(step*M_PI - M_PI_2);
        p.y += ray*sin(step*M_PI - M_PI_2);
        // add point to buffer
        // debug
        int colorFlash = ldColorUtil::lerpInt(0xFFFFFF,color,scale);
        colorFlash = ldColorUtil::lerpInt(colorFlash,0x000000,1.0-scale);
        if (ldMaths::isValidLaserPoint(p)) m_renderer->vertex(p.x, p.y, colorFlash);
    }
    m_renderer->end();
}

void ldPongVisualizer::onTimerTimeout() {
    m_startGameTimerValue--;

    updateTimerLabel();

    if(m_startGameTimerValue == 0) {
        m_timer.stop();
    }
}

void ldPongVisualizer::onGameReset()
{
    resetMatch();
}

void ldPongVisualizer::onGamePlay()
{
    startTimer();
}

void ldPongVisualizer::onGamePause()
{
    m_timer.stop();
    m_stateLabel->setText("PAUSE");

    centerTimerLabel();
}

// get angle closer to X-axis so pong computer can catch it
void ldPongVisualizer::adjustBallSpeedToXAxis()
{
    float angle = atan2(m_ballSpeedVector.y, m_ballSpeedVector.x);
    if(angle > 0 && angle < M_PI/2.f) {
        angle = angle/2.f;
    } else if(angle > M_PI/2.f && angle < M_PI) {
        angle = (angle + M_PI)/2.f;
    } else if(angle < 0 && angle > M_PI/-2.f) {
        angle = angle/2.f;
    } else if(angle < M_PI/-2.f && angle > M_PI*-1.f) {
        angle = (angle + M_PI*-1.f)/2.f;
    }
    float ballSpeed = sqrt(m_ballSpeedVector.x * m_ballSpeedVector.x + m_ballSpeedVector.y * m_ballSpeedVector.y);
    float signX = m_ballSpeedVector.x > 0.0f ? 1.0f : -1.0f;
    m_ballSpeedVector.x = fabsf(ballSpeed * cosf(angle)) * signX;
    m_ballSpeedVector.y = ballSpeed * sinf(angle) * -1.0f;
}

// change y direction depending on board position
void ldPongVisualizer::adjustSpeed(float boardBottom) {
    const float MAX_BOUNCE_ANGLE = (float)M_PI / 16.0f;

    float relativeIntersectY = (boardBottom + (BOARD_LENGTH / 2.0f)) - m_ballPos.y;
    float bounceAngle = (relativeIntersectY / (BOARD_LENGTH / 2.0f)) * (M_PI / 2.0 - MAX_BOUNCE_ANGLE);
    float ballSpeed = sqrt(m_ballSpeedVector.x * m_ballSpeedVector.x + m_ballSpeedVector.y * m_ballSpeedVector.y);
    float signX = m_ballSpeedVector.x > 0.0f ? -1.0f : 1.0f;
    m_ballSpeedVector.x = fabsf(ballSpeed * cosf(bounceAngle)) * signX;
    m_ballSpeedVector.y = ballSpeed * sinf(bounceAngle) * -1.0f;
}

void ldPongVisualizer::centerTimerLabel() {
    float w1 = m_stateLabel->getWidth();
    m_stateLabel->setPosition(ldVec2(0.5f - (w1/2.0f), 0.6f));
}

// TODO replace with ldVisualizerBeam::drawPointBeam
void ldPongVisualizer::drawPointBeam(float x, float y, uint32_t c, int hold, float radius) {
    m_renderer->begin(OL_POINTS);
    for (int i = 0; i < hold; i++) {
        float jx = (rand()%100)/100.0f*2-1;
        float jy = (rand()%100)/100.0f*2-1;
        m_renderer->vertex(x+jx*radius, y+jy*radius, c);
    }
    m_renderer->end();
}

void ldPongVisualizer::resetGame() {
    m_ballPosLastLost = m_ballPos;
    m_ballPos = ldVec2();

    adjustBallSpeedToXAxis();

    m_firstBoardBottomY = BOARD_LENGTH * -0.5;
    m_secondBoardBottomY = m_firstBoardBottomY;

    startTimer();
}

void ldPongVisualizer::resetMatch() {
    resetGame();
    m_ballSpeedVector = {0.02f, 0.03f};

    m_score = std::make_pair<int, int>(0, 0);
    emit scoreChanged(m_score);
}

void ldPongVisualizer::startTimer() {
    m_startGameTimerValue = GAME_DEFAULT_RESET_TIME;
    QMetaObject::invokeMethod(&m_timer, "start", Qt::QueuedConnection);

    updateTimerLabel();
}


void ldPongVisualizer::updateScoreLabel() {
    QString scoreString = QString::number(m_score.first) + " : " + QString::number(m_score.second);
    m_scoreLabel->setText(scoreString);

    float w1 = m_scoreLabel->getWidth();
    m_scoreLabel->setPosition(ldVec2(0.5f - (w1/2.0f), 0.9f));
}

void ldPongVisualizer::updateTimerLabel() {
    QString timerString;

    if(m_startGameTimerValue > 0) {
        timerString = QString::number(m_startGameTimerValue);
    }
    m_stateLabel->setText(timerString);

    centerTimerLabel();
}

