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

#include "ldCore/Render/ldRendererOpenlase.h"

#include <math.h>

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

#include "ldCore/Data/ldFrameBuffer.h"


void logCallbackFunc(const char *msg)
{
    qDebug().nospace() << msg;
}


/*!
  \class ldRendererOpenlase
  \brief Wrapper around openlase so it fits into the renderer interface.
  \inmodule rendering

*/
ldRendererOpenlase::ldRendererOpenlase(QObject *parent) :
    ldAbstractRenderer(OPENLASE, parent)
{
    olSetLogCallback(&logCallbackFunc);

    // Increased to 30000 to avoid RenderedFrame buffer overflow error.
    if(olInit(0, 30000) != 0) {
        qFatal("Error initializing openlase!!!");
        qApp->quit();
        return;
    }
}


/////////////////////////////////////////////////////////////////////////
//                         Libol Module Functions
/////////////////////////////////////////////////////////////////////////

void ldRendererOpenlase::setRenderParams(OLRenderParams * params){
    olSetRenderParams(params);
}

void ldRendererOpenlase::getRenderParams(OLRenderParams *params){
    olGetRenderParams(params);
}

void ldRendererOpenlase::setFrameModes(int flags) {
    m_frameModes = flags;
}

const std::vector<Vertex> &ldRendererOpenlase::getCachedFrame() const {
    return m_cachedFrame;
}

float ldRendererOpenlase::renderFrame(ldFrameBuffer * buffer, int max_fps){

    const float result = olRenderFrame(max_fps);
    OLRenderedFrame * rendered_frame = olGetRenderedFrames();
    m_lastFramePointCount = rendered_frame->pnext;

//    {static int s; s=(s+1)%100; int z=olGetRenderedFrames()->pnext; if (!s || z > 30001) qDebug() << "frames point count:" << z;}

    Vertex v = {{0, 0, 0}, {0, 0, 0, 0}};
    m_cachedFrame.resize(rendered_frame->pnext);

    buffer->setFrameModes(m_frameModes);
    for(int i = 0; i < rendered_frame->pnext; i++ ){
        OLPoint *p = rendered_frame->points+i;
        v.position[0] = p->x;
        v.position[1] = p->y;
        v.position[2] = 0.0f;
        v.color[0] =((p->color & 0xFF0000) >> 16) / 255.0f;
        v.color[1] =((p->color & 0x00FF00) >> 8) / 255.0f;
        v.color[2] =((p->color & 0x0000FF) >> 0) / 255.0f;
        v.color[3] = 1.0f;
        buffer->push(v, false, true); // also alters v with global filter
        m_cachedFrame[i] = v;
    }

    return result;
}


/////////////////////////////////////////////////////////////////////////
//                         Libol Graphic Operations
/////////////////////////////////////////////////////////////////////////

void ldRendererOpenlase::loadIdentity(void){
    olLoadIdentity();
}

void ldRendererOpenlase::pushMatrix(void){
    olPushMatrix();
}

void ldRendererOpenlase::popMatrix(void){
    olPopMatrix();
}

void ldRendererOpenlase::multMatrix(float m[9]){
    olMultMatrix(m);
}

void ldRendererOpenlase::rotate(float theta){
    olRotate(theta);
}

void ldRendererOpenlase::translate(float x, float y){
    olTranslate(x, y);
}

void ldRendererOpenlase::scale(float sx, float sy){
    olScale(sx, sy);
}


void ldRendererOpenlase::loadIdentity3(void){
    olLoadIdentity3();
}

void ldRendererOpenlase::pushMatrix3(void){
    olPushMatrix3();
}

void ldRendererOpenlase::popMatrix3(void){
    olPopMatrix3();
}


void ldRendererOpenlase::multMatrix3(float m[16]){
    olMultMatrix3(m);
}

void ldRendererOpenlase::rotate3X(float theta){
    olRotate3X(theta);
}

void ldRendererOpenlase::rotate3Y(float theta){
    olRotate3Y(theta);
}

void ldRendererOpenlase::rotate3Z(float theta){
    olRotate3Z(theta);
}

void ldRendererOpenlase::translate3(float x, float y, float z){
    olTranslate3(x, y, z);
}

void ldRendererOpenlase::scale3(float sx, float sy, float sz){
    olScale3(sx, sy, sz);
}


