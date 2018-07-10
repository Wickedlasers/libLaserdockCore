#ifndef LDSPECTRUMBANDVISUALIZER_H
#define LDSPECTRUMBANDVISUALIZER_H

#include "ldCore/Visualizations/ldVisualizer.h"

class ldSpectrumBandVisualizer : public ldVisualizer
{
public:
    ldSpectrumBandVisualizer();
    ldSpectrumBandVisualizer(int p_countCols, int p_countRows, float p_cellPadCoef, bool p_useLog, bool p_useMax, bool p_useBackground);
    ~ldSpectrumBandVisualizer();

    virtual const char* getInternalName() override { return __FILE__; }
    virtual const char* visualizerName () override { return "Appak. Spect Band v2"; }
    virtual float targetFPS() override { return 60; }
    bool init();
    virtual void onShouldStart() override;

protected:

    virtual void clearBuffer() override;
    virtual void onUpdate(ldSoundData* pSoundData, float delta) override;
    virtual void draw(void) override;

    void initData();
    void line(CCPoint a, CCPoint b);
    
    int countCols;
    int countRows;
    int maxPoints;
	
    bool useLog = false;
    bool useMax = false;
    bool useBackground = false;
    
    float cellPadCoef;
    float cellWidth;
    float cellPadding;
    
    // like Rock Classic Viz but dynamic dim
    vector<vector<int> > grid;
    vector<float> top;
    vector<float> accel;

    vector<int> intervals;
};

#endif // LDSPECTRUMBANDVISUALIZER_H

