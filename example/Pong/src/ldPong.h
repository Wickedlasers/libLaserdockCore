#ifndef LDPONG_H
#define LDPONG_H

#include <ldCore/Games/ldAbstractGame.h>

class ldPongVisualizer;

/**
 * @brief The ldPong class
 * qsTr("Player - arrow keys");
 */
class ldPong : public ldAbstractGame
{
    Q_OBJECT

public:
    explicit ldPong(QObject *parent = 0);
    virtual ~ldPong();

private:
    virtual bool handleKeyEvent(QKeyEvent *keyEvent) override;
    virtual ldAbstractGameVisualizer *getGameVisualizer() const override;

    QScopedPointer<ldPongVisualizer> m_visualizer;
};

#endif // LDPONG_H
