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
//  ldAbstractGameVisualizer.h
//  ldCore
//
//  Created by Sergey Gavrushkin 26/05/2017
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#ifndef __ldCore__ldAbstractGameVisualizer__
#define __ldCore__ldAbstractGameVisualizer__

#include <memory>

#include "ldCore/Visualizations/ldVisualizer.h"
#include "ldCore/Visualizations/util/SoundHelper/ldSoundEffects.h"
#include "ldCore/Visualizations/Visualizers/Games/Core/ldGameExplosion.h"
#include "ldCore/Visualizations/Visualizers/Games/Core/ldGameFirework.h"
#include "ldCore/Visualizations/Visualizers/Games/Core/ldGameSmoke.h"
#include "ldCore/Visualizations/Visualizers/Games/Core/ldGameSparkle.h"

class ldTextLabel;

class LDCORESHARED_EXPORT ldAbstractGameVisualizer : public ldVisualizer
{
    Q_OBJECT
public:
    explicit ldAbstractGameVisualizer();
    virtual ~ldAbstractGameVisualizer();

public slots:
    virtual void reset() = 0;
    virtual void togglePlay() = 0;

    virtual void setSoundEnabled(bool enabled);
    virtual void setSoundLevel(int soundLevel);

    // optional
    virtual void setComplexity(float /*speed*/) {}
    virtual void setLevelIndex(int /*index*/) {}

signals:
    void finished();

protected:
    static const int GAME_DEFAULT_RESET_TIME;
    // State machine.
    enum class State {
        Reset,
        Playing,
        GameOver
    };

    // Variables used in all games.
    State m_state = State::Reset;
    bool m_isPaused = false;
    bool m_isReset = false;
    float m_complexity = 1.0f;
    float m_gameTimer = 0.0f;

    virtual void draw(void) override;
    void addExplosion(Vec2 position, int color = 0xff7700, float size = 0.2f);
    void addFireworks(Vec2 position, int amount = 5);
    void addSparkle(Vec2 position);
    void addSmoke (Vec2 position);
    void drawVertexRainbow(ldRendererOpenlase* p_renderer, QList<Vec2> vertices, QList<int> colors, int segmentsPerLine = 4, int repeat = 1);

    // Labels.
    void showMessage(string text, float duration = 0.0f);
    void clearMessage();

    int m_readyToStartGameTimerValue = 0;

    ldSoundEffects m_soundEffects;

    QMutex m_mutex;

private:

    // Labels.
    QScopedPointer<ldTextLabel> m_messageLabel;
    float m_messageLabelTimer = 0.0f;

    // Effects.
    QList<ldGameObject> m_gameObjects;
    QList<ldGameExplosion> m_explosions;
    QList<ldGameFirework> m_fireworks;
    QList<ldGameSparkle> m_sparkles;
    QList<ldGameSmoke> m_smokes;
};

#endif /*__ldCore__ldAbstractGameVisualizer__*/
