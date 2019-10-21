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



#endif // LDFILTERCOLORIZE_H
