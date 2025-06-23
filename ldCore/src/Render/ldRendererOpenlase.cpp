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

#include <cmath>

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>

#include <ldCore/ldCore.h>
#include <ldCore/Data/ldFrameBuffer.h>
#include <ldCore/Filter/ldFilterManager.h>

void logCallbackFunc(const char *msg)
{
    // timer to stop flooding of log with
    // Point buffer overflow (temp): need 30001 points, have 30000
    static QTimer s_logIntervalTimer;

    if(s_logIntervalTimer.isActive())
        return;

    qWarning().nospace() << msg;

    s_logIntervalTimer.setSingleShot(true);
    s_logIntervalTimer.setInterval(5000);
    s_logIntervalTimer.start();
}


/*!
  \class ldRendererOpenlase
  \brief Wrapper around openlase so it fits into the renderer interface.
  \inmodule rendering

*/
ldRendererOpenlase::ldRendererOpenlase(QObject *parent) :
    ldAbstractRenderer(parent)
    , m_filterManager(ldCore::instance()->filterManager())
    , m_libol(new ldLibol())
{
    m_libol->olSetLogCallback(&logCallbackFunc);
}

ldRendererOpenlase::~ldRendererOpenlase()
{
}

ldLibol* ldRendererOpenlase::getLibol() const
{
    return m_libol.get();
}

/////////////////////////////////////////////////////////////////////////
//                         Libol Module Functions
/////////////////////////////////////////////////////////////////////////

void ldRendererOpenlase::setRenderParams(OLRenderParams * params){
    m_libol->olSetRenderParams(params);
}

void ldRendererOpenlase::getRenderParams(OLRenderParams *params){
    m_libol->olGetRenderParams(params);
}

void ldRendererOpenlase::setFrameModes(int flags) {
    m_frameModes = flags;
}

const std::vector<ldVertex> &ldRendererOpenlase::getCachedFrame() const {
    return m_frame.frame();
}

size_t ldRendererOpenlase::getCachedFrameSize() const {
    return m_frame.frame().size();
}

bool ldRendererOpenlase::isRendererPaused() const
{
    return m_isRendererPaused;
}

ldFilter *ldRendererOpenlase::filter() const
{
    return m_filter;
}

void ldRendererOpenlase::setFilter(ldFilter *filter)
{
    m_filter = filter;
}

void ldRendererOpenlase::renderFrame(ldFrameBuffer * buffer, int max_fps, bool is3d, bool isVisPaused)
{
    ldCore::instance()->filterManager()->lock();
    // clear last frame
    if(!isVisPaused && !m_lastFrame.empty())
        m_lastFrame.clear();

    if(!isVisPaused || m_lastFrame.empty())
        createNewFrame(max_fps, is3d);

    if(isVisPaused) {
        if(m_lastFrame.empty())
            m_lastFrame = m_frame;
        else
            m_frame = m_lastFrame;
    }

    // apply data filter
    buffer->pushFrame(m_frame);

    // make sure not to pause renderer before it fills last frame
    m_isRendererPaused = isVisPaused;
    ldCore::instance()->filterManager()->unlock();

}


/////////////////////////////////////////////////////////////////////////
//                         Libol Graphic Operations
/////////////////////////////////////////////////////////////////////////

void ldRendererOpenlase::loadIdentity(void){
    m_libol->olLoadIdentity();
}

void ldRendererOpenlase::pushMatrix(void){
    m_libol->olPushMatrix();
}

void ldRendererOpenlase::popMatrix(void){
    m_libol->olPopMatrix();
}

void ldRendererOpenlase::multMatrix(float m[9]){
    m_libol->olMultMatrix(m);
}

void ldRendererOpenlase::rotate(float theta){
    m_libol->olRotate(theta);
}

void ldRendererOpenlase::translate(float x, float y){
    m_libol->olTranslate(x, y);
}

void ldRendererOpenlase::scale(float sx, float sy){
    m_libol->olScale(sx, sy);
}


void ldRendererOpenlase::loadIdentity3(void){
    m_libol->olLoadIdentity3();
}

