#include "ldSpectrumBandVisualizer.h"

#include <QtCore/QDebug>

#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldCore/Helpers/Maths/ldMathSpectrum.h"

#include "ldCore/Helpers/Color/ldColorUtil.h"

ldSpectrumBandVisualizer::ldSpectrumBandVisualizer()
{
    countCols = 4;
    countRows = 6;
    cellPadCoef = 0.2f;
    init();
    initData();
}

//ldSpectrumBandVisualizer::ldSpectrumBandVisualizer(int p_countCols, int p_countRows, float p_cellPadCoef, bool p_useLog, bool p_useMax, bool p_useBackground)
ldSpectrumBandVisualizer::ldSpectrumBandVisualizer(int p_countCols, int p_countRows, float p_cellPadCoef, bool p_useLog, bool p_useMax, bool p_useBackground)
{
    countCols = p_countCols;
    countRows = p_countRows;
    cellPadCoef = p_cellPadCoef;
	useLog = p_useLog;
	useMax = p_useMax;
	useBackground = p_useBackground;
    init();
    initData();
}

ldSpectrumBandVisualizer::~ldSpectrumBandVisualizer() {
    grid.clear();
    top.clear();
    accel.clear();
    for (auto it : grid) it.clear();
    grid.clear();
}

bool ldSpectrumBandVisualizer::init()
{
    if (ldVisualizer::init())
    {
        setPosition(ccp(1, 1));
        setContentSize(CCSizeMake(2, 2));
        setAnchorPoint(ccp(0.5, 0.5));
        //clearBuffer();
        return true;
    }
    return false;
}

void ldSpectrumBandVisualizer::onShouldStart() {

    if (useBackground) {
        OLRenderParams params;
        memset(&params, 0, sizeof params);
        params.rate = m_renderer->rate();
        params.on_speed = 1.0f / 30.0f;
        params.off_speed = 1.0f / 30.0f;
        params.start_wait = 2; // 16
        params.start_dwell = 3;
        params.curve_dwell = 0;
        params.corner_dwell = 0;
        params.curve_angle = cosf(30.0f*(M_PIf / 180.0f)); // 30 deg
        params.end_dwell = 3;
        params.end_wait = 2;
        params.snap = 1 / 1000.0f;
        params.render_flags = RENDER_NOREVERSE | RENDER_NOREORDER;
        params.min_length = 0;
        params.max_framelen = params.rate / 45;
        params.flatness = 0.00001f; //for beziers to work (thanks Alec!)
        m_renderer->setRenderParams(&params);
    } else {
        OLRenderParams params;
        memset(&params, 0, sizeof params);
        params.rate = m_renderer->rate();
        params.on_speed = 1.0f / 36.0f;
        params.off_speed = 1.0f / 36.0f;
        params.start_wait = 2; // 16
        params.start_dwell = 1;
        params.curve_dwell = 0;
        params.corner_dwell = 0;
        params.curve_angle = cosf(30.0f*(M_PIf / 180.0f)); // 30 deg
        params.end_dwell = 1;
        params.end_wait = 1;
        params.snap = 1 / 1000.0f;
        params.render_flags = RENDER_NOREVERSE | RENDER_NOREORDER;
        params.min_length = 0;
        params.max_framelen = params.rate / 45;
        params.flatness = 0.00001f; //for beziers to work (thanks Alec!)
        m_renderer->setRenderParams(&params);
    }
}

void ldSpectrumBandVisualizer::clearBuffer() {
}

void ldSpectrumBandVisualizer::initData() {
    maxPoints = 666;
    
    //
    cellWidth = 2.0/(countCols + (countCols-1)*cellPadCoef);
    cellPadding = cellPadCoef*cellWidth;
    
    grid.clear();
    top.clear();
    accel.clear();
    for (auto it : grid) it.clear();
    grid.clear();

    //
    for (int i = 0; i < countCols; i++)
    {
        top.push_back(0);
        accel.push_back(0);
        //
        std::vector<int> tmp;
        for (int j = 0; j < countRows; j++)
        {
            tmp.push_back(0);
        }
        grid.push_back(tmp);
    }

    float max_band_Sound_data = 150;

    intervals.clear();
    float std_interval_delta = 1.0/countCols;
    float tmp_plot = 0;
    for (int i=0; i< countCols; i++)
    {
        tmp_plot += std_interval_delta;
        int logTmp = (int) max_band_Sound_data*ldMaths::normLog(tmp_plot, 1.60f);
        intervals.push_back(logTmp);
        //qDebug() << "tmp_plot" << tmp_plot  << "    logTmp" << logTmp;
    }
}