void ldRendererOpenlase::frustum (float left, float right, float bot, float top, float near, float far){
    olFrustum(left, right, bot, top, near, far);
}

void ldRendererOpenlase::perspective(float fovy, float aspect, float zNear, float zFar){
    olPerspective(fovy, aspect, zNear, zFar);
}


void ldRendererOpenlase::resetColor(void){
    olResetColor();
}

void ldRendererOpenlase::multColor(uint32_t color){
    olMultColor(color);
}

void ldRendererOpenlase::pushColor(void){
    olPushColor();
}

void ldRendererOpenlase::popColor(void){
    olPopColor();
}


void ldRendererOpenlase::begin(int prim){
    olBegin(prim);
}

void ldRendererOpenlase::vertex(float x, float y, uint32_t color, int repeat)
{
    for(int i = 0; i < repeat; i++) {
        olVertex(x, y, color);
    }
}

void ldRendererOpenlase::vertex3(float x, float y, float z, uint32_t color, int repeat){
    olVertex3(x, y, z, color, repeat);
}

void ldRendererOpenlase::end(void){
    olEnd();
}


void ldRendererOpenlase::transformVertex3(float *x, float *y, float *z){
    olTransformVertex3(x, y, z);
}

void ldRendererOpenlase::rect(float x1, float y1, float x2, float y2, uint32_t color){
//    olRect(x1, y1, x2, y2, color);
    olBegin(OL_LINESTRIP);
    vertex(x1,y1,color, 2);
    vertex(x1,y2,color, 5);
    vertex(x2,y2,color, 5);
    vertex(x2,y1,color, 5);
    vertex(x1,y1,color, 3);
    olEnd();
}

void ldRendererOpenlase::line(float x1, float y1, float x2, float y2, uint32_t color){
    olLine(x1, y1, x2, y2, color);
}

void ldRendererOpenlase::dot(float x, float y, int points, uint32_t color){
    olDot(x, y, points, color);
}


void ldRendererOpenlase::setRenderParamsQuality() {
    OLRenderParams params;
    memset(&params, 0, sizeof(params));
    params.rate = rate();
    params.on_speed = 2.0f / 80.0f;
    params.off_speed = 2.0f / 80.0f;
    params.start_wait = 6;
    params.start_dwell = 1;
    params.curve_dwell = 1;
    params.corner_dwell = 1;
    params.end_dwell = 1;
    params.end_wait = 2;
    params.curve_angle = cosf(30.0f * (3.14f / 180.0f)); // 30 deg
    params.flatness = 0;
    params.min_length = 0;
    params.snap = 0;
    params.render_flags = 0;
    setRenderParams(&params);
}

void ldRendererOpenlase::setRenderParamsSpeed() {
    OLRenderParams params;
    memset(&params, 0, sizeof(params));
    params.rate = rate();
    params.on_speed = 2.0f / 35.0f;
    params.off_speed = 2.0f / 45.0f;
    params.start_wait = 2;
    params.start_dwell = 0;
    params.curve_dwell = 0;
    params.corner_dwell = 0;
    params.end_dwell = 0;
    params.end_wait = 1;
    params.curve_angle = cosf(30.0f * (3.14f / 180.0f)); // 30 deg
    params.flatness = 0;
    params.min_length = 0;
    params.snap = 0;
    params.render_flags = RENDER_NOREVERSE | RENDER_NOREORDER;
    setRenderParams(&params);
}

void ldRendererOpenlase::setRenderParamsRaw() {
    OLRenderParams params;
    memset(&params, 0, sizeof(params));
    params.rate = rate();
    params.on_speed = 999;
    params.off_speed = 999;
    params.snap = 0;
    params.render_flags = RENDER_NOREVERSE | RENDER_NOREORDER;
    setRenderParams(&params);
}

void ldRendererOpenlase::setRenderParamsStandard()
{
    //
    OLRenderParams params;
    memset(&params, 0, sizeof params);
    params.rate = rate();
    params.on_speed = 2.0/32.0;
    params.off_speed = 2.0/32.0;
    params.start_wait = 8;
    params.start_dwell = 2;
    params.curve_dwell = 1;
    params.corner_dwell = 3;
    params.curve_angle = cosf(30.0f*((float) M_PI/180.0f)); // 30 deg
    params.end_dwell = 2;
    params.end_wait = 1;
    params.snap = 1/1000.0f;
    params.render_flags = RENDER_GRAYSCALE | RENDER_NOREVERSE | RENDER_NOREORDER;
    params.flatness = 0;
    params.min_length = 0;
    params.max_framelen = params.rate/30;
    setRenderParams(&params);
}

