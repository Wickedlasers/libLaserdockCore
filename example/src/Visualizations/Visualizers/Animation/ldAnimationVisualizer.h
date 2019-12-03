//
//  ldAnimationVisualizer.h
//  LaserdockVisualizer
//
//  Created by feldspar on 7/14/14.
//  Copyright (c) 2014 Wicked Lasers. All rights reserved.
//

#ifndef LaserdockVisualizer__ldAnimationVisualizer__
#define LaserdockVisualizer__ldAnimationVisualizer__

#include "ldCore/Visualizations/ldVisualizer.h"
#include "ldCore/Helpers/Visualizer/ldAnimationSequence.h"

class ldVisDirectionCtrl;

class ldAnimationVisualizer : public ldVisualizer
{
public:
    ldAnimationVisualizer(const QString &filePath);
    ~ldAnimationVisualizer();

public:
    void setDirectionCtrl(ldVisDirectionCtrl *directionCtrl);

    virtual QString visualizerName() const override;
    virtual int targetFPS() const override { return 42; }

    virtual void onShouldStart() override;
protected:
    virtual void onUpdate(ldSoundData* pSoundData, float delta) override;
    virtual void draw() override;

    void loadAnimation();

protected:
    int currentFrame() const;

    // internal
    ldAnimationSequenceBezier m_asb;
    bool m_isLoaded = false;

private:
    QString m_filePath;

    ldVisDirectionCtrl *m_directionCtrl = nullptr;
    bool m_isDirectionBack = false;

    float m_totalFrameDelta = 0;
    int m_currentFrame = 0;
};

#endif /* defined(__LaserdockVisualizer__ldAnimationVisualizer__) */

