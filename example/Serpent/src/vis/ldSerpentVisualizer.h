//
//  ldSerpentVisualizer.h
//  LaserdockVisualizer
//
//  Created by Darren Otgaar 16/08/2018
//  Copyright (c) 2018 Wicked Lasers. All rights reserved.
//

#ifndef LaserdockVisualizer__ldSerpentVisualizer__
#define LaserdockVisualizer__ldSerpentVisualizer__

#include "ldCore/Helpers/Sound/ldQSound.h"
#include "ldCore/Visualizations/Visualizers/Games/ldAbstractGameVisualizer.h"

class ldSerpentGame;

class ldSerpentVisualizer : public ldAbstractGameVisualizer
{
    Q_OBJECT
public:
    explicit ldSerpentVisualizer();
    virtual ~ldSerpentVisualizer();

    // ldVisualizer
    virtual QString visualizerName() const override { return "Snake B"; }

public slots:
    // ldAbstractGameVisualizer
    virtual void moveX(double x) override;
    virtual void moveY(double y) override;

    void moveLeft(bool keyPress);
    void moveRight(bool keyPress);
    void moveUp(bool keyPress);
    void moveDown(bool keyPress);

signals:

protected:
    void draw() override;

private slots:

private:
    // ldAbstractGameVisualizer
    virtual void onGameReset() override final;
    virtual void onGamePlay() override final;
    virtual void onGamePause() override final;

    std::unique_ptr<ldSerpentGame> m_game;
};

#endif /*__LaserdockVisualizer__ldSerpentVisualizer__*/