void ldSpectrumBandVisualizer::onUpdate(ldSoundData* pSoundData, float /*delta*/) {
    if (!pSoundData) return;

    //vector<float> tmpValues = ldMathSpectrum::getAverageSpectrumValueFromInterval(pSoundData, intervals, 100);
    //vector<float> tmpValues = ldMathSpectrum::getNormalizedSpectrumValueFromInterval(pSoundData, intervals, 200);

    
    std::vector<float> tmpValues;
    for (int i = 0; i < countCols; i++) {
        
        float f1 = 440;
        float f2 = 440;
        if (!useLog) {
            // constant width
            float width = 440.0 * 8 / countCols;
            f1 = width * (i + 0);
            f2 = width * (i + 1);
        } else {
            // log width
            //float base = 2; // 2 = each band is an octave
            //base = 1.5; // looks better
            //float center = 880; // centered at this hz
            //float mult = center / (powf(base, (countCols / 2)));

            float base = 2;
            //float center = 880;
            float mult = 40;

            //float start = 40;
            //float end = 10000;
            
            f1 = mult * powf(base, (i + 0));
            f2 = mult * powf(base, (i + 1));
        }
        //qDebug() << f1 << " -- " << f2;
        float v;
        if (!useMax) {
            v = pSoundData->GetFFTInFreqRange(f1, f2, true, true, false);
            v = (v - 0.125) / (1 - 0.125);
            if (v < 0) v = 0;
            v = powf(v, 1.0f / 2.5f);
        } else {
            v = pSoundData->GetFFTInFreqRange(f1, f2, true, false, true);
            v = powf(v, 1.0f/1.414f);
            if (v > 0.167) v = (logf(v) / logf(6)) + 1;            
            if (v < 0) v = 0;
        }          
        tmpValues.push_back(v);

    }

    //qDebug() << "countCols" <<  countCols << "tmpValues" << tmpValues.size();
    /*
    for (int i = 0; i < countCols; i++)
    {
        float coef = ldMaths::normLog(1.0*i/(countCols-1), 1/1.70);
        int ind = (int) coef*(countCols-1);
        ranges.push_back(ind);
    }
    for (int i = 1; i < countCols; i++)
    {
        int ind = ranges.push_back(ind);
        float v = pSoundData ? pSoundData->powerAt(i, countCols) : 0;
        tmpValues.push_back(pSoundData ? pSoundData->powerAt(i, countCols) : 0);
    }
    */

    for (int i = 0; i < countCols; i++)
    {
        
        float power = tmpValues.at(i);// pSoundData ? pSoundData->powerAt(i, countCols) : 0;
        //qDebug() << "power" << power;
        int pidx = power * countRows;
        
        if (power * countRows < top[i] && top[i] >= 0)
        {
            top[i] -= accel[i];
            accel[i] += 0.05f;
        }
        else
        {
            top[i] = fminf(power * countRows, countRows - 0.1f);
            accel[i] = 0;
        }

        for (int j = 0; j < countRows; j++)
        {
            if (j <= pidx || j == (int)top[i]) grid[i][j] = 255;
            else if (grid[i][j] == 255) grid[i][j] = 120;
            else grid[i][j] *= 0.001f;
            if (grid[i][j] < 50) grid[i][j] = 0;
            if (top[i] < 0.1) grid[i][j] = 0;
        }
    }

    tmpValues.clear();
}

void ldSpectrumBandVisualizer::draw()
{
    float limit=0.99f;
    /*
    CCPoint tl,tr,bl,br;
    tl = ccp(-0,limit);
    tr = ccp(limit,limit);
    bl = ccp(-0,-limit);
    br = ccp(limit,-limit);
    line(tl,tr);
    line(bl,br);
    */
    int maxPointPerLine = maxPoints/countRows;
    maxPointPerLine = maxPointPerLine/countCols;
    if (maxPointPerLine<3) maxPointPerLine = 3;

    if (useBackground) maxPointPerLine = 4;
    //qDebug() << "maxPointPerLine" << maxPointPerLine;
    
    for (int j = 0; j < countRows; j++)
    {
        for (int i = 0; i < countCols; i++)
        {
            float x0,x,y;
            x0 = (cellWidth+cellPadding)*i - 1;
            //y = ldMaths::normLog(1.0*j/(countRows-1), 1/1.70);
            y = 1.0*j/(countRows-1);
            y = 2.0*y-1;
            y*=limit;
            
            m_renderer->begin(OL_LINESTRIP);
            
            for (int k = 0; k < maxPointPerLine; k++) {
                x = x0 + 1.0*cellWidth*k/(maxPointPerLine-1);
                if (j%2!=0) x = -x;
                
                int color = C_RED;
                if (i==2) color = 0x00FF00;
                int i_col = i;
                if (j%2!=0) i_col = countCols-1-i;
                
                //color = grid[i_col][j]==255 ? 0xFF0000 : ldColorUtil::colorRGB(grid[i_col][j] / 2, grid[i_col][j], 0);
                if (grid[i_col][j]>0 || j==0) {
                    color = ldColorUtil::lerpInt(0x00FF00, 0xFF0000, ldMaths::normLog(1.0*j/(countRows-1), 3.0));
                } else {
                    color = 0;
                    if (useBackground) color = 0x00000041;
                }

                if(k == 0) m_renderer->vertex(x, y, C_BLACK, 2);
                m_renderer->vertex(x, y, color);
                if(k == (maxPointPerLine - 1)) m_renderer->vertex(x, y, C_BLACK, 2);
            }
            m_renderer->end();
            
        }
    }
}

void ldSpectrumBandVisualizer::line(CCPoint a, CCPoint b) {
    m_renderer->begin(OL_LINESTRIP);
    m_renderer->line(a.x, a.y, b.x, b.y, C_WHITE);
    m_renderer->end();
}




