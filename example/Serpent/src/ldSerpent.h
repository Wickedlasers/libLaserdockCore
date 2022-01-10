#ifndef LDSERPENT_H
#define LDSERPENT_H

#include <ldCore/Games/ldAbstractGame.h>

class ldSerpentVisualizer;

/**
 * @brief The ldSnake class
 * qsTr("< : Left\n> : Right\nv  : Pull Down\n^  : Rotate");
 */
class ldSerpent : public ldAbstractGame
{
    Q_OBJECT

public:
    explicit ldSerpent(QObject *parent = 0);
    virtual ~ldSerpent();

private:
    virtual bool handleKeyEvent(QKeyEvent *keyEvent) override;
    virtual ldAbstractGameVisualizer *getGameVisualizer() const override;

    QScopedPointer<ldSerpentVisualizer> m_visualizer;
};

#endif // LDSERPENT_H
