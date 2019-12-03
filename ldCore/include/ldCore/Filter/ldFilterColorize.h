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

#ifndef LDFILTERCOLORIZE_H
#define LDFILTERCOLORIZE_H

#include "ldFilter.h"

class LDCORESHARED_EXPORT ldFilterColorFade: public ldFilter { // rising fade effect
    Q_OBJECT
    LD_WRITABLE_MIN_MAX_PROPERTY(float, colors)

public:
    ldFilterColorFade(bool isProcessMusic = false);

    bool m_isProcessMusic = false;
    float offset = 0.f;
    float huebase = 0.f;
    float huerange = 0.5f;
    float freq = 0.5f;
    bool y = false;

    virtual void process(Vertex &input) override;
    virtual QString name() override { return QObject::tr("Color Fade"); }
    virtual bool isMusicAware() const override { return true; }

};


class LDCORESHARED_EXPORT ldFilterColorLift : public ldFilter {  // rising bands effect adapted from appak svg visualizer code
public:
    ldFilterColorLift();
//    float cde = 0.f;
//    float cde2;
//    float cde3;
//    float nde;
    virtual void process(Vertex &input) override;
    virtual QString name() override { return QObject::tr("Color Lift"); }
    virtual bool isMusicAware() const override { return true; }
};

class LDCORESHARED_EXPORT ldFilterColorDrop: public ldFilter { // color spreads from center outwards
public:
	//FilterCircleWipe();
	virtual void process(Vertex &input) override;
	virtual QString name() override { return QObject::tr("Color Drop"); }
    virtual bool isMusicAware() const override { return true; }
};


class LDCORESHARED_EXPORT FilterColorBlobs: public ldFilter { // color spreads from center outwards as blobs
public:
    FilterColorBlobs(int type = 0) {m_type = type;} int m_type;
    virtual void process(Vertex &input) override;
    virtual QString name() override {
        if (m_type == 0) return QObject::tr("Color Blobs A");
        else if (m_type == 1) return QObject::tr("Color Blobs B");
        else return QObject::tr("Color Blobs C");
    }
    virtual bool isMusicAware() const override { return (m_type == 2); }
};

class LDCORESHARED_EXPORT FilterColorFreq: public ldFilter { // colors from frequency spectrum graph
public:
    FilterColorFreq(int type = 0) {m_type = type;} int m_type;
    virtual void process(Vertex &input) override;
    virtual QString name() override {
        if (m_type == 0) return QObject::tr("Color Freq A");
        else if (m_type == 1) return QObject::tr("Color Freq B");
        else return QObject::tr("Color Freq C");
    }
    virtual bool isMusicAware() const override { return true; }
};

// ---------- ldShimmerFilter ----------

class LDCORESHARED_EXPORT ldShimmerFilter : public ldFilter
{
    Q_OBJECT
    LD_WRITABLE_MIN_MAX_PROPERTY(float, colors)
public:
    ldShimmerFilter();
    virtual void process(Vertex &input) override;
    virtual QString name() override { return QObject::tr("Shimmer A"); }
    virtual bool isMusicAware() const override { return true; }

private:
    float m_counter = 0;
};

// additional colorize filters

// utility classes
class FramePulse {
public:
    float value = 0;
    float freq = 1;
    bool tick = true;
    bool first = true;
    bool gate = true;
    float gatemax = 4.0f;
    FramePulse(float _freq = 1, float _value = 0);
    void start(float _freq = 1, float _value = 0);
    void update(float delta);
};

class ColorPanelCanvas {
public:
    static const int maxsize = 16;
    int xx = 8;
    int yy = 8;
    int steps = 5;
    bool instant = true;
    bool followsteps = true;
    float thresh = 0;
    bool round = false;
    float array[maxsize][maxsize];
    int nextstep = 1;
    ColorPanelCanvas();
    void init();
    bool isint(float f);
    void getRandXYModIntCross(int& xm, int& ym);
    void getRandXYModIntRound(int& xm, int& ym);
    void grow(float delta);
    void spread(float delta);
    void spread();
    void drop();
    float getf(float x, float y);
    float getfSmooth(float x, float y);
};

class GridNoise {
public:
    static const int msize = 16;
    float a[msize][msize];
    int size = msize;
    GridNoise();
    void init(int s);
    void clear(float z = 0);
    void noise(float f = 1);
    void stripe(float z = 1, bool h = true, bool v = true);
    void plasma(float d = 2.5f, float f = 0.2f);
    float get(float x, float y, float zoom, float rot = 0);
    float get(float x, float y);
};

class ColorMap {
public:
    virtual ~ColorMap() = default;