void ldRendererOpenlase::pushMatrix3(void){
    m_libol->olPushMatrix3();
}

void ldRendererOpenlase::popMatrix3(void){
    m_libol->olPopMatrix3();
}


void ldRendererOpenlase::multMatrix3(float m[16]){
    m_libol->olMultMatrix3(m);
}

void ldRendererOpenlase::rotate3X(float theta){
    m_libol->olRotate3X(theta);
}

void ldRendererOpenlase::rotate3Y(float theta){
    m_libol->olRotate3Y(theta);
}

void ldRendererOpenlase::rotate3Z(float theta){
    m_libol->olRotate3Z(theta);
}

void ldRendererOpenlase::translate3(float x, float y, float z){
    m_libol->olTranslate3(x, y, z);
}

void ldRendererOpenlase::scale3(float sx, float sy, float sz){
    m_libol->olScale3(sx, sy, sz);
}


void ldRendererOpenlase::frustum (float left, float right, float bot, float top, float near, float far){
    m_libol->olFrustum(left, right, bot, top, near, far);
}

void ldRendererOpenlase::perspective(float fovy, float aspect, float zNear, float zFar){
    m_libol->olPerspective(fovy, aspect, zNear, zFar);
}


void ldRendererOpenlase::resetColor(void){
    m_libol->olResetColor();
}

void ldRendererOpenlase::multColor(uint32_t color){
    m_libol->olMultColor(color);
}

void ldRendererOpenlase::pushColor(void){
    m_libol->olPushColor();
}

void ldRendererOpenlase::popColor(void){
    m_libol->olPopColor();
}


void ldRendererOpenlase::begin(int prim){
    m_libol->olBegin(prim);
}

void ldRendererOpenlase::vertex(float x, float y, uint32_t color, int repeat)
{
    for(int i = 0; i < repeat; i++) {
        m_libol->olVertex(x, y, color);
    }
}

void ldRendererOpenlase::vertex3(float x, float y, float z, uint32_t color, int repeat){
    m_libol->olVertex3(x, y, z, color, repeat);
}

void ldRendererOpenlase::end(void){
    m_libol->olEnd();
}


void ldRendererOpenlase::transformVertex3(float *x, float *y, float *z){
    m_libol->olTransformVertex3(x, y, z);
}

void ldRendererOpenlase::rect(float x1, float y1, float x2, float y2, uint32_t color){
//    olRect(x1, y1, x2, y2, color);
    m_libol->olBegin(OL_LINESTRIP);
    vertex(x1,y1,color, 2);
    vertex(x1,y2,color, 5);
    vertex(x2,y2,color, 5);
    vertex(x2,y1,color, 5);
    vertex(x1,y1,color, 3);
    m_libol->olEnd();
}

void ldRendererOpenlase::line(float x1, float y1, float x2, float y2, uint32_t color){
    m_libol->olLine(x1, y1, x2, y2, color);
}

void ldRendererOpenlase::dot(float x, float y, int points, uint32_t color){
    m_libol->olDot(x, y, points, color);
}


void ldRendererOpenlase::setRenderParamsQuality(int min_fps) {
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
    if (min_fps>0) params.max_framelen = params.rate/min_fps;
    setRenderParams(&params);
}

void ldRendererOpenlase::setRenderParamsSpeed(int min_fps) {
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
    if (min_fps>0) params.max_framelen = params.rate/min_fps;
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

void ldRendererOpenlase::setRenderParamsStandard(int min_fps)
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
    params.curve_angle = cosf(30.0f*(M_PIf/180.0f)); // 30 deg
    params.end_dwell = 2;
    params.end_wait = 1;
    params.snap = 1/1000.0f;
    params.render_flags = RENDER_GRAYSCALE | RENDER_NOREVERSE | RENDER_NOREORDER;
    params.flatness = 0;
    params.min_length = 0;
    if (min_fps>0) params.max_framelen = params.rate/min_fps;
    setRenderParams(&params);
}

