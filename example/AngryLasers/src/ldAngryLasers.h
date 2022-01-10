//
// Laser Angry Lasers
// By Vander R. N. Dias a.k.a. "imerso" @ imersiva.com
// Copyright(c) 2019 Wicked Lasers
//

#ifndef ldAngryLasers_H
#define ldAngryLasers_H

#include <ldCore/Games/ldAbstractGame.h>

class ldAngryLasersVisualizer;

class ldAngryLasers : public ldAbstractGame
{
    Q_OBJECT

public:
    explicit ldAngryLasers(QObject *parent = 0);
    virtual ~ldAngryLasers();

private:
    virtual bool handleKeyEvent(QKeyEvent *keyEvent) override;
    virtual ldAbstractGameVisualizer *getGameVisualizer() const override;

    QScopedPointer<ldAngryLasersVisualizer> m_visualizer;
};

#endif // ldAngryLasers_H
