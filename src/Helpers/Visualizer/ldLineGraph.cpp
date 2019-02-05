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

#include "ldCore/Helpers/Visualizer/ldLineGraph.h"

#include <ldCore/Helpers/Color/ldColorUtil.h>

ldLineGraph::ldLineGraph(char* n) {
    memset(this, 0, sizeof(*this));
    name = n;
    color = ldColorUtil::colorHSV(randRange(0, 360), 1, 1);
    head = 0;
    zoom = 0;
}

void ldLineGraph::add(float f) {
    clampfp(f, 0, 1);
    head = (head + 1) % range;
    floats[head] = f;
}

void ldLineGraph::draw(ldRendererOpenlase* r) {
    if (!enabled) return;
    r->begin(OL_LINESTRIP);
    int rangee = range / ((1-sqrt(zoom))*7+1);
    int skip = rangee/200+1;
    for (int i = 0; i < rangee; i += skip) {
        float x = (float(i) / rangee);
        int index = (head - i + range) % range;
        float y = floats[index];
        float px = (1-x)*1.8+0.1;
        float py = y*1.8+0.1;
        px = MIN(MAX(0, px), 2);
        py = MIN(MAX(0, py), 2);
        r->vertex(px, py, color);
        if (i == 0 ||  i == (rangee-1))
           r->vertex((1-x)*1.8+0.1, y*1.8+0.1, color);
    }
    r->end();
}
