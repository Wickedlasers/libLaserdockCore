//
//  ldArrowVisualizer.h
//  LaserdockVisualizer
//
//  Created by MEO 05/04/2017 - with code from Pong by Sergey Gavrushkin on 12/09/16.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#ifndef __LaserdockVisualizer__ldArrowVisualizer__
#define __LaserdockVisualizer__ldArrowVisualizer__

#include <QtCore/QTimer>

#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldCore/Helpers/Sound/ldQSound.h"

#include <ldLuaGame/ldLuaGameVisualizer.h>

class ldArrowVisualizer : public ldLuaGameVisualizer
{
    Q_OBJECT
public:
    explicit ldArrowVisualizer();
    virtual ~ldArrowVisualizer();

    // ldVisualizer
    virtual QString visualizerName() const override { return "Arrow"; }

public slots:
    void moveY(double y) override;
    void moveX(double x) override;

    void keyLeft(bool keyPress);
    void keyRight(bool keyPress);

protected:
    virtual void draw(void) override;

private:
    // ldAbstractGameVisualizer
    virtual void onGameReset() override final;
    virtual void onGamePause() override final;
    virtual void onGamePlay() override final;
};

#endif /*__LaserdockVisualizer__ldArrowVisualizer__*/
