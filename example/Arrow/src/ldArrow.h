#ifndef LDARROW_H
#define LDARROW_H

#include <ldCore/Games/ldAbstractGame.h>

class ldArrowVisualizer;

/**
 * @brief The ldArrow class
 * qsTr("< : Left\n> : Right\nv  : Pull Down\n^  : Rotate");
 */
class ldArrow : public ldAbstractGame
{
    Q_OBJECT

public:
    static void initResources();

    explicit ldArrow(QObject *parent = 0);
    virtual ~ldArrow();

private:
    virtual bool handleKeyEvent(QKeyEvent *keyEvent) override;
    virtual ldAbstractGameVisualizer *getGameVisualizer() const override;

    QScopedPointer<ldArrowVisualizer> m_visualizer;
};

#endif // LDARROW_H
