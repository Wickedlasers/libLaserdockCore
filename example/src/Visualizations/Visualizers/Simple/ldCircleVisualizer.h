#ifndef __ldCore__ldCircleVisualizer__
#define __ldCore__ldCircleVisualizer__

#include "ldCore/Visualizations/ldVisualizer.h"

class ldCircleVisualizer : public ldVisualizer
{
    Q_OBJECT
public:
    static void circle(ldRendererOpenlase *m_renderer, int points, float x, float y, float r, float c, float angle, int overlap, int windup, float dir = 1, bool gradient = false);

    explicit ldCircleVisualizer();
    virtual ~ldCircleVisualizer();

    bool init();

    // ldVisualizer
    virtual const char* getInternalName() override { return __FILE__; }
    virtual const char* visualizerName() override { return "Circle"; }

protected:
    // ldVisualizer
    virtual void draw(void) override;
};

#endif /*__ldCore__ldCircleVisualizer__*/
