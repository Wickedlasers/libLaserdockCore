//
// Created by Darren Otgaar on 2018/10/07.
//

#ifndef LASERDOCKCORE_LDSPLINE_H
#define LASERDOCKCORE_LDSPLINE_H

#include "ldRazer.h"

namespace ldRazer
{
    /*
     * A normalised Catmull-Rom spline
     */

    struct ldSpline
    {
        using vertices_t = std::vector<vec2f>;
        using speeds_t = std::vector<float>;

        ldSpline();

        ldSpline(const vertices_t &verts, bool isLoop = false);

        vec2f pos(float u) const;

        vec2f tangent(float u, float step = .01f) const;

        vec2f dir(float u, float step = .01f) const;

        vec2f normal(float u, float step = .01f) const;

        float distance(float u, float step = .01f) const;

        // Numerical integration of the spline.
        float normalise_speed(float step = .001f);

        vertices_t vertices;
        speeds_t speeds;
        bool isLoop;

        static const ldMatrix<float, 4, 4> M;
    };
}

#endif //LASERDOCKCORE_LDSPLINE_H
