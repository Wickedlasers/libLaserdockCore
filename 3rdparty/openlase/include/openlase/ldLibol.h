/*
		OpenLase - a realtime laser graphics toolkit

Wicked Lasers object version of libol

Copyright (C) 2009-2011 Hector Martin "marcan" <hector@marcansoft.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 or version 3.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef LDLIBOL_H
#define LDLIBOL_H

#include <stdint.h>

#ifdef _WIN32
#define OPENLASE_EXPORT __declspec(dllexport)
#else
#define OPENLASE_EXPORT
#endif

#define C_RED   0xff0000
#define C_GREEN 0x00ff00
#define C_BLUE  0x0000ff
#define C_WHITE 0xffffff
#define C_BLACK 0x000000

#define C_GREY(x)   (0x010101 * CLAMP((int)(x), 0, 255))
#define C_RED_I(x)   (0x010000 * CLAMP((int)(x), 0, 255))
#define C_GREEN_I(x)   (0x000100 * CLAMP((int)(x), 0, 255))
#define C_BLUE_I(x)   (0x000001 * CLAMP((int)(x), 0, 255))

#define CLAMP(a,b,c) (((a)<(b))?(b):((a)>(c)?(c):(a)))

typedef struct OLPoint {
    float x,y,z;
    uint32_t color;
} OLPoint;

typedef struct OLRenderParams {
    int rate;
    float on_speed;
    float off_speed;
    int start_wait;
    int start_dwell;
    int curve_dwell;
    int corner_dwell;
    int end_dwell;
    int end_wait;
    float curve_angle;
    float flatness;
    float snap;
    int render_flags;
    int min_length;
    int max_framelen;
    float z_near;
} OLRenderParams;

enum RenderStyle {
    OL_LINESTRIP=0,
    OL_BEZIERSTRIP,
    OL_POINTS
};

enum RenderFlags {
    RENDER_GRAYSCALE = 1,
    RENDER_NOREORDER = 2,
    RENDER_NOREVERSE = 4,
    RENDER_CULLDARK = 8,
};

class OPENLASE_EXPORT ldLibol
{
public:
    ldLibol(int buffer_count = 0, int max_points = 30000);
    ~ldLibol();

    struct OLFrameInfo {
        int objects;
        int points;
        int resampled_points;
        int resampled_blacks;
        int padding_points;
    };

    struct OLRenderedFrame {
        int pmax;
        int pnext;
        OLPoint *points;
    };

    typedef void (*LogCallbackFunc)(const char *msg);
    typedef void (*ShaderFunc)(float *x, float *y, uint32_t *color);
    typedef void (*Shader3Func)(float *x, float *y, float *z, uint32_t *color);

    void olSetRenderParams(OLRenderParams *params);
    void olGetRenderParams(OLRenderParams *params) const;
    void olLoadIdentity(void);
    void olPushMatrix(void);
    void olPopMatrix(void);
    void olMultMatrix(float m[9]);
    void olRotate(float theta);
    void olTranslate(float x, float y);
    void olScale(float sx, float sy);

    void olLoadIdentity3(void);
    void olPushMatrix3(void);
    void olPopMatrix3(void);

    void olMultMatrix3(float m[16]);
    void olRotate3X(float theta);
    void olRotate3Y(float theta);
    void olRotate3Z(float theta);
    void olTranslate3(float x, float y, float z);
    void olScale3(float sx, float sy, float sz);

    void olFrustum (float left, float right, float bot, float ttop, float near, float far);
    void olPerspective(float fovy, float aspect, float zNear, float zFar);

    void olResetColor(void);
    void olMultColor(uint32_t color);
    void olPushColor(void);
    void olPopColor(void);

    void olBegin(int prim);
    void olVertex(float x, float y, uint32_t color);
    void olVertex3(float x, float y, float z, uint32_t color, int repeat);
    void olVertex2Z(float x, float y, float z, uint32_t color);
    void olEnd(void);

    void olTransformVertex(float *x, float *y) const;
    void olTransformVertex3(float *x, float *y, float *z) const;
    void olTransformVertex4(float *x, float *y, float *z, float *w) const;

    void olSetVertexPreShader(ShaderFunc f);
    void olSetVertexShader(ShaderFunc f);
    void olSetVertex3Shader(Shader3Func f);

    void olSetPixelShader(ShaderFunc f);
    void olSetPixel3Shader(Shader3Func f);

    void olRect(float x1, float y1, float x2, float y2, uint32_t color);
    void olLine(float x1, float y1, float x2, float y2, uint32_t color);
    void olDot(float x, float y, int points, uint32_t color);

    float olRenderFrame(int max_fps);

    void olGetFrameInfo(OLFrameInfo *info) const;

    void olSetScissor (float x0, float y0, float x1, float y1);

    void olLog(const char *fmt, ...) const;


    void olSetLogCallback(LogCallbackFunc f);

    OLRenderedFrame* olGetRenderedFrames() const;

private:

    static const int MTX_STACK_DEPTH = 16;

    struct Object {
        int pointcnt;
        OLPoint *points;
        float bbox[2][2];
    };

    struct Frame {
        int objcnt;
        int objmax;
        Object *objects;
        int psmax;
        int psnext;
        OLPoint *points;
    };

    struct DrawState {
        Object *curobj;
        OLPoint last_point;
        OLPoint last_slope;
        OLPoint c1, c2;
        int prim;
        int state;
        int points;
    };

    OLRenderedFrame *frames{nullptr};
    OLFrameInfo last_info{};
    Frame wframe{};
    DrawState dstate{};
    OLRenderParams params{};
    OLPoint last_render_point{};
    int maxpoints_warn_once{1};
    volatile int cwbuf{0};
    int fbufs{0};
    int buflag{0};
    int out_point{0};
    int first_time_full{0};
    int first_output_frame{0};
    float bbox[2][2]{};
    int mtx2dp{0};
    float mtx2ds[MTX_STACK_DEPTH][3][3]{};
    float mtx2d[3][3]{};
    int mtx3dp{0};
    float mtx3ds[MTX_STACK_DEPTH][4][4]{};
    float mtx3d[4][4]{};
    int coldp{0};
    uint32_t cols[MTX_STACK_DEPTH]{};
    uint32_t curcol{0};

    ShaderFunc vpreshader{nullptr};
    ShaderFunc vshader{nullptr};
    Shader3Func v3shader{nullptr};
    ShaderFunc pshader{nullptr};
    Shader3Func p3shader{nullptr};

    LogCallbackFunc log_cb{nullptr};

    uint32_t colmul(uint32_t a, uint32_t b) const;
    OLPoint *ps_alloc(int count);
    void addpoint(const float &x, const float &y, const float &z, const uint32_t &color);
    int get_dwell(const float &x, const float &y) const;
    void line_to(const float &x, const float &y, const float &z, const uint32_t &color);
    void recurse_bezier(const float &x1, const float &y1, const float &x2, const float &y2, const float &x3, const float &y3, const uint32_t &color, const int &depth);
    void bezier_to(const float &x, const float &y, const uint32_t &color);
    void point_to(const float &x, const float &y, const float &z, const uint32_t &color);
    int chkpts(const int &count) const;
    void addrndpoint(const float &x, const float &y, const uint32_t &color);
    int render_object(Object *obj);

};

#endif
