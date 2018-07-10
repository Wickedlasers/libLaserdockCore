/*
		OpenLase - a realtime laser graphics toolkit

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

#ifndef LIBOL_H
#define LIBOL_H

#include <stdint.h>

#ifdef _WIN32
#define OPENLASE_EXPORT __declspec(dllexport)
#else
#define OPENLASE_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum {
	OL_LINESTRIP,
	OL_BEZIERSTRIP,
	OL_POINTS
};

#define C_RED   0xff0000
#define C_GREEN 0x00ff00
#define C_BLUE  0x0000ff
#define C_WHITE 0xffffff
#define C_BLACK 0x000000

#define CLAMP(a,b,c) (((a)<(b))?(b):((a)>(c)?(c):(a)))

#define C_GREY(x)   (0x010101 * CLAMP((int)(x), 0, 255))
#define C_RED_I(x)   (0x010000 * CLAMP((int)(x), 0, 255))
#define C_GREEN_I(x)   (0x000100 * CLAMP((int)(x), 0, 255))
#define C_BLUE_I(x)   (0x000001 * CLAMP((int)(x), 0, 255))

enum {
	RENDER_GRAYSCALE = 1,
	RENDER_NOREORDER = 2,
	RENDER_NOREVERSE = 4,
	RENDER_CULLDARK = 8,
};

typedef struct {
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

typedef struct {
	int objects;
	int points;
	int resampled_points;
	int resampled_blacks;
	int padding_points;
} OLFrameInfo;

typedef struct OLPoint {
	float x,y,z;
	uint32_t color;
} OLPoint;

typedef struct OLRenderedFrame {
	int pmax;
	int pnext;
	OLPoint *points;
#ifdef HAS_JACK_SUPPORT
	float *audio_l;
	float *audio_r;
#endif
} OLRenderedFrame;

OPENLASE_EXPORT int olInit(int buffer_count, int max_points);

OPENLASE_EXPORT void olSetRenderParams(OLRenderParams *params);
OPENLASE_EXPORT void olGetRenderParams(OLRenderParams *params);

#ifdef HAS_JACK_SUPPORT
typedef void (*AudioCallbackFunc)(float *leftbuf, float *rightbuf, int samples);

OPENLASE_EXPORT void olSetAudioCallback(AudioCallbackFunc f);
#endif

OPENLASE_EXPORT void olLoadIdentity(void);
OPENLASE_EXPORT void olPushMatrix(void);
OPENLASE_EXPORT void olPopMatrix(void);

OPENLASE_EXPORT void olMultMatrix(float m[9]);
OPENLASE_EXPORT void olRotate(float theta);
OPENLASE_EXPORT void olTranslate(float x, float y);
OPENLASE_EXPORT void olScale(float sx, float sy);

OPENLASE_EXPORT void olLoadIdentity3(void);
OPENLASE_EXPORT void olPushMatrix3(void);
OPENLASE_EXPORT void olPopMatrix3(void);

OPENLASE_EXPORT void olMultMatrix3(float m[16]);
OPENLASE_EXPORT void olRotate3X(float theta);
OPENLASE_EXPORT void olRotate3Y(float theta);
OPENLASE_EXPORT void olRotate3Z(float theta);
OPENLASE_EXPORT void olTranslate3(float x, float y, float z);
OPENLASE_EXPORT void olScale3(float sx, float sy, float sz);

OPENLASE_EXPORT void olFrustum (float left, float right, float bot, float ttop, float near, float far);
OPENLASE_EXPORT void olPerspective(float fovy, float aspect, float zNear, float zFar);

OPENLASE_EXPORT void olResetColor(void);
OPENLASE_EXPORT void olMultColor(uint32_t color);
OPENLASE_EXPORT void olPushColor(void);
OPENLASE_EXPORT void olPopColor(void);

OPENLASE_EXPORT void olBegin(int prim);
OPENLASE_EXPORT void olVertex(float x, float y, uint32_t color);
OPENLASE_EXPORT void olVertex3(float x, float y, float z, uint32_t color, int repeat);
OPENLASE_EXPORT void olVertex2Z(float x, float y, float z, uint32_t color);
OPENLASE_EXPORT void olEnd(void);

OPENLASE_EXPORT void olTransformVertex(float *x, float *y);
OPENLASE_EXPORT void olTransformVertex3(float *x, float *y, float *z);
OPENLASE_EXPORT void olTransformVertex4(float *x, float *y, float *z, float *w);

typedef void (*ShaderFunc)(float *x, float *y, uint32_t *color);
typedef void (*Shader3Func)(float *x, float *y, float *z, uint32_t *color);

OPENLASE_EXPORT void olSetVertexPreShader(ShaderFunc f);
OPENLASE_EXPORT void olSetVertexShader(ShaderFunc f);
OPENLASE_EXPORT void olSetVertex3Shader(Shader3Func f);

OPENLASE_EXPORT void olSetPixelShader(ShaderFunc f);

OPENLASE_EXPORT void olRect(float x1, float y1, float x2, float y2, uint32_t color);
OPENLASE_EXPORT void olLine(float x1, float y1, float x2, float y2, uint32_t color);
OPENLASE_EXPORT void olDot(float x, float y, int points, uint32_t color);

OPENLASE_EXPORT float olRenderFrame(int max_fps);

OPENLASE_EXPORT void olGetFrameInfo(OLFrameInfo *info);

#ifdef HAS_JACK_SUPPORT
OPENLASE_EXPORT void olShutdown(void);
#endif

OPENLASE_EXPORT void olSetScissor (float x0, float y0, float x1, float y1);

OPENLASE_EXPORT void olLog(const char *fmt, ...);

typedef void (*LogCallbackFunc)(const char *msg);

OPENLASE_EXPORT void olSetLogCallback(LogCallbackFunc f);

OPENLASE_EXPORT OLRenderedFrame* olGetRenderedFrames();

#ifdef __cplusplus
}  // extern "C"
#endif

#endif