void ldRendererOpenlase::setRenderParamsBezier(int min_fps)
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
    params.curve_angle = cosf(30.0f*(M_PIf/180.0f)); // 30 deg
    params.end_dwell = 2;
    params.end_wait = 1;
    params.snap = 1/1000.0f;
    params.render_flags = RENDER_GRAYSCALE | RENDER_NOREVERSE | RENDER_NOREORDER;
    params.min_length = 0;
    if (min_fps>0) params.max_framelen = params.rate/min_fps;
    params.flatness = 0.00001f; //for beziers to work (thanks Alec!)
    setRenderParams(&params);
}

void ldRendererOpenlase::setRenderParamsLower(int min_fps)
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
    params.curve_angle = cosf(30.0f*(M_PIf/180.0f)); // 30 deg
    params.end_dwell = 2;
    params.end_wait = 1;
    params.snap = 1/500.0f;
    params.render_flags = RENDER_GRAYSCALE | RENDER_NOREVERSE | RENDER_NOREORDER;
    params.flatness = 0;
    params.min_length = 0;
    if (min_fps>0) params.max_framelen = params.rate/min_fps;
    setRenderParams(&params);
}

void ldRendererOpenlase::setRenderParamsBeam(int min_fps) {
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
    if (min_fps>0) params.max_framelen = params.rate/min_fps;
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
    int nPoints = static_cast<int>(nPointsMax * radius);
    if (nPoints < nPointsMin) nPoints = nPointsMin;
    //nPoints = MIN(MAX(nPointsMin, nPoints), nPointsMax);
    begin(OL_POINTS);
    for (int i = 0-overlap; i <= nPoints+overlap; i++) {
        float f = static_cast<float>(i) / nPoints;
        float a = seamAngle + f*M_PIf*2;
        float jx = cos(a);
        float jy = sin(a);
        uint32_t tc = 0;
        if (i >= 0 && i <= nPoints) tc = color;
        vertex(x+jx*radius, y+jy*radius, tc);
    }
    end();
}

float ldRendererOpenlase::getLastFrameDeltaSeconds() {
    return static_cast<float>(m_lastFramePointCount) / rate();
}

void ldRendererOpenlase::createNewFrame(int max_fps, bool is3d,bool skipFilters)
{
    float result = m_libol->olRenderFrame(max_fps);
    if(result == -1.f)
        return;

    ldLibol::OLRenderedFrame * rendered_frame = m_libol->olGetRenderedFrames();
    m_lastFramePointCount = rendered_frame->pnext;

//    {static int s; s=(s+1)%100; int z=olGetRenderedFrames()->pnext; if (!s || z > 30001) qDebug() << "frames point count:" << z;}

    m_frame.reserve(static_cast<uint>(rendered_frame->pnext));
    m_frame.clear();

    ldVertex v = {{0, 0}, {0, 0, 0}};
    for(uint i = 0; i < static_cast<uint>(rendered_frame->pnext); i++ ){
        OLPoint *p = rendered_frame->points+i;
        v.x() = p->x;
        v.y() = p->y;
        v.r() =((p->color & 0xFF0000) >> 16) / 255.0f;
        v.g() =((p->color & 0x00FF00) >> 8) / 255.0f;
        v.b() =((p->color & 0x0000FF) >> 0) / 255.0f;

        m_frame.push_back(std::move(v));
    }

    if (!skipFilters) {
        // filter openlase result
        m_filterManager->resetFilters();
        if(m_filter)
            m_filter->resetFilter();
        m_filterManager->setFrameModes(m_frameModes);

        bool mode_disable_rotate = m_filterManager->dataFilter()->frameModes & FRAME_MODE_DISABLE_ROTATION;
        if(!mode_disable_rotate && !is3d) {
            for(ldVertex &frameV : m_frame.frame())
                m_filterManager->rotate3dFilter()->processFilter(frameV);
        }

        m_filterManager->processFrame1(m_frame);

        if(m_filter)
            for(uint i = 0; i < m_frame.size(); i++)
                m_filter->processFilter(m_frame[i]);

        m_filterManager->processFrame2(m_frame);
    }
}
