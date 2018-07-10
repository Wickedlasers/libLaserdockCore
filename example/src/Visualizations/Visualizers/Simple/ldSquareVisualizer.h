//
//  ldSquareVisualizer.h
//  ldCore
//
//  Created by Sergey Gavrushkin on 25/10/17.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#ifndef __ldCore__ldSquareVisualizer__
#define __ldCore__ldSquareVisualizer__

#include "ldCore/Visualizations/ldVisualizer.h"

class ldSquareVisualizer : public ldVisualizer
{
    Q_OBJECT
public:
    explicit ldSquareVisualizer();
    virtual ~ldSquareVisualizer();

    bool init();

    // ldVisualizer
    virtual const char* getInternalName() override { return __FILE__; }
    virtual const char* visualizerName() override { return "Square"; }

protected:
    // ldVisualizer
    virtual void draw(void) override;
};

#endif /*__ldCore__ldSquareVisualizer__*/
