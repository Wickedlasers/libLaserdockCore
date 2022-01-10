//
// Created by Darren Otgaar on 2018/10/19.
//

#ifndef LASERDOCKCORE_LDLINECLIPPER_H
#define LASERDOCKCORE_LDLINECLIPPER_H

// This class is responsible for providing line clipping algorithms to cull all lines not within the viewport.  This
// removes the majority of the lines and clips all those intersecting the viewport (defined as an AABB or rect).

#include "ldRazerVec2.h"
#include "ldLine.h"
#include "ldAABB.h"
#include "ldMesh.h"

namespace ldRazer
{

    enum regionCode
    {
        RC_INSIDE = 0,
        RC_LEFT = 1 << 0,
        RC_RIGHT = 1 << 1,
        RC_BOTTOM = 1 << 2,
        RC_TOP = 1 << 3
    };

    template<typename T>
    inline uint32_t getRegionCode(const ldRazerVec2<T> &P, const ldAABB<T> &bound)
    {
        uint32_t code = RC_INSIDE;
        if (P.x < bound.left()) code |= RC_LEFT;
        if (P.x > bound.right()) code |= RC_RIGHT;
        if (P.y < bound.bottom()) code |= RC_BOTTOM;
        if (P.y > bound.top()) code |= RC_TOP;
        return code;
    }

    template<typename T>
    bool lineClipper(const ldLine<T> &line, const ldAABB<T> &bound, ldLine<T> &newLine)
    {
        auto cA = getRegionCode(line.A, bound), cB = getRegionCode(line.B, bound);
        newLine = line;
        bool clipped = false;

        while (true) {
            if (cA & cB) break;
            else if (!(cA | cB)) {
                clipped = true;
                break;
            } else {
                T x = 0, y = 0;
                auto cN = cA ? cA : cB;
                if (cN & RC_TOP) {
                    x = newLine.A.x +
                        (newLine.B.x - newLine.A.x) * (bound.top() - newLine.A.y) / (newLine.B.y - newLine.A.y);
                    y = bound.top();
                } else if (cN & RC_BOTTOM) {
                    x = newLine.A.x +
                        (newLine.B.x - newLine.A.x) * (bound.bottom() - newLine.A.y) / (newLine.B.y - newLine.A.y);
                    y = bound.bottom();
                } else if (cN & RC_RIGHT) {
                    x = bound.right();
                    y = newLine.A.y +
                        (newLine.B.y - newLine.A.y) * (bound.right() - newLine.A.x) / (newLine.B.x - newLine.A.x);
                } else if (cN & RC_LEFT) {
                    x = bound.left();
                    y = newLine.A.y +
                        (newLine.B.y - newLine.A.y) * (bound.left() - newLine.A.x) / (newLine.B.x - newLine.A.x);
                }

                if (cN == cA) {
                    newLine.A.set(x, y);
                    cA = getRegionCode(newLine.A, bound);
                } else {
                    newLine.B.set(x, y);
                    cB = getRegionCode(newLine.B, bound);
                }
            }
        }

        return clipped;
    }

    ldMesh2f_t clipMesh(const ldMesh2f_t &mesh, const AABB2f_t &bound, bool strict = false);

}

#endif //LASERDOCKCORE_LDLINECLIPPER_H
