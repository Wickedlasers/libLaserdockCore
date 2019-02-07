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

#ifndef ldSpiralFighter_H
#define ldSpiralFighter_H

#include <ldCore/Games/ldAbstractGame.h>

class ldSpiralFighterVisualizer;

class ldSpiralFighterGame : public ldAbstractGame
{
    Q_OBJECT

public:
    explicit ldSpiralFighterGame(QObject *parent = 0);
    virtual ~ldSpiralFighterGame();

private:
    virtual bool handleKeyEvent(QKeyEvent *keyEvent) override;
    virtual ldAbstractGameVisualizer *getGameVisualizer() const override;

    QScopedPointer<ldSpiralFighterVisualizer> m_visualizer;
};

#endif // ldSpiralFighter_H