void ldRendererOpenlase::setRenderParamsBezier()
{
    //
    OLRenderParams params;
    memset(&params, 0, sizeof params);
    params.rate = rate();
    params.on_speed = 2.0/32.0;
    params.off_speed = 2.0/32.0;
    params.start_wait = 8; // 16
    params.start_dwell = 2;
    params.curve_dwell = 1;
    params.corner_dwell = 3;
    params.curve_angle = cosf(30.0f*((float) M_PI/180.0f)); // 30 deg
    params.end_dwell = 2;
    params.end_wait = 1;
    params.snap = 1/1000.0f;
    params.render_flags = RENDER_GRAYSCALE | RENDER_NOREVERSE | RENDER_NOREORDER;
    params.min_length = 0;
    params.max_framelen = params.rate/30;
    params.flatness = 0.00001f; //for beziers to work (thanks Alec!)
    setRenderParams(&params);
}

void ldRendererOpenlase::setRenderParamsLower()
{
    //
    OLRenderParams params;
    memset(&params, 0, sizeof params);
    params.rate = rate();
    //    params.on_speed = 2.0/32.0;
    //    params.off_speed = 2.0/32.0;

    params.on_speed = 1.0/64.0;
    params.off_speed = 1.0/64.0;

    params.start_wait = 8;
    params.start_dwell = 2;
    params.curve_dwell = 1;
    params.corner_dwell = 3;
    params.curve_angle = cosf(30.0f*((float) M_PI/180.0f)); // 30 deg
    params.end_dwell = 2;
    params.end_wait = 1;
    params.snap = 1/500.0f;
    params.render_flags = RENDER_GRAYSCALE | RENDER_NOREVERSE | RENDER_NOREORDER;
    params.flatness = 0;
    params.min_length = 0;
    params.max_framelen = params.rate/30;
    setRenderParams(&params);
}

void ldRendererOpenlase::setRenderParamsBeam() {
    OLRenderParams params;
    memset(&params, 0, sizeof(params));
    params.rate =rate();
    params.on_speed = 2.0f / 50.0f;
    params.off_speed = 2.0f / 50.0f;
    params.start_wait = 3;
    params.start_dwell = 0;
    params.curve_dwell = 0;
    params.corner_dwell = 0;
    params.end_dwell = 0;
    params.end_wait = 2;
    params.curve_angle = cosf(30.0f * (3.14f / 180.0f)); // 30 deg
    params.flatness = 0;
    params.min_length = 0;
    params.snap = 0;//1.0f / 40.0f;
    params.render_flags = RENDER_NOREVERSE | RENDER_NOREORDER;
    setRenderParams(&params);
}


void ldRendererOpenlase::drawPoints(float x, float y, uint32_t color, int nPoints, float jitter) {
    begin(OL_POINTS);
    for (int i = 0; i < nPoints; i++) {
        float jx = (rand()%1234)/1233.0f*2-1;
        float jy = (rand()%1234)/1233.0f*2-1;
        vertex(x+jx*jitter, y+jy*jitter, color);
    }
    end();
}

void ldRendererOpenlase::drawCircle(float x, float y, float radius, uint32_t color, float seamAngle, int nPointsMin, int nPointsMax, int overlap) {
    int nPoints = nPointsMax * radius;
    if (nPoints < nPointsMin) nPoints = nPointsMin;
    //nPoints = MIN(MAX(nPointsMin, nPoints), nPointsMax);
    begin(OL_POINTS);
    for (int i = 0-overlap; i <= nPoints+overlap; i++) {
        float f = ((float)i)/nPoints;
        float a = seamAngle + f*3.14*2;
        float jx = cos(a);
        float jy = sin(a);
        uint32_t tc = 0;
        if (i >= 0 && i <= nPoints) tc = color;
        vertex(x+jx*radius, y+jy*radius, tc);
    }
    end();
}

float ldRendererOpenlase::getLastFrameDeltaSeconds() {
    return m_lastFramePointCount / (float) rate();
}

