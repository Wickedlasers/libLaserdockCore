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
