/*
		OpenLase - a realtime laser graphics toolkit

Wicked Lasers Object version of libol

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

#include "openlase/ldLibol.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <memory.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#define SMALL_Z 0.00001f

#define POINT(x, y, z, color) {x,y,z,color}

ldLibol::ldLibol(int buffer_count, int max_points)
{
    int i;

    if (buffer_count < 0) buffer_count = 0;

    memset(&dstate, 0, sizeof(dstate));
    memset(&last_render_point, 0, sizeof(last_render_point));

    buflag = buffer_count;
    fbufs = buffer_count+1;

    cwbuf = 0;
    out_point = -1;
    first_time_full = 0;
    first_output_frame = 0;
    memset(&wframe, 0, sizeof(Frame));
    wframe.objmax = 16;
    wframe.objects = (ldLibol::Object*)malloc(wframe.objmax * sizeof(ldLibol::Object));
    wframe.psmax = max_points;
    wframe.points = (OLPoint*)malloc(wframe.psmax * sizeof(OLPoint));
    frames = (ldLibol::OLRenderedFrame*)malloc(fbufs * sizeof(ldLibol::OLRenderedFrame));
    for (i=0; i<fbufs; i++) {
        memset(&frames[i], 0, sizeof(ldLibol::OLRenderedFrame));
        frames[i].pmax = max_points;
        frames[i].points = (OLPoint*)malloc(frames[i].pmax * sizeof(OLPoint));
    }

    olLoadIdentity();
    for(i=0; i<MTX_STACK_DEPTH; i++)
        olPushMatrix();
    mtx2dp = 0;

    olLoadIdentity3();
    for(i=0; i<MTX_STACK_DEPTH; i++)
        olPushMatrix3();
    mtx3dp = 0;

    olResetColor();
    for(i=0; i<MTX_STACK_DEPTH; i++)
        olPushColor();
    coldp = 0;

    bbox[0][0] = -1;
    bbox[0][1] = -1;
    bbox[1][0] = 1;
    bbox[1][1] = 1;

    vpreshader = nullptr;
    vshader = nullptr;
    v3shader = nullptr;
    pshader = nullptr;

    // set some default safe param values
    memset(&params, 0, sizeof(params));
    params.rate = 30000;
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
    params.max_framelen = params.rate/45;
}

ldLibol::~ldLibol()
{
    free(wframe.objects);
    free(wframe.points);
    for (int i=0; i<fbufs; i++) {
        free(frames[i].points);
    }
    free(frames);
}

void ldLibol::olSetRenderParams(struct OLRenderParams *sp)
{
    params = *sp;
}

void ldLibol::olGetRenderParams(struct OLRenderParams *sp) const
{
    *sp = params;
}

uint32_t ldLibol::colmul(uint32_t a, uint32_t b) const
{
    uint32_t out = 0;
    out |= ((a&0xff)*(b&0xff)) / 255;
    out |= ((((a>>8)&0xff)*((b>>8)&0xff)) / 255)<<8;
    out |= ((((a>>16)&0xff)*((b>>16)&0xff)) / 255)<<16;
    return out;
}

OLPoint *ldLibol::ps_alloc(int count)
{
    OLPoint *ret;
    if ((count + wframe.psnext) > wframe.psmax) {
        olLog("Point buffer overflow (temp): need %d points, have %d\n", count + wframe.psnext, wframe.psmax);
        return 0;
    }
    ret = wframe.points + wframe.psnext;
    wframe.psnext += count;
    return ret;
}

void ldLibol::olBegin(int prim)
{
    if (dstate.curobj)
        return;
    if (wframe.objmax == wframe.objcnt) {
        wframe.objmax *= 2;
        wframe.objects = (ldLibol::Object*) realloc(wframe.objects, wframe.objmax * sizeof(ldLibol::Object));
    }
    dstate.curobj = wframe.objects + wframe.objcnt;
    memset(dstate.curobj, 0, sizeof(ldLibol::Object));
    memcpy(dstate.curobj->bbox, bbox, sizeof(bbox));
    dstate.curobj->points = wframe.points + wframe.psnext;
    dstate.prim = prim;
    dstate.state = 0;
    dstate.points = 0;
}

void ldLibol::addpoint(const float &x, const float &y, const float &z, const uint32_t &color)
{
    if (dstate.curobj->pointcnt >= wframe.psmax) {
        if (maxpoints_warn_once) {
            maxpoints_warn_once = 0;
            olLog("object has many points %d\n", dstate.curobj->pointcnt);
        }
        return;
    }
    OLPoint *pnt = ps_alloc(1);
    if(!pnt) return;
    pnt->x = x;
    pnt->y = y;
    pnt->z = z;
    pnt->color = color;
    dstate.curobj->pointcnt++;
}

int ldLibol::get_dwell(const float &x, const float &y) const
{
    if (dstate.points == 1) {
        return params.start_dwell;
    } else {
        float ex = dstate.last_point.x;
        float ey = dstate.last_point.y;
        float ecx = dstate.last_slope.x;
        float ecy = dstate.last_slope.y;
        float sx = ex;
        float sy = ey;
        float scx = x;
        float scy = y;
        float dex = ecx-ex;
        float dey = ecy-ey;
        float dsx = sx-scx;
        float dsy = sy-scy;
        float dot = dex*dsx + dey*dsy;
        float lens = sqrtf(dex*dex+dey*dey) * sqrtf(dsx*dsx+dsy*dsy);
        //olLog("%f,%f -> %f,%f -> %f,%f\n", ecx,ecy,ex,ey,x,y);
        if (lens == 0) {
            //olLog("deg cor\n");
            return params.corner_dwell;
        } else {
            dot = dot / lens;
            if (dot > params.curve_angle) {
                //olLog("curve\n");
                return params.curve_dwell;
            } else {
                //olLog("cor\n");
                return params.corner_dwell;
            }
        }
    }
}

void ldLibol::line_to(const float &x, const float &y, const float &z, const uint32_t &color)
{
    int dwell, i;

    if (dstate.points == 0) {
        addpoint(x,y,z,color);
        dstate.points++;
        dstate.last_point = POINT(x,y,z,color);
        return;
    }
    dwell = get_dwell(x, y);
    OLPoint last = dstate.last_point;
    for (i=0; i<dwell; i++)
        addpoint(last.x,last.y,last.z,last.color);
    float dx = x - last.x;
    float dy = y - last.y;
    float dz = z - last.z;
    float distance = fmaxf(fabsf(dx),fabsf(dy));
    int points = ceilf(distance/params.on_speed);
    for (i=1; i<=points; i++) {
        addpoint(last.x + (dx/(float)points) * i,
                 last.y + (dy/(float)points) * i,
                 last.z + (dz/(float)points) * i,
                 color);
    }
    dstate.last_slope = dstate.last_point;
    dstate.last_point = POINT(x,y,z,color);
    dstate.points++;
}

void ldLibol::recurse_bezier(const float &x1, const float &y1, const float &x2, const float &y2, const float &x3, const float &y3, const uint32_t &color, const int &depth)
{
    float x0 = dstate.last_point.x;
    float y0 = dstate.last_point.y;
    int subdivide = 0;

    if (depth > 100) {
        olLog("Bezier recurse error: %f,%f %f,%f %f,%f %f,%f\n", x0, y0, x1, y1, x2, y2, x3, y3);
        return;
    }

    float dx = x3-x0;
    float dy = y3-y0;
    float distance = fmaxf(fabsf(dx),fabsf(dy));
    if (distance > params.on_speed) {
        subdivide = 1;
    } else {
        float ux = (3.0*x1 - 2.0*x0 - x3); ux = ux * ux;
        float uy = (3.0*y1 - 2.0*y0 - y3); uy = uy * uy;
        float vx = (3.0*x2 - 2.0*x3 - x0); vx = vx * vx;
        float vy = (3.0*y2 - 2.0*y3 - y0); vy = vy * vy;
        if (ux < vx)
            ux = vx;
        if (uy < vy)
            uy = vy;
        if ((ux+uy) > params.flatness)
            subdivide = 1;
    }

    // was added by Wicked Lasers
    if (depth > 5) subdivide = 0; // shortcut

    if (subdivide) {
        //de Casteljau at t=0.5
        float mcx = (x1 + x2) * 0.5;
        float mcy = (y1 + y2) * 0.5;
        float ax1 = (x0 + x1) * 0.5;
        float ay1 = (y0 + y1) * 0.5;
        float ax2 = (ax1 + mcx) * 0.5;
        float ay2 = (ay1 + mcy) * 0.5;
        float bx2 = (x2 + x3) * 0.5;
        float by2 = (y2 + y3) * 0.5;
        float bx1 = (bx2 + mcx) * 0.5;
        float by1 = (by2 + mcy) * 0.5;
        float xm = (ax2 + bx1) * 0.5;
        float ym = (ay2 + by1) * 0.5;
        recurse_bezier(ax1, ay1, ax2, ay2, xm, ym, color, depth+1);
        recurse_bezier(bx1, by1, bx2, by2, x3, y3, color, depth+1);
    } else {
        addpoint(x3, y3, 0, color);
        dstate.last_point = POINT(x3,y3,0,color);
    }
}

void ldLibol::bezier_to(const float &x, const float &y, const uint32_t &color)
{
    int dwell, i;

    auto near = [&](OLPoint &a, OLPoint &b){
        float dx = a.x - b.x;
        float dy = a.y - b.y;
        return sqrtf(dx*dx+dy*dy) <= params.snap;
    };

    if (dstate.points == 0) {
        addpoint(x,y,0,color);
        dstate.points++;
        dstate.last_point = POINT(x,y,0,color);
        return;
    }

    switch(dstate.state) {
        case 0:
            dstate.c1 = POINT(x,y,0,color);
            dstate.state++;
            return;
        case 1:
            dstate.c2 = POINT(x,y,0,color);
            dstate.state++;
            return;
        case 2:
            break;
    }

    if (near(dstate.last_point, dstate.c1))
        dwell = get_dwell(dstate.c2.x, dstate.c2.y);
    else
        dwell = get_dwell(dstate.c1.x, dstate.c1.y);

    OLPoint last = dstate.last_point;
    for (i=0; i<dwell; i++)
        addpoint(last.x,last.y,last.z,last.color);

    recurse_bezier(dstate.c1.x, dstate.c1.y, dstate.c2.x, dstate.c2.y, x, y, color, 0);

    dstate.last_point = POINT(x,y,0,color);
    if (near(dstate.c2, dstate.last_point))
        dstate.last_slope = dstate.c1;
    else
        dstate.last_slope = dstate.c2;
    dstate.points++;
    dstate.state = 0;
}

void ldLibol::point_to(const float &x, const float &y, const float &z, const uint32_t &color)
{
    int i;
    addpoint(x,y,z, color);
    if (dstate.points == 0)
        for (i=0; i<params.start_dwell; i++)
            addpoint(x,y,z,color);

    dstate.points++;
    return;
}

void ldLibol::olTransformVertex(float *x, float *y) const
{
    float nx = mtx2d[0][0] * *x + mtx2d[0][1] * *y + mtx2d[0][2];
    float ny = mtx2d[1][0] * *x + mtx2d[1][1] * *y + mtx2d[1][2];
    float nw = mtx2d[2][0] * *x + mtx2d[2][1] * *y + mtx2d[2][2];

    *x = nx / nw;
    *y = ny / nw;
}

void ldLibol::olVertex2Z(float x, float y, float z, uint32_t color)
{
    if (x != x || y != y)
    {
//		olLog("x = %f, y = %f\n", x, y);
        return;
    }

    if (!dstate.curobj)
        return;

    if(vpreshader)
        vpreshader(&x, &y, &color);

    color = colmul(color,curcol);

    olTransformVertex(&x, &y);

    if (x != x || y != y)
    {
//		olLog("x = %f, y = %f\n", x, y);
        return;
    }

    if(vshader)
        vshader(&x, &y, &color);

    switch (dstate.prim) {
        case OL_LINESTRIP:
            line_to(x,y,z,color);
            break;
        case OL_BEZIERSTRIP:
            bezier_to(x,y,color);
            break;
        case OL_POINTS:
            point_to(x,y,z,color);
            break;
    }
}

void ldLibol::olVertex(float x, float y, uint32_t color)
{
    olVertex2Z(x, y, 0, color);
}

void ldLibol::olEnd(void)
{
    int i;
    if (!dstate.curobj)
        return;
    if (dstate.points < 2) {
        dstate.curobj = NULL;
        return;
    }
    OLPoint *last = dstate.curobj->points + dstate.curobj->pointcnt - 1;
    for (i=0; i<params.end_dwell; i++)
        addpoint(last->x,last->y,last->z,last->color);

    if(pshader) {
        for (i=0; i<dstate.curobj->pointcnt; i++) {
            pshader(&dstate.curobj->points[i].x, &dstate.curobj->points[i].y, &dstate.curobj->points[i].color);
        }
    }

    if (p3shader) {
        for (i=0; i<dstate.curobj->pointcnt; i++) {
            p3shader(&dstate.curobj->points[i].x,
                     &dstate.curobj->points[i].y,
                     &dstate.curobj->points[i].z,
                     &dstate.curobj->points[i].color);
        }
    }

    int nl=0,nr=0,nu=0,nd=0;
    for (i=0; i<dstate.curobj->pointcnt; i++) {
        if (!dstate.curobj->points[i].color)
            continue;
        if (dstate.curobj->points[i].x > -1)
            nl = 1;
        if (dstate.curobj->points[i].x < 1)
            nr = 1;
        if (dstate.curobj->points[i].y > -1)
            nd = 1;
        if (dstate.curobj->points[i].y < 1)
            nu = 1;

        if (nl && nr && nu && nd)
            break;
    }
    if (nl && nr && nu && nd)
        wframe.objcnt++;
    dstate.curobj = NULL;
}

int ldLibol::chkpts(const int &count) const
{
    if (frames[cwbuf].pnext + count > frames[cwbuf].pmax) {
        olLog("Point buffer overflow (final): need %d points, have %d\n",
              count + frames[cwbuf].pnext, frames[cwbuf].pmax);
        return 0;
    }

    return 1;
}

void ldLibol::addrndpoint(const float &x, const float &y, const uint32_t &color)
{
    if (frames[cwbuf].pnext >= frames[cwbuf].pmax) {
        olLog("point overflow\n");
        return;
    }
    frames[cwbuf].points[frames[cwbuf].pnext].x = x;
    frames[cwbuf].points[frames[cwbuf].pnext].y = y;
    frames[cwbuf].points[frames[cwbuf].pnext].color = color;
    frames[cwbuf].pnext++;
}

int ldLibol::render_object(Object *obj)
{
    int i,j;
    // heuristic... might overflow in pathological cases
    if(!chkpts(3 * (obj->pointcnt + params.start_wait + params.end_wait)))
        return 0;

    OLPoint *ip = obj->points;
    for (i=0; i<obj->pointcnt; i++, ip++) {
        if (ip->x < obj->bbox[0][0] || ip->x > obj->bbox[1][0] ||
                ip->y < obj->bbox[0][1] || ip->y > obj->bbox[1][1])
            continue;
        if (ip->color != C_BLACK)
            break;
    }
    if (i == obj->pointcnt) // null object
        return 0;

    ip = obj->points;
    int prev_inside = 0;
    for (i=0; i<obj->pointcnt; i++, ip++) {
        int inside = 1;
        if (ip->x < obj->bbox[0][0] || ip->x > obj->bbox[1][0] ||
                ip->y < obj->bbox[0][1] || ip->y > obj->bbox[1][1])
            inside = 0;
        if (ip->color == 0 && (params.render_flags & RENDER_CULLDARK))
            inside = 0;
        if (inside && !prev_inside) {
            float dx = ip->x - last_render_point.x;
            float dy = ip->y - last_render_point.y;
            float distance = fmaxf(fabsf(dx),fabsf(dy));
            int points = ceilf(distance/params.off_speed);
            if (distance > params.snap) {
                for (j=0; j<params.end_wait; j++) {
                    addrndpoint(last_render_point.x, last_render_point.y, C_BLACK);
                }
                for (j=0; j<points; j++) {
                    addrndpoint(last_render_point.x + (dx/(float)points) * j,
                                last_render_point.y + (dy/(float)points) * j,
                                C_BLACK);
                }
                for (j=0; j<params.start_wait; j++) {
                    addrndpoint(ip->x, ip->y, C_BLACK);
                }
            }
        }
        if (inside) {
            last_render_point = *ip;
            addrndpoint(ip->x, ip->y, ip->color);
        }
        prev_inside = inside;
    }
    return 1;
}

float ldLibol::olRenderFrame(int max_fps)
{
	int i;
	int count = 0;

	int min_points = params.rate / max_fps;

	memset(&last_info, 0, sizeof(last_info));
    maxpoints_warn_once = 1;

	frames[cwbuf].pnext=0;
	int cnt = wframe.objcnt;
	float dclosest = 0;
	int clinv = 0;

	if (!(params.render_flags & RENDER_NOREORDER)) {
		OLPoint closest_to = {-1,-1,-1, 0}; // first look for the object nearest the topleft
		while(cnt) {
			Object *closest = NULL;
			for (i=0; i<wframe.objcnt; i++) {
				if (!wframe.objects[i].pointcnt)
					continue;
				if (wframe.objects[i].pointcnt < params.min_length)
					continue;
				float dx = wframe.objects[i].points[0].x - closest_to.x;
				float dy = wframe.objects[i].points[0].y - closest_to.y;
				float distance = fmaxf(fabsf(dx),fabsf(dy)) + 0.01*(fabsf(dx)+fabsf(dy));
				if (!closest || distance < dclosest) {
					closest = &wframe.objects[i];
					clinv = 0;
					dclosest = distance;
				}
				if (!(params.render_flags & RENDER_NOREVERSE)) {
					dx = wframe.objects[i].points[wframe.objects[i].pointcnt-1].x - closest_to.x;
					dy = wframe.objects[i].points[wframe.objects[i].pointcnt-1].y - closest_to.y;
					distance = fmaxf(fabsf(dx),fabsf(dy)) + 0.01*(fabsf(dx)+fabsf(dy));
					if(!closest || distance < dclosest) {
						closest = &wframe.objects[i];
						clinv = 1;
						dclosest = distance;
					}
				}
			}
			if (!closest)
				break;
			if (clinv) {
				OLPoint *pt = closest->points;
				int cnt = closest->pointcnt;
				for (i=0; i<cnt/2; i++) {
					OLPoint tmp = pt[i];
					pt[i] = pt[cnt-i-1];
					pt[cnt-i-1] = tmp;
				}
			}
			//olLog("%d (%d) (nearest to %f,%f)\n", closest - wframe.objects, closest->pointcnt, closest_to.x, closest_to.y);
			if (render_object(closest))
				closest_to = last_render_point;
			//olLog("[%d] ", frames[cwbuf].pnext);
			//olLog("[LRP:%f %f]\n", last_render_point.x, last_render_point.y);
			closest->pointcnt = 0;
			cnt--;
			last_info.objects++;
		}
		//olLog("\n");
	} else {
		for (i=0; i<wframe.objcnt; i++) {
			if (wframe.objects[i].pointcnt < params.min_length)
				continue;
			render_object(&wframe.objects[i]);
		}
	}
	wframe.psnext = 0;
	wframe.objcnt = 0;
	count = frames[cwbuf].pnext;
	last_info.points = count;

	if (params.max_framelen && count > params.max_framelen)
	{
		int in_count = count;
		int out_count = params.max_framelen;
		if(!chkpts(count)) return -1;

		OLPoint *pin = frames[cwbuf].points;
		OLPoint *pout = &pin[in_count];

		float pos = 0;
		float delta = count / (float)out_count;

		count = 0;
		while (pos < (in_count - 1)) {
			int ipos = pos;
			float rest = pos - ipos;

			pout->x = pin[ipos].x * (1-rest) + pin[ipos+1].x * rest;
			pout->y = pin[ipos].y * (1-rest) + pin[ipos+1].y * rest;

			if (pin[ipos].color == C_BLACK || pin[ipos+1].color == C_BLACK) {
				pout->color = C_BLACK;
				pos += 1;
				last_info.resampled_blacks++;
			} else {
				pout->color = pin[ipos].color;
				pos += delta;
			}

			pout++;
			count++;
		}

		memcpy(pin, &pin[in_count], count * sizeof(*pin));
		frames[cwbuf].pnext = count;
		if(!chkpts(0)) return -1;
		last_info.resampled_points = count;
	}

	if (count) {
		last_render_point.x = frames[cwbuf].points[count-1].x;
		last_render_point.y = frames[cwbuf].points[count-1].y;
	}
	while(count < min_points) {
		frames[cwbuf].points[count].x = last_render_point.x;
		frames[cwbuf].points[count].y = last_render_point.y;
		frames[cwbuf].points[count].color = C_BLACK;
		count++;
		last_info.padding_points++;
	}
	frames[cwbuf].pnext = count;

	//olLog("Rendered frame! %d\n", cwbuf);
    cwbuf = (cwbuf+ 1) % fbufs;

    //return count / (float)params.rate;
    return (float)params.rate / count; // return fps
}


void ldLibol::olLoadIdentity(void)
{
	static const float identity[3][3] = {
		{1,0,0},
		{0,1,0},
		{0,0,1}
	};
	memcpy(&mtx2d[0][0], &identity[0][0], sizeof(mtx2d));
}

void ldLibol::olRotate(float theta)
{
	float rot[9] = {
		cosf(theta),-sinf(theta),0,
		sinf(theta),cosf(theta),0,
		0,0,1,
	};
	olMultMatrix(rot);
}

void ldLibol::olTranslate(float x, float y)
{
	float trans[9] = {
		1,0,0,
		0,1,0,
		x,y,1,
	};
	olMultMatrix(trans);
}

void ldLibol::olScale(float sx, float sy)
{
	float scale[9] = {
		sx,0,0,
		0,sy,0,
		0,0,1,
	};
	olMultMatrix(scale);
}

void ldLibol::olMultMatrix(float m[9])
{
	float _new[3][3];

	_new[0][0] = mtx2d[0][0]*m[0] + mtx2d[0][1]*m[1] + mtx2d[0][2]*m[2];
	_new[0][1] = mtx2d[0][0]*m[3] + mtx2d[0][1]*m[4] + mtx2d[0][2]*m[5];
	_new[0][2] = mtx2d[0][0]*m[6] + mtx2d[0][1]*m[7] + mtx2d[0][2]*m[8];
	_new[1][0] = mtx2d[1][0]*m[0] + mtx2d[1][1]*m[1] + mtx2d[1][2]*m[2];
	_new[1][1] = mtx2d[1][0]*m[3] + mtx2d[1][1]*m[4] + mtx2d[1][2]*m[5];
	_new[1][2] = mtx2d[1][0]*m[6] + mtx2d[1][1]*m[7] + mtx2d[1][2]*m[8];
	_new[2][0] = mtx2d[2][0]*m[0] + mtx2d[2][1]*m[1] + mtx2d[2][2]*m[2];
	_new[2][1] = mtx2d[2][0]*m[3] + mtx2d[2][1]*m[4] + mtx2d[2][2]*m[5];
	_new[2][2] = mtx2d[2][0]*m[6] + mtx2d[2][1]*m[7] + mtx2d[2][2]*m[8];

	memcpy(&mtx2d[0][0], &_new[0][0], sizeof(mtx2d));
}

void ldLibol::olPushMatrix(void)
{
	memcpy(&mtx2ds[mtx2dp][0][0], &mtx2d[0][0], sizeof(mtx2d));
	mtx2dp++;
}

void ldLibol::olPopMatrix(void)
{
	mtx2dp--;
	memcpy(&mtx2d[0][0], &mtx2ds[mtx2dp][0][0], sizeof(mtx2d));
}

void ldLibol::olLoadIdentity3(void)
{
	static const float identity[4][4] = {
		{1,0,0,0},
		{0,1,0,0},
		{0,0,1,0},
		{0,0,0,1},
	};
	memcpy(&mtx3d[0][0], &identity[0][0], sizeof(mtx3d));
}

void ldLibol::olRotate3X(float theta)
{
	float rot[16] = {
		1,0,0,0,
		0,cosf(theta),sinf(theta),0,
		0,-sinf(theta),cosf(theta),0,
		0,0,0,1
	};
	olMultMatrix3(rot);
}

void ldLibol::olRotate3Y(float theta)
{
	float rot[16] = {
		cosf(theta),0,-sinf(theta),0,
		0,1,0,0,
		sinf(theta),0,cosf(theta),0,
		0,0,0,1
	};
	olMultMatrix3(rot);
}

void ldLibol::olRotate3Z(float theta)
{
	float rot[16] = {
		cosf(theta), sinf(theta), 0, 0,
		-sinf(theta), cosf(theta), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	olMultMatrix3(rot);
}

void ldLibol::olTranslate3(float x, float y, float z)
{
	float trans[16] = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		x,y,z,1,
	};
	olMultMatrix3(trans);
}


void ldLibol::olScale3(float sx, float sy, float sz)
{
	float trans[16] = {
		sx,0,0,0,
		0,sy,0,0,
		0,0,sz,0,
		0,0,0,1,
	};
	olMultMatrix3(trans);
}

void ldLibol::olMultMatrix3(float m[16])
{
	float _new[4][4];
	_new[0][0] = mtx3d[0][0]*m[ 0] + mtx3d[0][1]*m[ 1] + mtx3d[0][2]*m[ 2] + mtx3d[0][3]*m[ 3];
	_new[0][1] = mtx3d[0][0]*m[ 4] + mtx3d[0][1]*m[ 5] + mtx3d[0][2]*m[ 6] + mtx3d[0][3]*m[ 7];
	_new[0][2] = mtx3d[0][0]*m[ 8] + mtx3d[0][1]*m[ 9] + mtx3d[0][2]*m[10] + mtx3d[0][3]*m[11];
	_new[0][3] = mtx3d[0][0]*m[12] + mtx3d[0][1]*m[13] + mtx3d[0][2]*m[14] + mtx3d[0][3]*m[15];
	_new[1][0] = mtx3d[1][0]*m[ 0] + mtx3d[1][1]*m[ 1] + mtx3d[1][2]*m[ 2] + mtx3d[1][3]*m[ 3];
	_new[1][1] = mtx3d[1][0]*m[ 4] + mtx3d[1][1]*m[ 5] + mtx3d[1][2]*m[ 6] + mtx3d[1][3]*m[ 7];
	_new[1][2] = mtx3d[1][0]*m[ 8] + mtx3d[1][1]*m[ 9] + mtx3d[1][2]*m[10] + mtx3d[1][3]*m[11];
	_new[1][3] = mtx3d[1][0]*m[12] + mtx3d[1][1]*m[13] + mtx3d[1][2]*m[14] + mtx3d[1][3]*m[15];
	_new[2][0] = mtx3d[2][0]*m[ 0] + mtx3d[2][1]*m[ 1] + mtx3d[2][2]*m[ 2] + mtx3d[2][3]*m[ 3];
	_new[2][1] = mtx3d[2][0]*m[ 4] + mtx3d[2][1]*m[ 5] + mtx3d[2][2]*m[ 6] + mtx3d[2][3]*m[ 7];
	_new[2][2] = mtx3d[2][0]*m[ 8] + mtx3d[2][1]*m[ 9] + mtx3d[2][2]*m[10] + mtx3d[2][3]*m[11];
	_new[2][3] = mtx3d[2][0]*m[12] + mtx3d[2][1]*m[13] + mtx3d[2][2]*m[14] + mtx3d[2][3]*m[15];
	_new[3][0] = mtx3d[3][0]*m[ 0] + mtx3d[3][1]*m[ 1] + mtx3d[3][2]*m[ 2] + mtx3d[3][3]*m[ 3];
	_new[3][1] = mtx3d[3][0]*m[ 4] + mtx3d[3][1]*m[ 5] + mtx3d[3][2]*m[ 6] + mtx3d[3][3]*m[ 7];
	_new[3][2] = mtx3d[3][0]*m[ 8] + mtx3d[3][1]*m[ 9] + mtx3d[3][2]*m[10] + mtx3d[3][3]*m[11];
	_new[3][3] = mtx3d[3][0]*m[12] + mtx3d[3][1]*m[13] + mtx3d[3][2]*m[14] + mtx3d[3][3]*m[15];
	memcpy(&mtx3d[0][0], &_new[0][0], sizeof(mtx3d));
}

void ldLibol::olPushMatrix3(void)
{
	memcpy(&mtx3ds[mtx3dp][0][0], &mtx3d[0][0], sizeof(mtx3d));
	mtx3dp++;
}

void ldLibol::olPopMatrix3(void)
{
	mtx3dp--;
	memcpy(&mtx3d[0][0], &mtx3ds[mtx3dp][0][0], sizeof(mtx3d));
}

void ldLibol::olTransformVertex4(float *x, float *y, float *z, float *w) const
{
	float px;
	float py;
	float pz;
	float pw;

	px = mtx3d[0][0]**x + mtx3d[0][1]**y + mtx3d[0][2]**z + mtx3d[0][3]**w;
	py = mtx3d[1][0]**x + mtx3d[1][1]**y + mtx3d[1][2]**z + mtx3d[1][3]**w;
	pz = mtx3d[2][0]**x + mtx3d[2][1]**y + mtx3d[2][2]**z + mtx3d[2][3]**w;
	pw = mtx3d[3][0]**x + mtx3d[3][1]**y + mtx3d[3][2]**z + mtx3d[3][3]**w;

	*x = px;
	*y = py;
	*z = pz;
	*w = pw;
}

void ldLibol::olTransformVertex3(float *x, float *y, float *z) const
{
	float w = 1.0;

	olTransformVertex4(x, y, z, &w);

	*x /= w;
	*y /= w;
	*z /= w;
}

void ldLibol::olVertex3(float x, float y, float z, uint32_t color, int repeat)
{
    if(v3shader)
        v3shader(&x, &y, &z, &color);
    olTransformVertex3(&x, &y, &z);
    if (z == 0)
		z = SMALL_Z; // Hack for code not clipping z
	for(int i = 0; i < repeat; i++) {
		olVertex2Z(x, y, 1.0 / z, color);
	}
}

void ldLibol::olRect(float x1, float y1, float x2, float y2, uint32_t color)
{
	olBegin(OL_LINESTRIP);
	olVertex(x1,y1,color);
	olVertex(x1,y2,color);
	olVertex(x2,y2,color);
	olVertex(x2,y1,color);
	olVertex(x1,y1,color);
	olEnd();
}

void ldLibol::olLine(float x1, float y1, float x2, float y2, uint32_t color)
{
	olBegin(OL_LINESTRIP);
	olVertex(x1,y1,color);
	olVertex(x2,y2,color);
	olEnd();
}


void ldLibol::olDot(float x, float y, int samples, uint32_t color)
{
	int i;
	olBegin(OL_POINTS);
	for (i = 0; i < samples; i++)
		olVertex(x,y,color);
	olEnd();
}

void ldLibol::olResetColor(void)
{
	curcol = C_WHITE;
}

void ldLibol::olMultColor(uint32_t color)
{
	curcol = colmul(curcol, color);
}

void ldLibol::olPushColor(void)
{
	cols[coldp] = curcol;
	coldp++;
}

void ldLibol::olPopColor(void)
{
	coldp--;
	curcol = cols[coldp];
}

void ldLibol::olSetVertexPreShader(ShaderFunc f)
{
	vpreshader = f;
}

void ldLibol::olSetVertexShader(ShaderFunc f)
{
	vshader = f;
}

void ldLibol::olSetVertex3Shader(Shader3Func f)
{
	v3shader = f;
}

void ldLibol::olSetPixelShader(ShaderFunc f)
{
	pshader = f;
}

void ldLibol::olSetPixel3Shader(Shader3Func f)
{
	p3shader = f;
}

void ldLibol::olFrustum (float l, float r, float b, float t, float n, float f)
{
	float m[16] = {
		(2*n)/(r-l),  0,            0,               0,
		0,            (2*n)/(t-b),  0,               0,
		(r+l)/(r-l),  (t+b)/(t-b),  -(f+n)/(f-n),   -1,
		0,            0,            (-2*f*n)/(f-n),  0,
	};

	olMultMatrix3(m);
}

void ldLibol::olPerspective (float fovy, float aspect, float zNear, float zFar)
{
	float xmin, xmax, ymin, ymax;

	ymax = zNear * tanf(fovy * M_PI / 360.0);
	ymin = -ymax;

	xmin = ymin * aspect;
	xmax = ymax * aspect;

	olFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}

void ldLibol::olSetScissor (float x0, float y0, float x1, float y1)
{
	bbox[0][0] = x0;
	bbox[0][1] = y0;
	bbox[1][0] = x1;
	bbox[1][1] = y1;
}

void ldLibol::olGetFrameInfo(OLFrameInfo *info) const
{
	*info = last_info;
}

void ldLibol::olLog(const char *fmt, ...) const
{
	char buf[1024];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, 1024, fmt, ap);
	va_end(ap);
	buf[1023] = 0;

	if (log_cb)
		log_cb(buf);
	else
		printf("%s", buf);
}

void ldLibol::olSetLogCallback(LogCallbackFunc f)
{
	log_cb = f;
}

ldLibol::OLRenderedFrame * ldLibol::olGetRenderedFrames() const
{
	return frames;
}

