/**
    libLaserdockCore
    Copyright(c) 2018 Wicked Lasers

    This file is part of libLaserdockCore.

    libLaserdockCore is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libLaserdockCore is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libLaserdockCore.  If not, see <https://www.gnu.org/licenses/>.
**/

#ifndef LDSPECTRUMBANDVISUALIZER_H
#define LDSPECTRUMBANDVISUALIZER_H

#include "ldCore/Visualizations/ldVisualizer.h"

class ldSpectrumBandVisualizer : public ldVisualizer
{
public:
    ldSpectrumBandVisualizer();
    ldSpectrumBandVisualizer(int p_countCols, int p_countRows, float p_cellPadCoef, bool p_useLog, bool p_useMax, bool p_useBackground);
    ~ldSpectrumBandVisualizer();

    virtual QString visualizerName() const override { return "Appak. Spect Band v2"; }
    virtual float targetFPS() const override { return 60; }
    bool init();
    virtual void onShouldStart() override;

protected:

    virtual void clearBuffer() override;
    virtual void onUpdate(ldSoundData* pSoundData, float delta) override;
    virtual void draw() override;

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
    std::vector<std::vector<int> > grid;
    std::vector<float> top;
    std::vector<float> accel;

    std::vector<int> intervals;
};

#endif // LDSPECTRUMBANDVISUALIZER_H

