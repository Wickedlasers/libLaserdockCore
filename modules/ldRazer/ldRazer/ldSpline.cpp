//
// Created by Darren Otgaar on 2018/10/07.
//

#include "ldRazer.h"
#include "ldSpline.h"
#include "ldColourSpline.h"

#include <QDebug>

namespace ldRazer
{

    using U_t = ldMatrix<float, 1, 4>;
    using M_t = ldMatrix<float, 4, 4>;
    using G_t = ldMatrix<vec2f, 4, 1>;

// Transposed because std::initializer initialises in row-major format
    const M_t ldSpline::M{{
                                  -.5f, 1.f, -.5f, .0f,
                                  1.5f, -2.5f, 0.f, 1.f,
                                  -1.5f, 2.f, .5f, 0.f,
                                  .5f, -.5f, 0.f, 0.f
                          }};

    const M_t ldColourSpline::M{{
                                        -.5f, 1.f, -.5f, .0f,
                                        1.5f, -2.5f, 0.f, 1.f,
                                        -1.5f, 2.f, .5f, 0.f,
                                        .5f, -.5f, 0.f, 0.f
                                }};

    ldSpline::ldSpline() = default;

    ldSpline::ldSpline(const ldSpline::vertices_t &verts, bool pIsLoop) : vertices(verts), isLoop(pIsLoop)
    {
        speeds.resize(verts.size());
        const auto inv = 1.f / (isLoop ? verts.size() : verts.size() - 1);
        auto total = 0.f;

        // Uniform segment traversal (use midpoint method?)
        for (size_t i = 0; i != speeds.size(); ++i) {
            speeds[i] = total;
            total = speeds[i] + inv;
        }
    }

    vec2f ldSpline::pos(float u) const
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

        return (U * (M * G))[0];
    }

    vec2f ldSpline::tangent(float u, float step) const
    {
        return 1.f / step * (pos(u + step) - pos(u));
    }

    vec2f ldSpline::dir(float u, float step) const
    {
        return normalise(tangent(u, step));
    }

    vec2f ldSpline::normal(float u, float step) const
    {
        return perp(dir(u, step));
    }

    float ldSpline::distance(float u, float /*step*/) const
    {
        u = clamp(u, 0.f, 1.f);
        return speeds[findInterval(speeds, u)];
    }

    float ldSpline::normalise_speed(float step)
    {
        float total = 0.f;
        float t = 0.f;
        auto copy = speeds;
        copy.emplace_back(1.f);
        std::vector<float> distance(copy.size(), 0.f);
        for (size_t i = 0; i != copy.size(); ++i) {
            while (t < copy[i]) {
                total += (pos(t + step) - pos(t)).length();
                t += step;
            }
            distance[i] = total;
        }

        const float inv_total = 1.f / total;
        for (auto &f : distance) f *= inv_total;

        /*
        for(size_t i = 0; i != distance.size(); ++i) {
            qDebug() << "O:" << speeds[i] << isZero(speeds[i]) << "N:" << distance[i];
        }

        qDebug() << "\n\n";
        */

        distance.pop_back();
        speeds = distance;
        return total;
    }

}

