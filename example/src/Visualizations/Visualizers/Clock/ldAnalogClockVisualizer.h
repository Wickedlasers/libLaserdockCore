#ifndef __ldCore__ldAnalogClockVisualizer__
#define __ldCore__ldAnalogClockVisualizer__

#include "ldCore/Visualizations/ldVisualizer.h"
#include "ldCore/Visualizations/util/MathsHelper/ldMaths.h"

class ldClockComplexObject;

class ldAnalogClockVisualizer : public ldVisualizer
{
    Q_OBJECT
public:
    explicit ldAnalogClockVisualizer(QObject *parent = nullptr);
    virtual ~ldAnalogClockVisualizer();

    bool init();

    // ldVisualizer
    virtual const char* getInternalName() override { return __FILE__; }
    virtual const char* visualizerName() override { return "Analog"; }
    virtual float targetFPS() override { return 42; }
    virtual void onShouldStart() override;

protected:
    virtual void draw(void) override;

private:
    QScopedPointer<ldClockComplexObject> m_clockObj;
};

#endif /*__ldCore__ldAnalogClockVisualizer__*/
