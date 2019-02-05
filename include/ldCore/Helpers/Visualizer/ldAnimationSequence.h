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

#ifndef _ldAnimationSequence_h
#define _ldAnimationSequence_h

#include <stdlib.h>
#include <vector>

#include <ldCore/Helpers/Color/ldColorUtil.h>
#include "ldCore/Visualizations/ldVisualizer.h"
#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldCore/Helpers/Visualizer/ldLaserFilter.h"

class LDCORESHARED_EXPORT ldAnimationSequenceBezier
{
public:
    enum RenderAlg {
        Bezier = 3, // for compatibility with old values
        Lines = 4,
        Lights = 5
    };

    ldAnimationSequenceBezier();

    const svgBezierCurvesSequence &frames() const;
    void setFrames(const svgBezierCurvesSequence &frames);

    void setRenderAlg(RenderAlg renderAlg);

    void drawFrame(ldRendererOpenlase* r, int index);

//load svg sequence
    void loadDir(const QString &dirPath, const QString &filePrefix = "", int maskSize = -1, bool isExternal = false);

//.ldva1 format
    void load(const QString &filePath);
    void save(const QString &filePath);

//.ldva2 format
    bool load2(const QString &filePath);
    bool save2(const QString &filePath);

//.ldva3 format
    void load3(const QString &filePath);
    void save3(const QString &filePath);

// util funcs
    void autoscale();
    void removeblank();
    void scale(float xs, float ys);
    void move(float xt, float yt);

    std::vector<int> keyStart; // 8
    std::vector<int> keyEnd;
    float sourceFPS = 60;
    float sourceBPM = 80;

    uint32_t color = C_WHITE;

    float _fstart = 0;
    float _fend = 1;
// 2 colors and shaders for foreground/background in laser draw
    uint32_t _c1 = C_WHITE;
    uint32_t _c2 = C_WHITE;
    ldShader* _shader1 = nullptr;
    ldShader* _shader2 = nullptr;

private:
    void drawFrameLights5(ldRendererOpenlase* r, int index);
    void drawFrameLine4(ldRendererOpenlase* r, int index);
    void drawFrameBezier3(ldRendererOpenlase* r, int index);

// laser draw effect
    void drawFrameBezier3x(ldRendererOpenlase* r, int index, float fstart, float fend, uint32_t c1, uint32_t c2);


    RenderAlg m_renderAlg = RenderAlg::Lines;
    svgBezierCurvesSequence m_frames;
};



#endif //#ifndef _ldAnimationSequence_h

