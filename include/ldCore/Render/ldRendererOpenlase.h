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


/*

ldRendererOpenlase provides access to the openlase render state and some additional utility functions.

Drawing a custom shape involves calling olBegin(...), adding points with olVertex(...), then olEnd().
Extended funcitons drawDots(...) and drawCircle(...) are also provided for convenience (these functions
include the begin and end).

Openlase render parameters may also be set before drawing with setRenderParams(...). Common presets are
accessible with setRenderParamsDefaultQuality(), etc..

Render params and frame modes apply to the current frame only and should be set in every draw (if used).

- Frame modes

Frame modes are an additional way to interact with the rendering system with setFrameModes(...). These are
used in order to have stronger control over the final image in special circumstances.

Note that the safety systems are necessarily restrictive and so can cause a frame to be displayed differently
than expected. Generally this is not an issue when using reasonable render params such as on/off speed and dwell
values. Also, some projectors have underscan or overscan protections built in. So if you disable these filters in
order to exceed their limits, the resulting signal may trigger the hardware safety, resulting in blanking or other
undesired behavior.

*/


#ifndef LDRENDEREROPENLASE_H
#define LDRENDEREROPENLASE_H

#include <QtCore/QObject>

#include <openlase/libol.h>

#include "ldCore/ldCore_global.h"
#include "ldCore/Render/ldAbstractRenderer.h"
#include "ldCore/Utilities/ldBasicDataStructures.h"


// ----------------- libol helpers -------------------

#define C_YELLOW 0xFFFF00
#define C_CYAN 0x00FFFF
#define C_MAGENTA 0xFF00FF
#define C_DARK_BLUE 0x000066
#define C_LIGHT_BLUE 0x4444FF


inline bool operator==(const OLPoint &e1, const OLPoint &e2)
{
    return e1.x == e2.x
            && e1.y == e2.y
            && e1.z == e2.z
            && e1.color == e2.color;
}

// ------------------------ ldRendererOpenlase -----------

class ldFrameBuffer;

/** Openlase renderer wrapper */
class LDCORESHARED_EXPORT ldRendererOpenlase : public ldAbstractRenderer
{
    Q_OBJECT
public:
    explicit ldRendererOpenlase(QObject *parent = 0);

    /////////////////////////////////////////////////////////////////////////
    //                         Libol Module Functions
    /////////////////////////////////////////////////////////////////////////

    void setRenderParams(OLRenderParams * params);
    void getRenderParams(OLRenderParams *params);
    void setFrameModes(int flags);
    float renderFrame(ldFrameBuffer * buffer, int max_fps);

    /////////////////////////////////////////////////////////////////////////
    //                         Libol Graphic Operations
    /////////////////////////////////////////////////////////////////////////

    void loadIdentity(void);
    void pushMatrix(void);
    void popMatrix(void);

    void multMatrix(float m[9]);
    void rotate(float theta);
    void translate(float x, float y);
    void scale(float sx, float sy);

    void loadIdentity3(void);
    void pushMatrix3(void);
    void popMatrix3(void);

    void multMatrix3(float m[16]);
    void rotate3X(float theta);
    void rotate3Y(float theta);
    void rotate3Z(float theta);
    void translate3(float x, float y, float z);
    void scale3(float sx, float sy, float sz);

    void frustum (float left, float right, float bot, float top, float near, float far);
    void perspective(float fovy, float aspect, float zNear, float zFar);

    void resetColor(void);
    void multColor(uint32_t color);
    void pushColor(void);
    void popColor(void);

    void begin(int prim);
    void vertex(float x, float y, uint32_t color, int repeat = 1);
    void vertex3(float x, float y, float z, uint32_t color, int repeat = 1);
    void end(void);

    void transformVertex3(float *x, float *y, float *z);

    void rect(float x1, float y1, float x2, float y2, uint32_t color);
    void line(float x1, float y1, float x2, float y2, uint32_t color);
    void dot(float x, float y, int points, uint32_t color);


    /////////////////////////////////////////////////////////////////////////
    //                         Extended Graphic Operations
    /////////////////////////////////////////////////////////////////////////

    // set render params using a default template
    // quality - precise laser movement
    // speed - uses fewer points for better fps
    // raw - no processing (raw mode)
    // standard - no processing (raw mode)
    // lower - ?
    // bezier - for bezier lines
    // beam - optimized for beam vis
    void setRenderParamsQuality();
    void setRenderParamsSpeed();
    void setRenderParamsRaw();
    void setRenderParamsStandard();
    void setRenderParamsLower();
    void setRenderParamsBezier();
    void setRenderParamsBeam();

    // drawPoints draws a point multiple times, useful for beam and particle visualizers.
    // nPoints - number of samples to spend
    // jitter - places points in a random spread of radius=jitter
    void drawPoints(float x, float y, uint32_t color = C_WHITE, int nPoints = 16, float jitter = 0.01);

    // drawCircle draws a circle using points
    // seamAngle - the angle (in radians) where the circle begins/ends, which results in a visible seam.
    //           - you can put this at the top (pi/2), or some angle that makes sense in the context of
    //           - your scene.  setting this to a random value will remove the seam but cause flicker.
    // nPointsMin and nPointsMax - the circle is approximated by a N-GON with this many sides.
    //                           - larger circles use values closer to max, s.t. (radius >= 1) --> max.
    // overlap - number of additional (black) vertexes to extend at the start and end of circle. this gives
    //         - galvos time to begin motion, reduces seam/gap. value of 1 or 2 is usually good. 0 is disable.
    void drawCircle(float x, float y, float radius, uint32_t color = C_WHITE, float seamAngle = 0, int nPointsMin = 8, int nPointsMax = 64, int overlap = 1);

    // frame timing utility
    int m_lastFramePointCount = 0;
    float getLastFrameDeltaSeconds();

    // frame modes
    int m_frameModes = 0;

    // cached frames for screenshot feature
    std::vector<Vertex> m_cachedFrame;
    const std::vector<Vertex> &getCachedFrame() const;

};

#endif // LDRENDEREROPENLASE_H