    virtual void getRGB(float f, float& r, float& g, float& b) = 0;
    void getRGBClamp(float f, float& r, float& g, float& b);
    void getRGBCycle(float f, float& r, float& g, float& b);
    void getRGBMirror(float f, float& r, float& g, float& b);
};
class ColorMapHSVGradient : public ColorMap {
public:
    float h1 = 0, s1 = 0, v1 = 1;
    float h2 = 0, s2 = 0, v2 = 1;
    virtual void getRGB(float f, float& r, float& g, float& b);
};
class ColorMapBPSW : public ColorMap {
public:
    float v = 1;
    int hue6 = 0;
    virtual void getRGB(float f, float& r, float& g, float& b);
};

// filters
class LDCORESHARED_EXPORT FilterColorScroll : public ldFilter { // scrolling stripes color patterns
public:
    FilterColorScroll(int type = 0);
    virtual void process(Vertex &input) override;
    virtual QString name() override { if (m_type == 2) return QObject::tr("Colorize Scroll C");
                                    if (m_type == 1) return QObject::tr("Colorize Scroll B");
                                                    return QObject::tr("Colorize Scroll A");}
    virtual bool isMusicAware() const override { return (m_type != 0); }
private:
//    bool m_alt;
    int m_type;
    FramePulse pulse1;
    FramePulse pulse2;
    FramePulse pulse3;
};

class LDCORESHARED_EXPORT FilterColorPanels : public ldFilter { // color based on grid with flood fill style animation
public:
    FilterColorPanels(bool alt = false);
    virtual void process(Vertex &input) override;
    virtual QString name() override { return QObject::tr(!m_alt?"Colorize Panel":"Colorize Crumble"); }
    virtual bool isMusicAware() const override { return !m_alt; }
private:
    bool m_alt;
    ColorPanelCanvas panel;
    FramePulse pulse1;
    FramePulse pulse2;
};

class LDCORESHARED_EXPORT FilterColorGlass : public ldFilter { // color objects separately; color is chosen and remains until next blank points
public:
    FilterColorGlass(bool alt = false);
    virtual void process(Vertex &input) override;
    virtual QString name() override { return QObject::tr(!m_alt?"Colorize Glass":"Colorize Mosaic"); }
    virtual bool isMusicAware() const override { return true; }
private:
    bool m_alt;
    float ox = 0;
    float oy = 0;
    float oangle = 0;
    int phase = 0;
    float curvalue = 0;
    float curoffset = 0;
    FramePulse pulse1;
};

class LDCORESHARED_EXPORT FilterColorAura : public ldFilter { // color using noise maps, emphasis on outside areas
public:
    FilterColorAura(bool alt = false);
    virtual void process(Vertex &input) override;
    virtual QString name() override { return QObject::tr(!m_alt?"Colorize Aura A":"Colorize Aura B"); }
    virtual bool isMusicAware() const override { return m_alt; }
private:
    bool m_alt;
    FramePulse fc1;
    FramePulse fc2;
    FramePulse fc3;
    FramePulse fc4;
    FramePulse fc5;
    FramePulse fc6;
    GridNoise sbg1;
    GridNoise sbg2;
    GridNoise sbg3;
    GridNoise sbg4;
    int huecount = 0;
};

class LDCORESHARED_EXPORT FilterColorAcid : public ldFilter { // color using noise maps, hue cycle effect
public:
    FilterColorAcid(bool alt = false);
    virtual void process(Vertex &input) override;
    virtual QString name() override { return QObject::tr(!m_alt?"Colorize Acid A":"Colorize Acid B"); }
    virtual bool isMusicAware() const override { return m_alt; }
private:
    bool m_alt;
    FramePulse fc1;
    FramePulse fc2;
    FramePulse fc3;
    FramePulse fc4;
    FramePulse fc5;
    FramePulse fc6;
    GridNoise sbg1;
    GridNoise sbg2;
    GridNoise sbg3;
    GridNoise sbg4;
    int huecount = 0;
};

class LDCORESHARED_EXPORT FilterColorLava : public ldFilter { // color using noise maps, rising effect
public:
    FilterColorLava(bool alt = false);
    virtual void process(Vertex &input) override;
    virtual QString name() override { return QObject::tr(!m_alt?"Colorize Lava":"Colorize Slime"); }
    virtual bool isMusicAware() const override { return false; }
private:
    bool m_alt;
    FramePulse fc1;
    FramePulse fc2;
    FramePulse fc3;
    FramePulse fc4;
//    FramePulse fc5;
//    FramePulse fc6;
    GridNoise sbg1;
    GridNoise sbg2;
    GridNoise sbg3;
    GridNoise sbg4;
//    int huecount = 0;
};


class LDCORESHARED_EXPORT FilterColorVolt : public ldFilter { // color based on remapping HSV space
public:
    FilterColorVolt(bool alt = false);
    virtual void process(Vertex &input) override;
    virtual QString name() override { return QObject::tr(!m_alt?"Color Volt A":"Color Volt B"); }
    virtual bool isMusicAware() const override { return m_alt; }
private:
    bool m_alt;
};



#endif // LDFILTERCOLORIZE_H
