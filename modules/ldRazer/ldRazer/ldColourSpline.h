//
// Created by Darren Otgaar on 2018/11/13.
//

#ifndef LASERDOCKCORE_LDCOLOURSPLINE_H
#define LASERDOCKCORE_LDCOLOURSPLINE_H

#include "ldRazer.h"
#include "ldVector.h"
#include "ldMatrix.h"
#include <QDebug>

namespace ldRazer
{

    inline ldVector<float, 3> colourToVector(uint32_t colour)
    {
        return ldVector<float, 3>{{
                                          ((colour & 0x00FF0000) >> 16) / 255.99f,
                                          ((colour & 0x0000FF00) >> 8) / 255.99f,
                                          (colour & 0x000000FF) / 255.99f
                                  }};
    }

    inline uint32_t vectorToColour(const ldVector<float, 3> &v)
    {
        return makeARGB(
                uint8_t(clamp(v[0] * 255, 0.f, 255.f)),
                uint8_t(clamp(v[1] * 255, 0.f, 255.f)),
                uint8_t(clamp(v[2] * 255, 0.f, 255.f)));
    }

    struct ldColourSpline
    {
        using vector_t = ldVector<float, 3>;
        using vertices_t = std::vector<uint32_t>;
        using vvertices_t = std::vector<vector_t>;
        using speeds_t = std::vector<float>;

        using U_t = ldMatrix<float, 1, 4>;
        using M_t = ldMatrix<float, 4, 4>;
        using G_t = ldMatrix<vector_t, 4, 1>;

        ldColourSpline() = default;

        ldColourSpline(const ldColourSpline::vertices_t &verts, bool pIsLoop = false) : isLoop(pIsLoop)
        {
            vertices.resize(verts.size());
            speeds.resize(verts.size());

            std::transform(verts.begin(), verts.end(), vertices.begin(), [](const uint32_t &a)
            {
                return colourToVector(a);
            });

            const auto inv = 1.f / (isLoop ? verts.size() : verts.size() - 1);
            auto total = 0.f;

            for (size_t i = 0; i != speeds.size(); ++i) {
                speeds[i] = total;
                total = speeds[i] + inv;
            }
        }

        uint32_t pos(float u) const
        {
            u = clamp(u, 0.f, 1.f);
            const auto count = vertices.size();
            const auto idx = findInterval(speeds, u);
            const auto end = isLoop && idx == speeds.size() - 1 ? 1.f : speeds[idx + 1];

            u = (u - speeds[idx]) / (end - speeds[idx]);
            const U_t U{{u * u * u, u * u, u, 1.f}};
            G_t G;
            if (isLoop) {
                G = G_t{{
                                vertices[idx == 0 ? count - 1 : idx - 1],
                                vertices[idx],
                                vertices[(idx + 1) % count],
                                vertices[(idx + 2) % count]
                        }};
            } else if (idx == 0) {
                G = G_t{{
                                vertices[1] - (vertices[2] - vertices[1]),
                                vertices[0],
                                vertices[1],
                                vertices[2]
                        }};
            } else if (idx == count - 2) {
                G = G_t{{
                                vertices[idx - 1],
                                vertices[idx],
                                vertices[idx + 1],
                                vertices[idx + 1] - (vertices[idx] - vertices[idx + 1])
                        }};
            } else if (idx < count - 2) {
                G = G_t{{
                                vertices[idx - 1],
                                vertices[idx],
                                vertices[idx + 1],
                                vertices[idx + 2]
                        }};
            }

            vector_t vec = (U * (M * G))[0];
            return vectorToColour(vec);
        }

        vvertices_t vertices;
        speeds_t speeds;
        bool isLoop;

        static const ldMatrix<float, 4, 4> M;
    };
}

#endif //LASERDOCKCORE_LDCOLOURSPLINE_H
